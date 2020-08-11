
## About Simulation of Dielectric Breakdown

In this library, the simulation of dielectric breakdown is performed in 2D-grid.

![breakout](https://user-images.githubusercontent.com/6506891/89886125-14332780-dc07-11ea-9d40-4bad3a64c2cc.png)

Firstly, one broken cell is put on a cell. Neighbor cells are marked as candidate cell, these are next target of breakdown. 
All candidate cells are tested whether the cell will break or not break with the probability of the cell `P(x, y)`.
A random number in the range [0.0, 1.0] is generated and compared with `P(x, y)`. If the number is less than `P(x, y)`,
the cell will be broken.

```python
# Pseudo code
def test(x, y):
    rand = random()  # [0.0, 1.0]
    if rand < P(x, y):
        return True  # Break
    else:
        return False  # Not Break
```

## Breakdown core algorithm

This is a pseudo code of breakdown chart(python-like).

```python
broken_cells = first_broken_cells  # (1)
for loop in range(max_loop):
    candidate_cells = collect_candidates(broken_cells)  # (2)
    if len(candidate_cells) == 0:
        break
    for cand_cell in candidate_cells:
        x, y = cand_cell.position
        if test(x, y):  # (3)
            broken_cells.append(cand_cell)  # (4)
```

(1): Firstly, one or some cells are marked as broken cells. Each cell has own position(x,y).

(2): Get all neighbor cells of broken cells, without broken cells itself.

(3): Generate a random number and test if the cell is broken or not.

(4): If a cell pass the test, add it into broken cells list.