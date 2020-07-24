import lichtenberg
from PIL import Image
from pathlib import Path


def main():
    width = 200
    height = 200
    sim = lichtenberg.Simulator(width, height)
    sim.breakdown(60, 60)
    sim.insulate_circle(100, 100, 50)
    sim.simulate(max_loop=1000)

    image = Image.new('RGB', (width, height), (0x00, 0x00, 0x00))
    c_max = sim.cells.get_max_count()
    for y in range(height):
        for x in range(width):
            if sim.cells.get_insulated(x, y):
                # visualize insulated cells
                image.putpixel((x, y), (255, 0, 0))
            else:
                c = sim.cells.get_count(x, y)
                lm = int(255 * c / c_max)  # normalize
                image.putpixel((x, y), (lm, lm, lm))
    image.save(Path(__file__).stem + '.png')


if __name__ == '__main__':
    main()
