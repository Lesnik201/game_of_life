# Conway's Game of Life (C++ console implementation)

A console implementation of Conway's "Game of Life" cellular automaton. The starting generation is loaded from a text file, and the field evolves according to the classic rules, redrawing in place in the terminal.

## Rules

- A dead cell with exactly **3** live neighbours becomes alive.
- A live cell with **2 or 3** live neighbours stays alive.
- A live cell with fewer than 2 or more than 3 live neighbours dies (underpopulation / overpopulation).

## Field format

The starting generation is read from a file named `first_generation.txt`, which must be located in the program's working directory.

- `*` — border of the field (must form a complete, unbroken rectangle around the edge).
- `#` — a live cell.
- `' '` (space) — a dead cell.

Requirements enforced by the loader:

- All lines must be the same length (≤ 255 characters).
- The first and last lines must consist entirely of `*`.
- Every other line must start and end with `*`, and contain only `#` or `' '` in between.

Example (`first_generation.txt`):

```
***************************************
*                                     *
*                                     *
*                                     *
*                                     *
*             #                       *
*           # #                       *
*            ##                       *
*                                     *
*                                     *
*                                     *
*                                     *
*                                     *
*                                     *
***************************************
```

## Building

Requires a C++17-capable compiler (uses structured bindings).

```bash
g++ -std=c++17 -O2 -o live live.cpp
```

## Running

Make sure `first_generation.txt` is present in the current directory, then run:

```bash
./live
```

The field is redrawn in place every 300 ms. The program runs indefinitely — stop it with `Ctrl+C`.

## Known limitations

- Field dimensions (rows/columns) are limited to 255 each (`uint8_t`), and the file must match this.
- The border is fixed and does not evolve — cells never spawn or persist on the outer `*` frame.
- The input filename (`first_generation.txt`) is hardcoded; there is no command-line argument for a custom path.
