from lichtenberg.util import draw_blur
from PIL import Image
from random import randint
from pathlib import Path


def main():
    width, height = 600, 600
    img = Image.new("RGB", (width, height))

    blur_params = [(0, 1.0), (1, 4.0), (2, 8.0)]
    color = (1.2, 1.0, 1.0)

    num_line = 50
    for i in range(num_line):
        print(f"{i+1}/{num_line}")
        sx, sy = randint(100, 500), 50
        ex, ey = sx + randint(-50, 50), 550
        draw_blur(img, [(sx, sy), (ex, ey)], blur_params, 0.3, color)

    img.save(Path(__file__).stem + ".png")


if __name__ == '__main__':
    main()
