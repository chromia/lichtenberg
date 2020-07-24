from typing import List
import lichtenberg as lb
from lichtenberg.archive import save
from PIL import Image
from pathlib import Path


def visualize_field(width: int, height: int,
                    cells: lb.CellList2D,
                    model: lb.DielectricBreakModel) -> Image:
    scale = 2
    img = Image.new("RGB", (width + 2, height + 2))

    # Draw potentials
    min_p = 100.0
    max_p = -1.0
    for iy in range(height + 2):
        for ix in range(width + 2):
            p = model.get_potential(ix, iy)
            min_p = min(min_p, p)
            max_p = max(max_p, p)
    for iy in range(height + 2):
        for ix in range(width + 2):
            p = model.get_potential(ix, iy)
            if max_p == min_p:
                v = 128
            else:
                v = int((p - min_p) / (max_p - min_p) * 255)
            img.putpixel((ix, iy), (v, v, v))
    # Draw broken cells
    for cy in range(height):
        for cx in range(width):
            if cells.get_broken(cx, cy):
                img.putpixel((cx + 1, cy + 1), (255, 255, 0))

    return img.resize((img.width * scale, img.height * scale))


def main():
    width = 100
    height = 100
    lb.set_random_seed(156)
    save_GIF = True
    GIF_frames: List[Image] = []

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
    # Initial Conditions:
    # - Top row has a minus potentials(attracted)
    # - Bottom row has a plus potentials(attractor)
    top, bottom = 0, height+1
    for x in range(width+2):
        grid[top][x].lock = True
        grid[bottom][x].lock = True
        grid[bottom][x].potential = 1.0
    left, right = 0, width+1
    for y in range(height+2):
        grid[y][left].lock = True
        grid[y][right].lock = True

    def end_condition(ix, iy):
        """
        If you return True, the simulation will be interrupted.
        """
        end = (iy == height-1)
        if end:
            print("Reached the end line. Break.")
        return end

    # Initialize a break model
    eta = 1.0  # Detail level of branching
    min_guarantee = 0.005
    model = lb.DielectricBreakModel(width, height, grid,
                                    min_guarantee=min_guarantee, eta=eta)

    def loop(current_loop, max_loop, cells):
        """
        If you return True, the simulation will be interrupted.
        """
        # Progress Indicator
        print(f"loop:{current_loop}/{max_loop}")

        # True to generate animation frames
        if save_GIF:
            if current_loop % 10 == 0:
                frame = visualize_field(width, height, cells, model)
                GIF_frames.append(frame)
        return False

    # Simulate
    sim = lb.Simulator(width, height, model, up=False)
    sim.breakdown(width // 2, 0)
    sim.simulate(max_loop=10000, callback_on_break=end_condition, callback_on_loop=loop)

    # Output
    gamma = 1.0/1.8
    scale = 1.0
    save(sim.cells, Path(__file__).stem,
         output_binary=True, output_mono=True, output_gray=True,
         gamma=gamma, scale=scale)

    if save_GIF:
        GIF_frames[0].save(Path(__file__).stem + ".gif",
                           save_all=True,
                           append_images=GIF_frames[1:],
                           optimize=False, duration=100, loop=0)


if __name__ == '__main__':
    main()
