from lichtenberg.util import draw_blur_with_points
from lichtenberg.fastdbm import BiasDirection, make_bias
import lichtenberg as lb
from PIL import Image
from pathlib import Path


def main():
    width, height = 300, 300
    img = Image.new("RGB", (width, height))

    # Using FastDBM(see also 10_fastdbm_top.py)
    def end_condition(ix, iy):
        return iy == height - 1

    def loop(current_loop, max_loop, cells):
        if current_loop % 100 == 0:
            print(f'loop:{current_loop}/{max_loop}')
        return False

    min_guarantee = 0.0
    eta = 2.0
    bias = make_bias(BiasDirection.Down, width, height)
    model = lb.FastDBM(width, height, min_guarantee=min_guarantee, eta=eta, bias=bias)
    sim = lb.Simulator(width, height, model)
    sim.breakdown(width//2, 0)
    sim.simulate(max_loop=40000, callback_on_break=end_condition, callback_on_loop=loop)

    # Trimming short tree branches
    min_count = 50
    min_luminance, max_luminance = 0.0, 1.00
    points, luminance = [], []
    max_count = sim.cells.get_max_count()
    for y in range(height):
        for x in range(width):
            if sim.cells.get_broken(x, y):
                c = sim.cells.get_count(x, y)  # length from the end of branch
                if c >= min_count:
                    if max_count == min_count:
                        f = 1.0
                    else:
                        f = (c - min_count) / (max_count - min_count)
                    lum = min_luminance + f * (max_luminance - min_luminance)
                    luminance.append(lum)
                    points.append((x, y))

    # Imaging
    blur_params = [(0, 1.0), (1, 4.0), (2, 8.0)]
    color = (1.2, 1.0, 1.3)
    draw_blur_with_points(img, points, blur_params, 1.0, color, luminance)

    img.save(Path(__file__).stem + ".png")


if __name__ == '__main__':
    main()
