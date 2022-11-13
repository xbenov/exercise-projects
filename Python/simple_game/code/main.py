import numpy as np
import gui

class Game():

    def __init__(self) -> None:
        # Initialize game
        self.win = gui.GUI()
        self.win.clear_board()

        self.state = np.zeros(shape=(3,3))
        self.player_x = 0
        self.player_y = 0

    def start(self):
        # Start the GUI loop
        self.win.start_loop()

    



if __name__ == "__main__":
    game = Game()
    game.start()