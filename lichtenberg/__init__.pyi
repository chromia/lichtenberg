from typing import List, Tuple, Callable
from enum import Enum

def set_random_seed(seed: int) -> None: ...
def make_electric_field(width: int, height: int, points: List[Tuple[int, int, float]]) -> List[List[float]]: ...


class BreakModel:
    def test(self, x: int, y:int) -> bool: ...
    def update(self, cells: CellList2D, broken_list: List[Tuple[int, int]]) -> None: ...


class DefaultBreakModel(BreakModel):
    pass


class ValueNoiseBreakModel(BreakModel):
    def __init__(self, width: int, height: int, min_guarantee: float = 0.05, seed: int = 0,
                 scale: float = 10.0, persistence: float = 0.5, octaves: int = 5) -> None: ...
    def get(self, x: int, y: int) -> float: ...


class ManualBreakModel(BreakModel):
    def __init__(self, width: int, height: int, source: List[float], min_guarantee: float = 0.05) -> None: ...


class DBMCell:
    def __init__(self, potential: float = 0.0, lock: bool = False) -> None: ...
    @property
    def potential(self) -> float: ...
    @potential.setter
    def potential(self, v: float) -> None: ...
    @property
    def lock(self) -> bool: ...
    @lock.setter
    def lock(self, v: bool) -> None: ...


DBMGrid = List[List[DBMCell]]


class DielectricBreakModel(BreakModel):
    def __init__(self, width: int, height: int, initial_state: DBMGrid, min_guarantee: float = 0.0, eta: float = 1.0) -> None: ...
    def get_potential(self, x: int, y: int) -> float: ...


class FastDBM(BreakModel):
    def __init__(self, width: int, height: int, min_guarantee: float = 0.0, eta: float = 1.0) -> None: ...


class DLABreakModel(BreakModel):
    def __init__(self, width: int, height: int, num_particle: int) -> None: ...


class Direction(Enum):
    Nim = 0
    Up = 1
    Left = 2
    Down = 3
    Right = 4


class CellList2D:
    def __init__(self, width: int = None, height: int = None) -> None: ...

    @property
    def width(self) -> int: ...

    @property
    def height(self) -> int: ...
    def is_valid(self) -> bool: ...
    def load(self, filepath: str) -> bool: ...
    def save(self, filepath: str) -> bool: ...
    def get_broken(self, x: int, y: int) -> bool: ...
    def set_broken(self, x: int, y: int) -> None: ...
    def get_count(self, x: int, y: int) -> int: ...
    def set_count(self, x: int, y: int, n: int) -> None: ...
    def add_count(self, x: int, y: int) -> None: ...
    def get_dir(self, x: int, y: int) -> Direction: ...
    def set_dir(self, x: int, y: int, direction: Direction) -> None: ...
    def get_insulated(self, x: int, y: int) -> bool: ...
    def set_insulated(self, x: int, y: int) -> None: ...
    def get_max_count(self) -> int: ...


class Simulator:
    def __init__(self, width: int, height: int, model: BreakModel = None,
                 up: bool = True, down: bool = True, left: bool = True, right: bool = True) -> None: ...
    def breakdown(self, x: int, y: int) -> None: ...
    def insulate(self, x: int, y: int) -> None: ...
    def insulate_square(self, x1: int, y1: int, x2: int, y2: int, fill: bool = False) -> None: ...
    def insulate_circle(self, x: int, y: int, radius: int, fill: bool = False) -> None: ...
    def simulate(self, max_loop: int = 50000, callback_on_break: Callable[[int, int], bool] = None,
                 callback_on_loop: Callable[[int, int, CellList2D], bool] = None) -> None: ...
    @property
    def cells(self) -> CellList2D: ...


class TreeNode:
    @property
    def parent(self) -> TreeNode: ...
    @parent.setter
    def parent(self, node: TreeNode) -> None: ...
    @property
    def prev(self) -> TreeNode: ...
    @prev.setter
    def prev(self, node: TreeNode) -> None: ...
    @property
    def next(self) -> TreeNode: ...
    @next.setter
    def next(self, node: TreeNode) -> None: ...
    @property
    def child(self) -> TreeNode: ...
    @child.setter
    def child(self, node: TreeNode) -> None: ...
    @property
    def point(self) -> Tuple[int, int, int]: ...
    @point.setter
    def point(self, v: Tuple[int, int, int]) -> None: ...
    def set_point(self, point: Tuple[int, int, int]) -> None: ...


class Leaf:
    def __init__(self, node: TreeNode, count: int) -> None: ...
    @property
    def node(self) -> TreeNode: ...
    @property
    def count(self) -> int: ...


class Tree:
    def __init__(self, cells: CellList2D) -> None: ...
    def get_node(self, x: int, y: int) -> TreeNode: ...
    def get_path(self, x1: int, y1: int, x2: int, y2: int) -> List[Tuple[int, int]]: ...
    def get_leaves(self) -> List[Leaf]: ...
