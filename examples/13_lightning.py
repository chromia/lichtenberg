from lichtenberg.util import draw_potential, draw_blur
from PIL import Image
from pathlib import Path


def main():
    # Load image
    # "cityby-night-05.jpg" is distributed under public domain.
    # https://www.publicdomainpictures.net/en/view-image.php?image=149940&picture=city-by-night-05
    image = Image.open("resources/cityby-night-05.jpg")

    # Color in exponential representation(not in range [0.0-1.0])
    color = (1.0, 1.0, 1.5)  # Blue

    # Lightning with potentials
    control_points = [
        ((1300, -80), 1.0),
        ((1384, 184), 1.0),
        ((1457, 250), 1.0),
        ((1422, 353), 1.05),
    ]
    draw_potential(image, control_points, gamma=20.0, multiply=0.8, color=color)

    # Lightning with blur
    blur_params = [  # (line weight, blur radius)
        (0, 1.0),
        (1, 4.0),
        (2, 8.0),
    ]
    p = [(657, 127), (616, 200), (709, 231), (793, 288), (779, 358)]
    draw_blur(image, p, blur_params, multiply=0.3, color=color)

    p = [(652, 138), (727, 168), (854, 162), (943, 155), (1011, 204), (1088, 227)]
    draw_blur(image, p, blur_params, multiply=0.1, color=color)
    draw_blur(image, p, blur_params, multiply=0.1, color=color)

    p = [(1035, 48), (1084, 185), (1060, 297), (1062, 424), (1073, 497)]
    draw_blur(image, p, blur_params, multiply=0.3, color=color)

    p = [(1025, 159), (1037, 232), (926, 277), (866, 260), (801, 226), (731, 241), (617, 256), (563, 274)]
    draw_blur(image, p, blur_params, multiply=0.25, color=color)

    # Output
    image.save(Path(__file__).stem + ".png")


if __name__ == '__main__':
    main()
