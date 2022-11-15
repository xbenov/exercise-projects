import tkinter as tk
import numpy as np
import time


class GameOfLife():
    # Settings
    grid_size = 50
    cell_size = 20
    window_size = grid_size * cell_size

    def __init__(self) -> None:
        # GUI Window init
        self.window = tk.Tk()
        self.window.title('Game Of Life')
        self.setCentre(self.window,GameOfLife.window_size)
        self.canvas = tk.Canvas(self.window,width=GameOfLife.window_size,height=GameOfLife.window_size)
        self.canvas.pack()

        self.grid = self.getGrid(GameOfLife.grid_size)
        
    def setCentre(self,window,size):
        # get screen width and height
        ws = window.winfo_screenwidth() # width of the screen
        hs = window.winfo_screenheight() # height of the screen

        # calculate x and y coordinates for the Tk root window
        x = (ws/2) - (size/2)
        y = (hs/2) - (size/2)
        self.window.geometry(f"{size}x{size}+{int(x)}+{int(y)}")
        
    def getGrid(self,grid_size):
        return np.random.randint(0,2,size=(grid_size,grid_size))

    def draw(self,cell_size):
        # Draw grid
        self.canvas.delete("all")
        for y,row in enumerate(self.grid):
            for x,alive in enumerate(row):
                x0,y0 = x*cell_size, y*cell_size
                self.canvas.create_rectangle(x0,y0,x0+cell_size,y0+cell_size,fill='black' if alive else 'white', outline='lightgrey')
        self.canvas.update()


    def update_grid(self):
        # Update grid using rules
        new_grid = np.empty((GameOfLife.grid_size,GameOfLife.grid_size),dtype=int)
        
        for y,row in enumerate(self.grid):
            for x,cell in enumerate(row):
                count = self.count_neighbours((x,y))
                new_grid[x][y] = cell
                if cell and (count < 2 or count > 3):
                    new_grid[x][y] = not cell
                elif not cell and count == 3:
                    new_grid[x][y] = not cell

        self.grid = new_grid


    def count_neighbours(self,x) -> int:
        # Returns the number of living neighbours around x
        count = 0 
        for n in range(3):
            for m in range(3):
                dx,dy = m-1, n-1

                # If out of bounds, skip
                if (x[0]+dx) < 0 or (x[1]+dy) < 0 or (x[0]+dx) >= GameOfLife.grid_size or (x[1]+dy) >= GameOfLife.grid_size:
                    continue

                # Skip the cell in question
                if dx == dy == 0:
                    continue

                # Add to count if alive
                count += self.grid[x[0]+dx][x[1]+dy]
        return count

    def start(self):

        for n in range(GameOfLife.grid_size * 4):
            self.update_grid()
            self.draw(GameOfLife.cell_size)
            #time.sleep(0.01)

        self.window.mainloop()
        pass

    
if __name__=="__main__":
    game = GameOfLife()
    game.start()
