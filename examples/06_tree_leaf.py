import lichtenberg
from PIL import Image
from pathlib import Path


def main():
    width = 200
    height = 200
    sim = lichtenberg.Simulator(width, height)
    sim.breakdown(width // 2, height // 2)
    sim.insulate_circle(100, 100, 100)
    sim.simulate(max_loop=1000)

    tree = lichtenberg.Tree(sim.cells)
    leaves = tree.get_leaves()

    # Select and Draw 2048 longest branch
    image = Image.new('RGB', (width, height), (0x00, 0x00, 0x00))
    # Leaf.count is the length from root(the point specified by Simulator.breakdown)
    longest_leaves = sorted(leaves, key=lambda lf: lf.count)[-2048:]
    for leaf in longest_leaves:
        node = leaf.node
        while node is not None:
            x, y, _ = node.point
            image.putpixel((x, y), (255, 255, 255))
            node = node.parent
    image.save(Path(__file__).stem + '_1.png')

    # Cut down 20-unit from outside
    image = Image.new('RGB', (width, height), (0x00, 0x00, 0x00))
    threshold = 20
    for leaf in longest_leaves:
        node = leaf.node
        while node is not None:
            x, y, count = node.point
            # count is the length from leaf
            # So omit the end of branch
            if count >= threshold:
                image.putpixel((x, y), (255, 255, 255))
            node = node.parent
    image.save(Path(__file__).stem + '_2.png')


if __name__ == '__main__':
    main()
