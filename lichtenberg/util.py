from typing import Tuple, List
from PIL import Image, ImageDraw, ImageMath, ImageFilter
from random import randint
import lichtenberg as lb


def get_path(start: Tuple[int, int], end: Tuple[int, int], margin: int = 50,
             randomness: float = 50.0, seed: int = None) \
        -> List[Tuple[int, int]]:
    """
    Generate single zig-zag line
    :param start: Starting Point
    :param end: Ending Point
    :param margin: The margin of the working area[pixel]
    :param randomness: Roughness of the line
    :param seed: The seed value of the internal random number generator
    :return: The list of points
    """
    if seed is not None and not isinstance(seed, int):
        raise ValueError("seed must be int or None")

    x1, y1 = start
    x2, y2 = end
    width = abs(x2 - x1 + 1)
    height = abs(y2 - y1 + 1)
    work_width = width + margin * 2
    work_height = height + margin * 2
    x_start, x_end = 0, width - 1
    y_start, y_end = 0, height - 1
    if x1 > x2:
        x_start, x_end = x_end, x_start
    if y1 > y2:
        y_start, y_end = y_end, y_start

    # Simulation
    if seed is None:
        seed = randint(0, 2**24)
    model = lb.ValueNoiseBreakModel(work_width, work_height, seed=seed, scale=randomness)
    sim = lb.Simulator(work_width, work_height, model)
    sim.breakdown(x_start+margin, y_start+margin)
    sim.simulate(max_loop=10**5)
    # Get Path
    tree = lb.Tree(sim.cells)
    points = tree.get_path(x_start+margin, y_start+margin, x_end+margin, y_end+margin)
    x_offset = x1 - x_start - margin
    y_offset = y1 - y_start - margin
    return [(x+x_offset, y+y_offset) for x, y in points]


def _linspace(start, stop, num):
    step = (stop - start) / num
    return [start + i * step for i in range(num)]


def draw_potential(image: Image, control_points: List[Tuple[Tuple[int, int], float]],
                   margin: int = 50, gamma: float = 1.0, multiply: float = 1.0,
                   color: Tuple[float, float, float] = None, seed: int = None) -> None:
    """
    Draw a lightning with potential simulation method(for an intense lightning)
    :param image: Target PIL image(with RGB mode)
    :param control_points: The list of control point and its intensity(brightness)
                           [((x, y), intensity), ...]
    :param margin: The margin of the working area[pixel]
    :param gamma: The gamma value to adjust the degree of light diffusion
    :param multiply: The coefficient to adjust brightness of light
    :param color: Color using exponential notation. (1.0, 1.0, 1.0) is white, (1.x, 1.0, 1.0) is red.
    :param seed: The seed value of the internal random number generator
    """
    width, height = image.size
    if color is None:
        color = (1.0, 1.0, 1.0)

    # Get Paths
    path_points = []
    path_sizes = []
    last = control_points[0]
    for cur in control_points[1:]:
        line_points = get_path(last[0], cur[0], seed=seed)[:-1]  # remove end point(to connect next line)
        line_sizes = _linspace(last[1], cur[1], len(line_points))
        path_points.extend(line_points)
        path_sizes.extend(line_sizes)
        last = cur
    x_min, x_max = y_min, y_max = 10 ** 8, -10 ** 8
    for x, y in path_points:
        x_min, x_max = min(x, x_min), max(x, x_max)
        y_min, y_max = min(y, y_min), max(y, y_max)

    # Electric Potential Simulation
    path_width = x_max - x_min + 1
    path_height = y_max - y_min + 1
    field_width = path_width + margin * 2
    field_height = path_height + margin * 2
    ox, oy = x_min - margin, y_min - margin
    potentials = [(x - ox, y - oy, siz) for (x, y), siz in zip(path_points, path_sizes)]
    field = lb.make_electric_field(field_width, field_height, potentials)

    # Imaging(Saturated Addition)
    cR, cG, cB = 1.0 / color[0], 1.0 / color[1], 1.0 / color[2]
    for y in range(field_height):
        for x in range(field_width):
            dx = x + x_min - margin
            dy = y + y_min - margin
            c = field[y][x]
            f = c ** gamma * multiply
            lumR = int(f ** cR * 255)
            lumG = int(f ** cG * 255)
            lumB = int(f ** cB * 255)
            if 0 <= dx < width and 0 <= dy < height:
                r, g, b = image.getpixel((dx, dy))
                r = min(255, r + lumR)
                g = min(255, g + lumG)
                b = min(255, b + lumB)
                image.putpixel((dx, dy), (r, g, b))


def draw_blur(image: Image, control_points: List[Tuple[int, int]],
              params: List[Tuple[int, float]], multiply: float,
              color: Tuple[float, float, float] = None, seed: int = None) -> None:
    """
    Draw a lightning with blur effect
    :param image: Target PIL image(with RGB mode)
    :param control_points: he list of control point. [(x, y), ...]
    :param params: The list of <line weight> and <blur radius>. [(weight, radius), ...]
    :param multiply: he coefficient to adjust brightness of light
    :param color: Color using exponential notation. (1.0, 1.0, 1.0) is white, (1.x, 1.0, 1.0) is red.
    :param seed: The seed value of the internal random number generator
    """
    width, height = image.size
    if color is None:
        color = (1.0, 1.0, 1.0)

    # Get Paths
    path_points = []
    last = control_points[0]
    for cur in control_points[1:]:
        line_points = get_path(last, cur, seed=seed)[:-1]  # remove end point(to connect next line)
        path_points.extend(line_points)
        last = cur
    x_min, x_max = y_min, y_max = 10 ** 8, -10 ** 8
    for x, y in path_points:
        x_min, x_max = min(x, x_min), max(x, x_max)
        y_min, y_max = min(y, y_min), max(y, y_max)

    # Blur Effect
    margin = 20
    path_width = x_max - x_min + 1
    path_height = y_max - y_min + 1
    work_width = path_width + margin * 2
    work_height = path_height + margin * 2
    img_work = Image.new("I", (work_width, work_height))
    for weight, radius in params:
        img_sub = Image.new("L", (work_width, work_height))
        draw = ImageDraw.Draw(img_sub)
        for x, y in path_points:
            w = weight
            ix = x - x_min + margin
            iy = y - y_min + margin
            draw.ellipse((ix - w, iy - w, ix + w, iy + w), fill=(255,))
        img_blur = img_sub.filter(ImageFilter.GaussianBlur(radius))
        img_work = ImageMath.eval("work+blur", work=img_work, blur=img_blur)

    # Imaging
    cR, cG, cB = 1.0 / color[0], 1.0 / color[1], 1.0 / color[2]
    for y in range(work_height):
        for x in range(work_width):
            dx = x + x_min - margin
            dy = y + y_min - margin
            if 0 <= dx < width and 0 <= dy < height:
                f = img_work.getpixel((x, y)) * multiply / 255
                lumR = int(f ** cR * 255)
                lumG = int(f ** cG * 255)
                lumB = int(f ** cB * 255)
                r, g, b = image.getpixel((dx, dy))
                r = min(255, r + lumR)
                g = min(255, g + lumG)
                b = min(255, b + lumB)
                image.putpixel((dx, dy), (r, g, b))


if __name__ == '__main__':
    pass
