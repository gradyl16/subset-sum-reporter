# SSum

SSum is a command line program for computing and reporting on various statistics relating to a sequence of elements and a target sum. See `report.pdf` for a full explanation of the dynamic programming algorithm used.

## Compilation

Assuming you have the makefile included with this repository, you can compile with `make` commands:

```
make all
```

Otherwise:

```
g++ -std=c++11 ssum.cpp -o ssum
```

These commands are identical. You may choose a different c standard or executable name, but those may not be consistent with the usage listed here.

## Usage

Assuming the executable is located in your current working directory, the syntax is:

```
./ssum [target sum] < [optional input file]
```

If redirection is not utilized, values will be read from standard input according to the format specified by `read_elems` in `ssum.cpp`.

Example:

```
./ssum 5 < toy.txt
```