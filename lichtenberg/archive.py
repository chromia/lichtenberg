import lichtenberg as lb
from PIL import Image


def gray(cells: lb.CellList2D, **kwargs) -> Image:
    """
    Output grayscale image
    :param cells: Result of the simulation(Simulator.cells)
    :param kwargs:
        gamma: float: gamma value (default is 1.0)
        scale: float: scaling value(default is 1.0)
    """
    width, height = cells.width, cells.height
    image = Image.new('L', (width, height))
    gamma = kwargs.get("gamma", 1.0)  # to remove short branches
    scale = kwargs.get("scale", 1.0)  # darken branches to emphasis return stroke
    c_max = cells.get_max_count()
    for y in range(height):
        for x in range(width):
            if cells.get_broken(x, y):
                c = cells.get_count(x, y)
                k = (c / c_max) ** gamma * scale
                lm = min(int(255 * k), 255)
                image.putpixel((x, y), (lm,))
    return image


def mono(cells: lb.CellList2D) -> Image:
    """
    Output image which draw broken cells
    :param cells: Result of the simulation(Simulator.cells)
    """
    width, height = cells.width, cells.height
    image = Image.new('L', (width, height))
    for y in range(height):
        for x in range(width):
            if cells.get_broken(x, y):
                lm = 255
                image.putpixel((x, y), (lm,))
    return image


def save(cells: lb.CellList2D, prefix: str, **kwargs) -> None:
    """
    Common output function
    :param cells: Result of the simulation(Simulator.cells)
    :param prefix: Prefix of the output files (xxxxx.png)
    :param kwargs:
        output_binary: bool: generate *.bin file (used for CellList2D.load())
        output_mono: bool: generate image which draw broken cells
        output_gray: bool: generate grayscale image with gamma correction
        gamma: float: gamma value (default is 1.0)
        scale: float: scaling value(default is 1.0)
    """

    # output binary
    if kwargs.get("output_binary", False):
        cells.save(prefix + '.bin')

    # output mono
    if kwargs.get("output_mono", False):
        image = mono(cells)
        image.save(prefix + '_mono.png')

    # output grayscale
    if kwargs.get("output_gray", False):
        image = gray(cells, **kwargs)
        image.save(prefix + '_gray.png')
