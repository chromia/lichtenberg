import lichtenberg as lb
from lichtenberg.archive import save
from pathlib import Path
from random import randint


def main():
    width = 300
    height = 300

    lb.set_random_seed(randint(0, 32768))
    
    def end_condition(ix, iy):
        return ix == 0 or ix == width - 1 or iy == 0 or iy == height - 1

    def loop(current_loop, max_loop, cells):
        if current_loop % 100 == 0:
            print(f'loop:{current_loop}/{max_loop}')
        return False

    # Initialize a break model
    min_guarantee = 0.0
    eta = 6.0
    model = lb.FastDBM(width, height, min_guarantee=min_guarantee, eta=eta)

    # Simulate
    sim = lb.Simulator(width, height, model)
    sim.breakdown(width // 2, height // 2)
    sim.simulate(max_loop=10000, callback_on_break=end_condition, callback_on_loop=loop)

    # Output
    save(sim.cells, Path(__file__).stem,
         output_binary=True, output_mono=True, output_gray=True)


if __name__ == '__main__':
    main()
