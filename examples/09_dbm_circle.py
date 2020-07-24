import lichtenberg as lb
from lichtenberg.archive import save
from pathlib import Path
import math


def main():
    width = 100
    height = 100
    radius = width // 2

    def end_condition(ix, iy):
        dx = ix - width // 2
        dy = iy - height // 2
        d = (dx*dx+dy*dy) ** 0.5
        end = (d >= radius - 3)
        if end:
            print("Reached the outer circle. Break.")
        return end

    def loop(current_loop, max_loop, cells):
        print(f'loop:{current_loop}/{max_loop}')
        return False

    # Initialize the field of simulation (width+2) x (height+2)
    grid = []
    for y in range(height+2):
        row = []
        for x in range(width+2):
            c = lb.DBMCell()
            c.potential = 0.0
            c.lock = False
            row.append(c)
        grid.append(row)
    n = width * height // 2
    cx, cy = width // 2, height // 2
    R = radius - 2
    for i in range(n):
        # Put potential-1.0 on the circle (x-xc)^2+(y-yc)^2=R^2
        t = 2 * math.pi * (i / n)
        x = int(R * math.cos(t)) + cx
        y = int(R * math.sin(t)) + cy
        grid[y][x].lock = True
        grid[y][x].potential = 1.0

    # Initialize a break model
    eta = 2.0  # Detail level of branching
    min_guarantee = 0.001
    model = lb.DielectricBreakModel(width, height, grid,
                                    min_guarantee=min_guarantee, eta=eta)

    # Simulate
    sim = lb.Simulator(width, height, model)
    sim.breakdown(width // 2, height // 2)
    sim.simulate(max_loop=10000, callback_on_break=end_condition, callback_on_loop=loop)

    # Output
    gamma = 1.0 / 1.8  # to remove short branches
    scale = 1.0
    save(sim.cells, Path(__file__).stem,
         output_binary=True, output_mono=True, output_gray=True,
         gamma=gamma, scale=scale)


if __name__ == '__main__':
    main()
