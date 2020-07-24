import lichtenberg
from PIL import Image
from pathlib import Path
import random


def main():
    width = 200
    height = 200
    sim = lichtenberg.Simulator(width, height)
    sim.breakdown(0, 0)
    sim.insulate_square(0, 50, 40, 51)
    sim.insulate_square(50, 0, 51, 45)
    sim.simulate(max_loop=1000)

    # 1. Draw entire tree
    image = Image.new('RGB', (width, height), (0x00, 0x00, 0x00))
    c_max = sim.cells.get_max_count()
    for y in range(height):
        for x in range(width):
            c = sim.cells.get_count(x, y)
            lm = int(255 * c / c_max)  # normalize
            image.putpixel((x, y), (lm, lm, lm))
    image.save(Path(__file__).stem + '_1.png')

    # 2. Select several paths
    targets = [(5, 116), (3, 176), (46, 180), (94, 184), (184, 187), (175, 85), (156, 9)]
    image = Image.new('RGB', (width, height), (0x00, 0x00, 0x00))
    tree = lichtenberg.Tree(sim.cells)
    for target in targets:
        path = tree.get_path(0, 0, *target)  # get the path between (0, 0) and target
        for point in path:
            x, y = point
            image.putpixel((x, y), (255, 255, 255))
    image.save(Path(__file__).stem + '_2.png')


if __name__ == '__main__':
    main()
