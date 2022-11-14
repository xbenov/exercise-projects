import tkinter as tk

class GUI:
    size_board = 900
    symbol_size = (size_board/3 - size_board/5)/2
    symbol_thickness = 60
    symbol_color_red = '#EE4035'
    symbol_color_blue = '#0492CF'

    def __init__(self) -> None:

        #create and set main window
        self.window = tk.Tk()
        self.window.title('Tic-Tac-Toe')
        self.setCentre(self.window)
        
        self.canvas = tk.Canvas(self.window,width=GUI.size_board,height=GUI.size_board)
        self.canvas.pack()

        

    def setCentre(self,window):
        # get screen width and height
        ws = window.winfo_screenwidth() # width of the screen
        hs = window.winfo_screenheight() # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws/2) - (GUI.size_board/2)
        y = (hs/2) - (GUI.size_board/2)
        self.window.geometry(f"{GUI.size_board}x{GUI.size_board}+{int(x)}+{int(y)}")

    def start_loop(self):
        #self.draw_state([[0,1,1],[1,1,1],[0,1,0]])
        self.window.mainloop()


    def clear_board(self):
        #Resets board
        self.canvas.delete("all")
        
        for n in range(2):
            self.canvas.create_line((n+1)*(GUI.size_board/3),0,(n+1)*(GUI.size_board/3),GUI.size_board)
            self.canvas.create_line(0,(n+1)*(GUI.size_board/3),GUI.size_board,(n+1)*(GUI.size_board/3))
    
    def result_screen(self,winner,scores):
        win_text = f"{'Winner is X' if winner == 2 else 'Winner is O' if winner == 1 else 'Its a tie'}"
        px_stat = f"Player O wins: {scores[0]}"
        po_stat = f"Player X wins: {scores[1]}"
        pt_stat = f"Ties: {scores[2]}"
        
        self.canvas.delete("all")
        self.canvas.create_text(GUI.size_board/2,GUI.size_board/2 - 50, text=win_text,font=('Arial',50))
        self.canvas.create_text(GUI.size_board/2,GUI.size_board/2 + 30, text=px_stat,font=('Arial',20))
        self.canvas.create_text(GUI.size_board/2,GUI.size_board/2 + 70, text=po_stat,font=('Arial',20))
        self.canvas.create_text(GUI.size_board/2,GUI.size_board/2 + 110, text=pt_stat,font=('Arial',20))

    def draw_state(self, state):
        self.clear_board()

        for y,row in enumerate(state):
            for x,value in enumerate(row):
                if value == 1:
                    # draw O at position x,y
                    self.canvas.create_oval((x+1)*(GUI.size_board/3) - GUI.symbol_size, (y+1)*(GUI.size_board/3) - GUI.symbol_size,
                                (x)*(GUI.size_board/3) + GUI.symbol_size, (y)*(GUI.size_board/3) + GUI.symbol_size, width=GUI.symbol_thickness,
                                outline=GUI.symbol_color_red)
                elif value == 2:
                    # draw X at position x,y
                    self.canvas.create_line((x+1)*(GUI.size_board/3) - GUI.symbol_size, (y+1)*(GUI.size_board/3) - GUI.symbol_size,
                                (x)*(GUI.size_board/3) + GUI.symbol_size, (y)*(GUI.size_board/3) + GUI.symbol_size, width=GUI.symbol_thickness,
                                fill=GUI.symbol_color_blue)
                    self.canvas.create_line((x+1)*(GUI.size_board/3) - GUI.symbol_size, (y)*(GUI.size_board/3) + GUI.symbol_size,
                                (x)*(GUI.size_board/3) + GUI.symbol_size, (y+1)*(GUI.size_board/3) - GUI.symbol_size, width=GUI.symbol_thickness,
                                fill=GUI.symbol_color_blue)







