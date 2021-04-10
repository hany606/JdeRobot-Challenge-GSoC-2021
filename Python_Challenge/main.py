# -----------------------------------------------
# Author: Hany Hamed
# Description: This is the main file for the user of the game
# -----------------------------------------------
from GameOfLife import GameOfLife

g = GameOfLife((75,75))
g.random()
g.game_gui()
