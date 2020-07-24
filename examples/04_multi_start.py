import lichtenberg as lb
from lichtenberg.archive import save
from pathlib import Path
from random import randint


def main():
    width = 200
    height = 200
    lb.set_random_seed(randint(0, 65535))
    sim = lb.Simulator(width, height)
    sim.breakdown(100, 50)
    sim.breakdown(50, 100)
    sim.breakdown(100, 150)
    sim.breakdown(150, 100)
    sim.simulate(max_loop=1000)
    save(sim.cells, Path(__file__).stem, output_gray=True)


if __name__ == '__main__':
    main()
