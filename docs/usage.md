
## import

```python
import lichtenberg as lb
```

## Basic Simulation

3-steps for easiest simulation

```python
width, height = 50, 50
sim = lb.Simulator(width, height)  (1)
sim.breakdown(width//2, height//2)  (2)
sim.simulate()  (3)
```

    - (1). Make `Simulator` instance with the size you preferred.
    - (2). Set the start point of Dielectric Breakdown.
    - (3). Start simulation

So you can get the result of simulator with `sim.cells` which is the instance of `CellList2D`.

```python
sim.cells.get_broken(x, y)   # Whether the cell (x,y) is broken or not
sim.cells.get_count(x, y)  # The length of longest branch grown from the cell (x, y)
sim.cells.get_max_count()  # Max count in all cells
```

### Visualization of broken cells(with Pillow)

```python
from PIL import Image
image = Image.new("L", (width, height))
for y in range(height):
    for x in range(width):
        c = 255 if sim.cells.get_broken(x, y) else 0
        image.putpixel((x, y), (c,))
image.save("broken.png")
```

![broken](https://user-images.githubusercontent.com/6506891/88887272-0977aa80-d277-11ea-8e5a-972ba896375c.png)

Unfortunately the image is completely white! Because all cell is broken by default settings(default model).

### Visualization of cell count(with Pillow)

```python
image = Image.new("L", (width, height))
    max_count = sim.cells.get_max_count()
    for y in range(height):
        for x in range(width):
            count = sim.cells.get_count(x, y)
            c = int(count / max_count * 255)  # Brighter proportion to the count
            image.putpixel((x, y), (c,))
    image.save("count.png")
```

![count](https://user-images.githubusercontent.com/6506891/88887353-35932b80-d277-11ea-8cf5-68f398d2d8aa.png)

The root of the tree(= start point) has max_count(brightest). The leaf of the tree has count 1(dark).


## Breakdown Models

The constructor of `Simulator` class has the 3rd argument `model`. And you can specify a breakdown model.


There are several implemented breakdown models.

|model name|class name|references|
|---|---|---|
|Uniformed Probability Model(*1)|DefaultBreakModel|http://lazylifeeasyexit.blog67.fc2.com/blog-entry-53.html|
|ValueNoise Probability Model(*1)|ValueNoiseBreakModel|original|
|Probablity by Image(*1)|ManualBreakModel|original|
|Dielectric Breakdown Model|DielectricBreakModel|https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.52.1033|
|Fast DBM(*1)|FastDBM|http://gamma.cs.unc.edu/FRAC/|
|Diffusion-limited aggregation|DLABreakModel|https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.47.1400|

### Uniformed Probability Model(DefaultBreakModel)

On DefaultBreakModel, the breakdown probability of each cell is 0.5(50%).
if you don't pass `model` argument to `Simulator`, this model is used.

```python
model = lb.DefaultBreakModel()
sim1 = lb.Simulator(width, height, model)  (1)
sim2 = lb.Simulator(width, height)  (2)
```

(2) is the same as (1).

see also [/examples/01_basic.py](https://github.com/chromia/lichtenberg/blob/master/examples/01_basic.py)

### Value Noise Probability Model(ValueNoiseBreakModel)

The probability of each cell is decided with [Value Noise method](https://web.archive.org/web/20160530124230/http://freespace.virgin.net/hugo.elias/models/m_perlin.htm).
This model requires the size of simulation field and parameters of Value Noise.

|Argument|Type|Default|Description|
|---|---|---|---|
|width|int|-||
|height|int|-||
|min_guarantee|float|0.05|Minimum probability of each cell. If probability = 0.0 is set by random number, the simulation will never end. To prevent this, probabilities of all cells are narrowed in the range \[min_guarantee, 1.0\].|
|seed|int|0|The seed of internal random generator.|
|scale|float|10.0|Zoom factor of value noise field.|
|persistence|float|0.5|Contrast of value noise field.|
|octaves|float|5|Detail level of value noise field.|


```python
model = lb.ValueNoiseBreakModel(width, height)
sim = Simulator(width, height, model)
```

You can extract the probability by `get(x,y)`.

see also [/examples/02_valuenoisemodel.py](https://github.com/chromia/lichtenberg/blob/master/examples/02_valuenoisemodel.py)

```python
prob = model.get(0, 0)
```

### Probability by Image(ManualBreakModel)

ManualBreakModel receives 2D-probability map directly and use it.

```python
prob_map = [
    [0.50, 0.01, 0.01],
    [0.50, 0.50, 0.01],
    [0.01, 0.50, 0.50]
]
model = lb.ManualBreakModel(3, 3, prob_map)
```

|Argument|Type|Default|Description|
|---|---|---|---|
|width|int|-||
|height|int|-||
|source|List[List[float]]|-|2D-array of cell which stores probability.|
|min_guarantee|float|0.05|Minimum probability of each cell. If probability = 0.0 is set by random number, the simulation will never end. To prevent this, probabilities of all cells are narrowed in the range \[min_guarantee, 1.0\].|

see also [/examples/03_imagemodel.py](https://github.com/chromia/lichtenberg/blob/master/examples/03_imagemodel.py)

### Dielectric Breakdown Model(DielectricBreakdownModel)

This is an implementation [DBM](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.52.1033).

In this model, initial condition of electric charge is required. It is 2D-array of `DBMCell` with the size (width+2)*(height+2). Plus 2 means the outside border of simulation area.
For example, If the size of simulation area is 3x3, then the size of initial condition is 5x5.

```python
potentials = [
    [0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0],
    [1, 1, 1, 1, 1]
]
```

The value of potential(voltage of electric charge) represents the power of attraction. In this case, the direction of breakdown is downward.

```python
locked = [
    [1, 1, 1, 1, 1],
    [1, 0, 0, 0, 1],
    [1, 0, 0, 0, 1],
    [1, 0, 0, 0, 1],
    [1, 1, 1, 1, 1],
]
```
`locked` flag indicates potential value of the cell will never change during simulation. 
As very important constraints, the cells on outside border must be locked.

```python
initial = [[lb.DBMCell(p, bool(l)) for p, l in zip(row_p, row_l)]
           for row_p, row_l in zip(potentials, locked)]
model = lb.DielectricBreakModel(3, 3, initial)
```

So `DBMCell` requires two arguments, `potential` and `locked`.

|Argument|Type|Default|Description|
|---|---|---|---|
|width|int|-||
|height|int|-||
|initial_state_|List[List[DBMCell]]|-|2D-array of `DBMCell` which stores potential and locked state.|
|min_guarantee|float|0.05|Minimum probability of each cell. If probability = 0.0 is set by random number, the simulation will never end. To prevent this, probabilities of all cells are narrowed in the range \[min_guarantee, 1.0\].|
|eta|float|1.0|Parameter for branching. The higher the value, the less likely it is to branch out.

see also [/examples/09_dbm_top.py](https://github.com/chromia/lichtenberg/blob/master/examples/09_dbm_top.py) and [/examples/09_dbm_circle.py](https://github.com/chromia/lichtenberg/blob/master/examples/09_dbm_circle.py)

### Fast DBM(FastDBM)

now writing...
