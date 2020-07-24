from typing import Tuple, List
from PIL import Image, ImageDraw
from pathlib import Path
import lichtenberg as lb
from scipy.interpolate import Akima1DInterpolator
import numpy as np


FlarePoint = Tuple[int, int, float]  # x, y, potential


def func_potential():
    """
    Return the potential curve
    :return: Function of intensity: f(t) (0<=t<=1)
    """
    # End of Tree  <--------------------------> Root of Tree
    ys = np.array([1.50, 1.20, 1.00, 0.80, 0.50])  # Potential Factor
    xs = np.linspace(0.0, 1.0, num=len(ys), endpoint=True)
    return Akima1DInterpolator(xs, ys)


def main():
    width = 300
    height = 300
    seed = 49057  # randint(0, 65535)
    print(f"seed={seed}")
    lb.set_random_seed(seed)

    # Simulate
    sim = lb.Simulator(width, height)
    sim.insulate_circle(75, 225, 150)
    sim.breakdown(10, 10)
    sim.simulate(max_loop=2000)

    # Get Longest Path
    tree = lb.Tree(sim.cells)
    leaves = tree.get_leaves()
    leaves = sorted(leaves, key=lambda lf: lf.count, reverse=True)
    longest_leaf = leaves[0]
    longest_points = []
    node = longest_leaf.node
    while node is not None:
        x, y, _ = node.point
        longest_points.append((x, y))
        node = node.parent

    # Generate Potential-Points on the path
    n = len(longest_points)
    step = 1
    potentials = []
    potential_curve = func_potential()
    for i in range(0, n, step):
        t = i / n
        x, y = longest_points[i]
        p = potential_curve(t)
        potentials.append((x, y, p))

    # Output
    margin = 50
    field_width = width + margin * 2
    field_height = height + margin * 2
    time_factors = [1.0, 0.5, 0.1]
    for i, t in enumerate(time_factors):
        print(f"{i + 1}/{len(time_factors)}")

        # Simulate potential field
        flares: List[FlarePoint] = []
        for x, y, p in potentials:
            flares.append((margin + x, margin + y, p * t))
        field = lb.make_electric_field(field_width, field_height, flares)

        # Make Images
        gamma = 1.0
        img = Image.new("L", (width, height))
        for y in range(height):
            for x in range(width):
                p = field[margin + y][margin + x]
                p = p ** gamma
                c = int(p * 255)
                img.putpixel((x, y), (c,))
        img.save(Path(__file__).stem + f"_{i:02}.png")

    img = Image.new("L", (width, height))
    draw = ImageDraw.Draw(img)
    n = len(potentials)
    for i in range(n-1):
        x1, y1, _ = potentials[i]
        x2, y2, _ = potentials[i+1]
        draw.line((x1, y1, x2, y2), fill=(255,), width=10)
    img.save(Path(__file__).stem + "_line.png")


if __name__ == '__main__':
    main()
