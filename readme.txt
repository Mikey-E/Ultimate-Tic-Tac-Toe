On windows the game can be started by running "uttt_windows.exe".
On linux it can be started by running "uttt_linux".

For those who want to compile from source code:
On windows the game may be compiled by running "cwindows.bat" in this directory.
On linux the game may be compiled by running "clinux.sh" in this directory.

Advice: it's a good idea to right-click->properties and change the font and/or size of your windows command prompt so you aren't looking at a very tiny game board.

Flags:
-h	help
-s	seconds of sleep between turns (default 1)
-r	a specific random number seed [1, 4294967295]. 0 means base on current time
-1	set first agent type (1,2,3). MCTS can be followed by a search depth (2-9), example: "-1 32" to make first agent MCTS with depth 2.
-2	set second agent type.
-f	set custom location and/or name of log file (fully qualified path or name in this local directory).
-g	specify amount of games to play in a batch.
-q	Quiet. Does not display the board. Makes large batches run faster.

RULES:
Conquer 3 sectors just like in normal tic-tac-toe to achieve victory.
Each sector is its own tic-tac-toe game.
Where you play in each sector corresponds to the next sector that your opponent has to play in,
if that sector is full or conquered, then your opponent may play anywhere.
