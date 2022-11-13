import tkinter as tk

size_board = 900
symbol_size = (size_board/3 - size_board/5)/2
symbol_thickness = 30
symbol_color_red = '#EE4035'
symbol_color_blue = '#0492CF'

class GUI:

    def __init__(self) -> None:

        #create and set main window
        self.window = tk.Tk()
        self.window.title('Tic-Tac-Toe')

        self.setCentre(self.window)
        
        self.canvas = tk.Canvas(self.window,width=size_board,height=size_board)
        self.canvas.pack()

        self.window.bind('<Button-1>', self.click)

    def setCentre(self,window):
        # get screen width and height
        ws = window.winfo_screenwidth() # width of the screen
        hs = window.winfo_screenheight() # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws/2) - (size_board/2)
        y = (hs/2) - (size_board/2)
        self.window.geometry(f"{size_board}x{size_board}+{int(x)}+{int(y)}")

    def start_loop(self):
        self.draw_state([[0,1,1],[1,1,1],[0,1,0]])
        self.window.mainloop()


    def clear_board(self):
        #Resets board
        self.canvas.delete("all")

        for n in range(2):
            self.canvas.create_line((n+1)*(size_board/3),0,(n+1)*(size_board/3),size_board)
            self.canvas.create_line(0,(n+1)*(size_board/3),size_board,(n+1)*(size_board/3))
    

    def draw_state(self, state):

        for y,row in enumerate(state):
            for x,value in enumerate(row):
                if value == 0:
                    # draw O at position x,y
                    self.canvas.create_oval((x+1)*(size_board/3) - symbol_size, (y+1)*(size_board/3) - symbol_size,
                                (x)*(size_board/3) + symbol_size, (y)*(size_board/3) + symbol_size, width=symbol_thickness,
                                outline=symbol_color_red)
                elif value == 1:
                    # draw X at position x,y
                    self.canvas.create_line((x+1)*(size_board/3) - symbol_size, (y+1)*(size_board/3) - symbol_size,
                                (x)*(size_board/3) + symbol_size, (y)*(size_board/3) + symbol_size, width=symbol_thickness,
                                fill=symbol_color_blue)
                    self.canvas.create_line((x+1)*(size_board/3) - symbol_size, (y)*(size_board/3) + symbol_size,
                                (x)*(size_board/3) + symbol_size, (y+1)*(size_board/3) - symbol_size, width=symbol_thickness,
                                fill=symbol_color_blue)


    def click(self, event):
        pass




