# Lichtenberg

![test](https://github.com/chromia/lichtenberg/workflows/test/badge.svg)

Lichtenberg is a collection of example implementations of [Dielectric Breakdown](https://en.wikipedia.org/wiki/Electrical_breakdown) represents `Lichtenberg Figure` like thunder and lightning.


 - DBM(Dielectric Breakdown Model) - [Paper](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.52.1033)
 - Fast DBM - [Paper](http://gamma.cs.unc.edu/FRAC/)
 - DLA(Diffusion-limited aggregation) - [Paper](https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.47.1400)
 - Uniform Probability Model - [URL](http://lazylifeeasyexit.blog67.fc2.com/blog-entry-53.html)
 - ValueNoise Probability Model
 - Probability from Image
 
 The source code of the core algorithm are implemented in C++. Examples are written in Python(via Pybind11).
 
 ![thumb](https://user-images.githubusercontent.com/6506891/88478910-d4bdd780-cf86-11ea-873c-7355a6ab8b55.png)
 
## Prerequisites
 
  - Python 3.7 or later
  - C++ Compiler supports C++17
    - for Windows: Visual Studio 2019 is confirmed
    - for Linux(Mac): gcc 9.3 is confirmed
  - CMake 3.1 or later
  - [Pybind11](https://github.com/pybind/pybind11)
  - [Pillow](https://pillow.readthedocs.io/en/stable/)
  - [Scipy](https://www.scipy.org/) (for some examples)
  - [NumPy](https://numpy.org/) (for some examples)
 
## Install
 

(for Ubuntu)Install C++ Compiler and CMake
```bash
sudo apt install build-essential cmake
```

Clone this repository (don't forget *recursive* option)

```bash
git clone --recursive https://github.com/chromia/lichtenberg.git
cd lichtenberg
```

Install with setup.py

```bash
python setup.py install
```

## Running examples

Examples are in [examples/](https://github.com/chromia/lichtenberg/tree/master/examples) directory

```bash
cd examples
python 01_basic.py
...
```

The result will be output to *examples/* directory(e.g. examples/01_basic_gray.png)
 
## Documentation

[docs/](https://github.com/chromia/lichtenberg/tree/master/docs)
 
## License

- MIT License
- except examples/resources/cityby-night-05.jpg(public domain image) - [original](https://www.publicdomainpictures.net/en/view-image.php?image=149940&picture=city-by-night-05)