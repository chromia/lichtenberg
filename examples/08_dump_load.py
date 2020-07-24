import lichtenberg as lb
from lichtenberg.archive import save
from pathlib import Path


def main():
    sim = lb.Simulator(100, 100)
    sim.breakdown(0, 0)
    sim.simulate()

    prefix = Path(__file__).stem
    binary_name = prefix + ".bin"

    # Dump
    sim.cells.save(binary_name)

    # Load
    cells2 = lb.CellList2D()
    cells2.load(binary_name)

    # Imaging & Output
    save(cells2, prefix, output_gray=True)  # see lichtenberg/archive.py


if __name__ == '__main__':
    main()