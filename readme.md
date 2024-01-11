# Ultimate Tic-Tac-Toe

### How to play:
Conquer 3 sectors just like in normal tic-tac-toe to achieve victory.
Each sector is its own tic-tac-toe game.
Where you play in each sector corresponds to the next sector that your opponent has to play in,
if that sector is full or conquered, then your opponent may play anywhere. Arrows (^^^) point to where you can make a next move. A conquered sector gets all locations set to the character of the player that conquered it.

Agents may be set to either human, random (plays randomly), or [Monte Carlo Tree Search](https://en.wikipedia.org/wiki/Monte_Carlo_tree_search) (written from scratch in C!) where you can set the search depth of the tree to control how smart it is.

Playing a game will log some info about what happened in that game in a history file.

### The simple way(s) to run:
On windows the game can be started by running "uttt_windows.exe".

On linux it can be started by running "./uttt_linux".

### For those who want to compile from source code:
On windows the game may be compiled by running "cwindows.bat" in this directory.

On linux the game may be compiled by running "./clinux.sh" in this directory.

### Advice:
It's a good idea to right-click->properties and change the font and/or size of your windows command prompt so you aren't looking at a very tiny game board.

Some console fonts/settings may cause the board to print out with inconsistent spacing. If this happens, try switching to the font "consolas".

### Optional command line flags:
| Flag | Description                                              |
|--------|----------------------------------------------------------|
| -h     | Help                                                     |
| -s     | Seconds of sleep between turns (default 1).              |
| -r     | A specific random number seed [1, 4294967295]. 0 means base on current time. |
| -1     | Set first agent type (1,2,3). MCTS can be followed by a search depth (2-9), example: "-1 32" to make first agent MCTS with depth 2. |
| -2     | Set second agent type.                                   |
| -f     | Set custom location and/or name of log file (fully qualified path or name in this local directory). |
| -g     | Specify amount of games to play in a batch.              |
| -q     | Quiet. Does not display the board. Makes large batches run faster. |
