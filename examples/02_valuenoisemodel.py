import lichtenberg as lb
from lichtenberg.archive import save
from PIL import Image
from pathlib import Path
from random import randint


def main():
    # ValueNoiseBreakModel has randomness more than default model, but slower...
    save_probability_mask = True
    width = 300
    height = 300
    seed = randint(0, 65535)  # ValueNoiseBreakModel uses own randomize code
    scale = 50.0  # zoom factor
    persistence = 1.0  # contrast
    octaves = 6  # details
    model = lb.ValueNoiseBreakModel(width, height, seed=seed, scale=scale,
                                    persistence=persistence, octaves=octaves)

    # Save probability
    if save_probability_mask:
        prob = Image.new("L", (width, height))
        for y in range(height):
            for x in range(width):
                p = model.get(x, y)
                lum = int(p * 255)
                prob.putpixel((x, y), (lum,))
        prob.save(Path(__file__).stem + '_prob.png')

    sim = lb.Simulator(width, height, model)
    sim.breakdown(150, 150)
    sim.simulate(max_loop=3000)

    # Save to PNG
    save(sim.cells, Path(__file__).stem, output_gray=True)


if __name__ == '__main__':
    main()
