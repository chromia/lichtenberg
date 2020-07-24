import lichtenberg as lb
from lichtenberg.archive import save
from pathlib import Path
from random import randint


def main():
    # Basic Simulation
    width = 300
    height = 300
    lb.set_random_seed(randint(0, 65535))  # Set the random seed of C/C++ standard library
    sim = lb.Simulator(width, height)
    sim.breakdown(150, 150)  # First broken cell is (x,y)=(150, 150)
    sim.simulate(max_loop=2000)

    # Save to PNG
    save(sim.cells, Path(__file__).stem, output_gray=True)  # See lichtenberg/archive.py
    # -> ./01_basic_gray.png


if __name__ == '__main__':
    main()
