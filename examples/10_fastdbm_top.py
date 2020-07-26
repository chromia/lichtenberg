import lichtenberg as lb
from lichtenberg.archive import save
from lichtenberg.fastdbm import BiasDirection, make_bias
from pathlib import Path
from random import randint


def main():
    width = 300
    height = 300

    lb.set_random_seed(randint(0, 32768))
    
    def end_condition(ix, iy):
        return iy == height - 1

    def loop(current_loop, max_loop, cells):
        if current_loop % 100 == 0:
            print(f'loop:{current_loop}/{max_loop}')
        return False

    # Initialize a break model
    min_guarantee = 0.0
    eta = 6.0
    bias = make_bias(BiasDirection.Down, width, height)
    model = lb.FastDBM(width, height, min_guarantee=min_guarantee, eta=eta, bias=bias)

    # Simulate
    sim = lb.Simulator(width, height, model)
    sim.breakdown(width//2, 0)
    sim.simulate(max_loop=20000, callback_on_break=end_condition, callback_on_loop=loop)

    # Output
    save(sim.cells, Path(__file__).stem,
         output_binary=True, output_mono=True, output_gray=True)


if __name__ == '__main__':
    main()
