import lichtenberg
from lichtenberg.archive import gray
from PIL import Image, ImageMath
from pathlib import Path


"""
Usage of ManualBreakModel

ManualBreakModel requires an array of 1-dimensional probability values.
Each probability value must be from 0.0 to 1.0.

But "probability = 0.0" means the cell is not break infinity,
so "min_guarantee" argument overwrite the minimum value. 

In the case of "min_guarantee == 0.05":
  raw range:   0.00 <-------------> 1.00
  fixed range: 0.05  <------------> 1.00
"""


def image_to_array(image: Image):
    """
    convert 2D-image to 2d-array(float)
    """
    pixels = []
    for y in range(image.height):
        row = []
        for x in range(image.width):
            f = image.getpixel((x, y)) / 255  # normalize 0.0<=f<=1.0
            row.append(f)
        pixels.append(row)
    return pixels


def simulation_1():
    mask_path = './resources/route.png'
    image = Image.open(mask_path)
    width, height = image.size
    pixels = image_to_array(image)
    model = lichtenberg.ManualBreakModel(width, height, pixels, min_guarantee=0.005)
    sim = lichtenberg.Simulator(width, height, model)
    sim.breakdown(100, 30)
    sim.simulate(max_loop=2000)
    figure = gray(sim.cells)
    figure.save(Path(__file__).stem + "_1_gray.png")
    img_sum = ImageMath.eval("convert((a+b), 'L')", a=image, b=figure)
    img_sum.save(Path(__file__).stem + "_1_gray_overlay.png")


def simulation_2():
    mask_path = './resources/mask.png'
    image = Image.open(mask_path)
    width, height = image.size
    pixels = image_to_array(image)
    model = lichtenberg.ManualBreakModel(width, height, pixels, min_guarantee=0.01)
    sim = lichtenberg.Simulator(width, height, model)
    sim.breakdown(0, 0)
    sim.simulate(max_loop=1000)
    figure = gray(sim.cells)
    figure.save(Path(__file__).stem + "_2_gray.png")
    img_sum = ImageMath.eval("convert((a+b), 'L')", a=image, b=figure)
    img_sum.save(Path(__file__).stem + "_2_gray_overlay.png")


if __name__ == '__main__':
    simulation_1()
    simulation_2()
