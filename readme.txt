On windows the game may be compiled by running "cg" in this directory. It can be started by running "uttt".

On linux you may have to comment out "#include <windows.h>" (insert "//" before it in the same line) in core.h before compiling.
Compile with the first line in cg.bat
Run: ./uttt

Advice: it's a good idea to right-click->properties and change the font and/or size of your windows command prompt so you aren't looking at a very tiny game board.

Flags:
-h	help
-s	seconds of sleep between turns (default 1)
-r	a specific random number seed [1, 4294967295]. 0 means base on current time

RULES:
Conquer 3 sectors just like in normal tic-tac-toe to achieve victory.
Each sector is its own tic-tac-toe game.
Where you play in each sector corresponds to the next sector that your opponent has to play in,
if that sector is full or conquered, then your opponent may play anywhere.