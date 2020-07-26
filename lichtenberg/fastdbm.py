from enum import Enum


class BiasDirection(Enum):
    Left = 1,
    Right = 2,
    Up = 3,
    Down = 4


def make_bias(direction: BiasDirection, width: int, height: int):
    bias = []
    for y in range(height):
        for x in range(width):
            if direction == BiasDirection.Left:
                b = 1.0 + 1.0 - x/width
            elif direction == BiasDirection.Right:
                b = 1.0 + x/width
            elif direction == BiasDirection.Up:
                b = 1.0 + 1.0 - y/height
            elif direction == BiasDirection.Down:
                b = 1.0 + y/height
            else:
                b = 1.0
            bias.append(b)
    return bias
