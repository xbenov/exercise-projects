import numpy as np
import gui

class Game():

    def __init__(self) -> None:
        # Initialize game
        self.win = gui.GUI()
        self.init_game()
        self.scores = [0,0,0] # [o,x,ties]
        
    def init_game(self):
        self.win.clear_board()
        self.win.window.bind('<Button-1>', self.click)

        self.state = np.zeros(shape=(3,3),dtype=int)
        self.player_x_turn = 1        

    def start(self):
        # Start the GUI loop
        self.win.start_loop()

    def who_won(self):
        # Return winner if found
        for n in range(3):
            if (self.state[n][0] == self.state[n][1] == self.state[n][2]) and self.state[n][0] != 0:
                return self.state[n][0]
            if (self.state[0][n] == self.state[1][n] == self.state[2][n]) and self.state[0][n] != 0:
                return self.state[0][n]

        if (self.state[0][0] == self.state[1][1] == self.state[2][2]) and self.state[1][1] != 0:
            return self.state[1][1]
        if (self.state[0][2] == self.state[1][1] == self.state[2][0]) and self.state[1][1] != 0:
            return self.state[1][1]

        # Check tie
        for n in range(3):
            for m in range(3):
                if self.state[n][m] == 0:
                    # No winner or tie found
                    return 0
        # Its a tie
        return 3

    def winner_scr(self,winner):
        self.scores[winner - 1] += 1
        print('X wins' if winner == 2 else 'O wins' if winner == 1 else 'Tie')
        self.win.result_screen(winner,self.scores)
        self.win.window.bind('<Button-1>', self.reset_game)

    def reset_game(self,event):
         self.init_game()
    
    def click(self, event):
        # Get click position and transform to state position
        s_pos = [int(event.x // (self.win.size_board / 3)), int(event.y // (self.win.size_board / 3))]
        
        if self.player_x_turn:
            # Check if we can change state and draw
            
            if not self.state[s_pos[1]][s_pos[0]]:
                self.state[s_pos[1]][s_pos[0]] = 2
                self.player_x_turn = 0
        else:
            if not self.state[s_pos[1]][s_pos[0]]:
                self.state[s_pos[1]][s_pos[0]] = 1
                self.player_x_turn = 1

        self.win.draw_state(self.state)

        # Check if someone won
        winner=self.who_won()
        if winner:
            # Announce winner
            self.winner_scr(winner)
            print(self.state)

if __name__ == "__main__":
    game = Game()
    game.start()