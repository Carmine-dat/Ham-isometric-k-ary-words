Computing the index of non-isometric k-ary words with the Hamming distance
=========

Introduction
-----------

Isometric k-ary words are those words whose occurrence as a factor in a transformation of a word u in a word v can be avoided, while preserving the minimal length of the transformation.
Such minimal length refers to a distance between u and v.
The aim of this project is to implement in a programming language an algorithm to decide whether a word f is Ham-isometric and provide evidence of the eventual non-isometricity by returning a pair of words of minimal length whose transformation cannot avoid the factor f.
Such pair of words is called a pair of witnesses and the minimal length of the witnesses is called the index of f.
The algorithms run in O(n) time with a preprocessing of O(n) time and space to construct a data structure that
allows answering LCA queries on the suffix tree of f in constant time.

The algorithm was implemented based on results of theoretical studies by Marcella Anselmo, Manuela Flores and Maria Madonia published in the paper "Computing the index of non-isometric k-ary words with the Hamming and Lee Distance".

Previous programs
------------

At the time of the development of this project, another project already exists that implements the same algorithm with reference to the Lee distance and with quaternary alphabets (https://github.com/Galiem/IsometricString).
This project refers to the Hamming distance, with alphabets of any size k with k >= 2.

Requirements
------------

This installation requires:

* A modern, C++11 ready compiler such as `g++` version 4.9 or higher or `clang` version 3.2 or higher.
* The [cmake][cmake] build system.
* A 64-bit operating system. Either Mac OS X or Linux are currently supported.
* The libraries: libdivsufsort and sdsl.

Installation
------------

To download and install the library use the following commands.

```sh
git clone https://github.com/Carmine-dat/Ham-isometric-k-ary-words.git
cd Ham-isometric-k-ary-words
bash install.sh
```

This installs the sdsl and libdivsufsort library into the `include` and `lib` directories in your
home directory. A different location prefix can be specified as a parameter of
the `install.sh` script:

```sh
bash install.sh /usr/local/
```

To remove the library from your system use the provided uninstall script:

```sh
bash uninstall.sh
```

Compilation and execution
------------

To compile the program using `g++` run:

```sh
g++ isometricWords.cpp -o isometricWords -lsdsl -ldivsufsort
```

To execute the program run:

```sh
./isometricWords
```

Authors of the SDSL Library
--------

The main contributors to the library are:

* [Johannes Bader](https://github.com/olydis)
* [Timo Beller](https://github.com/tb38)
* [Simon Gog](https://github.com/simongog) (Creator)
* [Matthias Petri](https://github.com/mpetri)

## External Resources used in SDSL

In sdsl library is included the code of an excellent suffix array construction algorithm for byte-alphabets created by Yuta Mori in the [libdivsufsort][DIVSUF] library.

## Other references

Furthermore, to build the LCP array I used a function I found to https://github.com/solonas13/maw/tree/master

[cmake]: http://www.cmake.org/ "CMake tool"
[DIVSUF]: https://github.com/y-256/libdivsufsort/ "libdivsufsort"