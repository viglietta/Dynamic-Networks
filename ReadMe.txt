COMMAND LINE

anonymity [filename]
Load the network stored in the given file. If no filename is provided or the file cannot be opened, "DefaultNetwork.txt" is loaded.


MOUSE COMMANDS

Left-click: select/deselect entity or view
Mouse wheel up/down: decrement/increment current round


NETWORK WINDOW

Left-click on an entity and drag to draw a new link; release mouse button to confirm.
Hold CTRL while creating a link to decrement the multiplicity of that link.
Hold ALT while creating/deleting a link to affect all rounds.
Hold SHIFT while creating/deleting a link to make it two-way.
Right-click on an entity and drag to move it.
Right-click away from entities to create a new entity.


HOTKEYS

TAB: select counting algorithm
SPACE: execute counting algorithm step by step
ESC: deselect entities and view
UP/DOWN: decrement/increment current round
LEFT/RIGHT: select different anonymity class
BACKSPACE: delete all interactions in current round
DEL: delete selected entities
+/-: insert/delete round
A: toggle arrows
L: change red edge drawing mode
B: draw round/square nodes
H: toggle highlight current level
D: snap entities to grid
0..9: change state of selected entities
CAPS-LOCK: create double links by default
F1: load network from "DefaultNetwork.txt"
F2: save network to "DefaultNetwork.txt"


See https://arxiv.org/abs/2204.02128 for more information.