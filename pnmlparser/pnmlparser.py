import os
import xml.etree.ElementTree as ET

from jinja2 import Environment, FileSystemLoader


TEMPLATES_DIRECTORY = os.path.abspath(os.path.dirname(os.path.dirname(__file__)))


class PetriNet(object):

    def __init__(self, transitions, initial_marking, is_one_safe=False):
        self.transitions     = transitions
        self.initial_marking = initial_marking
        self.is_one_safe     = is_one_safe

    @property
    def place_count(self):
        return len(self.transitions[0]['pre']) if self.transitions else 0

    @property
    def transition_count(self):
        return len(self.transitions)

    def export(self):
        jinja_env = Environment(
            loader        = FileSystemLoader(TEMPLATES_DIRECTORY),
            trim_blocks   = True,
            lstrip_blocks = True)

        template = jinja_env.get_template('model.hh.template')
        return template.render(pn=self)

    @staticmethod
    def from_pnml(filename):
        # Parse the PNML file, stripping all namespaces.
        tree = ET.iterparse(filename)
        for _, el in tree:
                el.tag = el.tag.split('}', 1)[1]
        root = tree.root

        nets = {}
        for net_node in root:
            # As we list the places, we associate each of them with an index,
            # so we can forget about the place names afterwards.
            index_from_id = {}
            place_tokens  = {}
            place_names   = {}

            for place_index, place_node in enumerate(net_node.iter('place')):
                try:
                    tokens = int(place_node.find('./initialMarking/text').text)
                except AttributeError:
                    tokens = 0

                if tokens > 1:
                    is_one_safe = False

                index_from_id[place_node.get('id')] = place_index
                place_tokens[place_index] = tokens
                place_names[place_index]  = place_node.find('./name/text').text

            initial_marking = [place_tokens[i] for i in range(len(place_tokens))]

            # Get the list of transitions.
            transitions = {}
            for transition_node in net_node.iter('transition'):
                transitions[transition_node.get('id')] = transition_node.find('./name/text').text

            # Build the pre/post matrices.
            pre  = {t: [0] * len(place_tokens) for t in transitions.values()}
            post = {t: [0] * len(place_tokens) for t in transitions.values()}
            for arc_node in net_node.iter('arc'):
                source = arc_node.get('source')
                target = arc_node.get('target')

                try:
                    tokens = int(arc_node.find('./inscription/text').text)
                except AttributeError:
                    tokens = 1

                # Identify whether the arc represents a pre- or post-condition.
                if source in index_from_id:
                    pre[transitions[target]][index_from_id[source]] = tokens
                else:
                    post[transitions[source]][index_from_id[target]] = tokens

            transitions = [{'pre' : pre[t], 'post': post[t]} for t in sorted(pre.keys())]
            nets[net_node.get('id')] = PetriNet(transitions, initial_marking)

        return nets


if __name__ == '__main__':
    import sys

    pns = PetriNet.from_pnml(sys.argv[1])
    pn  = next(iter(pns.values()))
    pn.is_one_safe = (len(sys.argv) > 2) and (sys.argv[2] == '--safe')

    with open('model.hh', 'w') as f:
        f.write(pn.export())
