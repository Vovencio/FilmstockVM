# FilmstockVM

The virtual machine at the core of the **Filmstock** toolchain. It loads and executes compiled `.filmstock` binaries, with optional hardware-accelerated graphics via [raylib](https://www.raylib.com/).

**Toolchain repositories:**
- [FilmstockCASM](https://github.com/Vovencio/FilmstockCASM) - Compiler & Assembler: produces `.filmstock` files from Filmstock source code.
- [FilmstockBin](https://github.com/Vovencio/FilmstockBin) - The Filmstock toolchain.

---

## Overview

FilmstockVM is a register-less, flat-memory virtual machine. The entire address space is a single array of `double` values called the **film**. Every instruction operates directly on film indices - there are no general-purpose registers. A separate dynamic list system (`matrix`) provides resizable arrays of doubles for heap-like data structures.

The VM auto-detects whether a program requires graphics and opens a window accordingly; console-only programs run without any windowing overhead.

---

## Architecture

### Memory model

| Name | Type | Description |
|---|---|---|
| `film` | `double[]` | Flat main memory. All variables live here, addressed by integer index. |
| `matrix` | `vector<vector<double>>` | Dynamic list heap. Lists are created at runtime and referenced by integer ID stored in `film`. |

### Instruction format

Every instruction is 16 bytes wide: four 32-bit little-endian integers.

```
[ opcode (4B) | param1 (4B) | param2 (4B) | param3 (4B) ]
```

All three parameters are **film indices** unless otherwise noted per opcode. The fixed width allows the loader to slice the instruction stream trivially and enables the boundary-precomputation optimisation described below.

### Binary file format (`.filmstock`)

```
┌─────────────────────────────────────────────┐
│ film_length        (4 bytes, int32-LE)      │
├─────────────────────────────────────────────┤
│ initial values     (12 bytes × N)           │
│   index  (4B int32) + value (8B float64)    │
│   … sparse; only non-zero cells listed …    │
├─────────────────────────────────────────────┤
│ margin             (64 × 0xFF)              │
│   separates data segment from code segment  │
├─────────────────────────────────────────────┤
│ instructions       (16 bytes × M)           │
│   see Instruction format above              │
└─────────────────────────────────────────────┘
```

The loader scans for the first run of 64 consecutive `0xFF` bytes to locate the boundary between the data and code segments.

### Execution loop optimisation

Before entering the main loop the VM pre-computes a `bounds[]` array: for each instruction position it stores how many consecutive non-branching instructions follow before the next jump or end. The inner loop then executes that whole run without re-checking bounds on every step, reducing branch overhead for straight-line code.

---

## Instruction set

Opcodes 0–51 are available in console mode. Opcodes 52–65 require a graphics window and cause the VM to call `InitWindow` automatically.

### Memory & control flow

| Op | Name | Params | Description |
|---|---|---|---|
| 0 | `copy` | `src, dst, -` | `film[dst] = film[src]` |
| 22 | `iterate` | `pos, -, -` | `film[pos] += 1` |
| 23 | `copyFrom` | `pos, ptrPos, -` | `film[pos] = film[(int)film[ptrPos]]` - indirect load |
| 24 | `pointer` | `pos, dst, -` | `film[dst] = pos` - load address literal |
| 25 | `position` | `dst, -, -` | `film[dst] = currentInstruction` - program counter snapshot |
| 19 | `jumpIf` | `cond, addrPos, -` | jump to instruction `film[addrPos]` if `film[cond] ≠ 0` |
| 26 | `jump` | `addrPos, -, -` | unconditional jump to instruction `film[addrPos]` |
| 27 | `end` | `codePos, -, -` | halt with exit code `(int)film[codePos]` |

### Arithmetic

| Op | Name | Params | Description |
|---|---|---|---|
| 1 | `add` | `a, b, dst` | `film[dst] = film[a] + film[b]` |
| 2 | `subtract` | `a, b, dst` | `film[dst] = film[a] - film[b]` |
| 3 | `multiply` | `a, b, dst` | `film[dst] = film[a] * film[b]` |
| 4 | `divide` | `a, b, dst` | `film[dst] = film[a] / film[b]` |
| 5 | `squareRoot` | `a, dst, -` | `film[dst] = √film[a]` |
| 12 | `pow` | `base, exp, dst` | `film[dst] = film[base] ^ film[exp]` |
| 13 | `mod` | `a, b, dst` | `film[dst] = film[a] % film[b]` (floating-point) |
| 28 | `floor` | `a, dst, -` | `film[dst] = ⌊film[a]⌋` |
| 29 | `round` | `a, dst, -` | `film[dst] = round(film[a])` |
| 30 | `ceil` | `a, dst, -` | `film[dst] = ⌈film[a]⌉` |

### Trigonometry

| Op | Name | Params | Description |
|---|---|---|---|
| 6 | `sin` | `a, dst, -` | |
| 7 | `cos` | `a, dst, -` | |
| 8 | `tan` | `a, dst, -` | |
| 9 | `asin` | `a, dst, -` | |
| 10 | `acos` | `a, dst, -` | |
| 11 | `atan2` | `y, x, dst` | |

### Logic & comparison

| Op | Name | Params | Description |
|---|---|---|---|
| 14 | `equal` | `a, b, dst` | `film[dst] = film[a] == film[b]` (1.0 or 0.0) |
| 15 | `compare` | `a, b, dst` | `film[dst] = film[a] > film[b]` (1.0 or 0.0) |
| 16 | `not` | `a, dst, -` | `film[dst] = film[a] == 0 ? 1 : 0` |
| 17 | `and` | `a, b, dst` | |
| 18 | `or` | `a, b, dst` | |

### Console I/O

| Op | Name | Params | Description |
|---|---|---|---|
| 20 | `print` | `start, length, -` | Print `film[start..start+length]` as ASCII characters |
| 21 | `printNumbers` | `start, length, -` | Print `film[start..start+length]` as space-separated decimals |
| 48 | `updateConsole` | `-, -, -` | Flush the print buffer to stdout |
| 49 | `printVector` | `listId, -, -` | Print list as ASCII string |
| 50 | `printVectorNumbers` | `listId, -, -` | Print list as `[x.xx, y.yy, …]` |
| 51 | `printVectorSeparated` | `listId, sepPos, eachPos` | Print list as characters, inserting `film[sepPos]` every `film[eachPos]` elements |

Characters are encoded as `double mod 256 → ASCII`.

### Lists (`matrix`)

Lists are dynamically typed vectors of `double`. Each list is assigned an integer ID stored in `film`.

| Op | Name | Params | Description |
|---|---|---|---|
| 32 | `newList` | `dst, -, -` | Allocate new list; store its ID in `film[dst]` |
| 33 | `copyFromList` | `listId, index, dst` | `film[dst] = matrix[film[listId]][film[index]]` |
| 34 | `lengthOfList` | `listId, dst, -` | `film[dst] = len(matrix[film[listId]])` |
| 35 | `listAmount` | `dst, -, -` | `film[dst] = total number of lists` |
| 36 | `addList` | `listId, val, -` | Append `film[val]` to list |
| 37 | `removeAt` | `listId, index, -` | Remove element at `film[index]` |
| 38 | `emptyList` | `listId, -, -` | Clear list contents |
| 39 | `removeAll` | `-, -, -` | Destroy all lists |
| 40 | `addAt` | `listId, val, index` | Insert `film[val]` at `film[index]` |
| 41 | `reverseList` | `listId, -, -` | |
| 42 | `shuffleList` | `listId, -, -` | Fisher–Yates shuffle |
| 43 | `sort` | `listId, descPos, -` | Sort ascending if `film[descPos] == 0`, else descending |
| 47 | `listSet` | `listId, index, val` | `matrix[film[listId]][film[index]] = film[val]` |

### Utilities

| Op | Name | Params | Description |
|---|---|---|---|
| 31 | `time` | `dst, -, -` | `film[dst]` = current Unix time in milliseconds |
| 44 | `sleep` | `msPos, -, -` | Sleep for `film[msPos]` milliseconds |
| 45 | `random` | `dst, -, -` | `film[dst]` = uniform random double in `[0, 1)` |

### Graphics (raylib, opcodes 52–65)

Any program using opcodes in this range causes the VM to open an 800×600 resizable window at startup.

**Color** is packed into a single double as `AABBGGRR` (8 bits per channel, little-endian byte order in the integer representation).

| Op | Name | Params | Description |
|---|---|---|---|
| 52 | `setColor` | `colorPos, -, -` | Set current draw color |
| 53 | `setPosition` | `xPos, yPos, -` | Set cursor position for subsequent draw calls |
| 54 | `setTextSize` | `sizePos, -, -` | Set text size |
| 55 | `drawTriangles` | `listId, -, -` | Draw filled triangles from flat `[x0,y0, x1,y1, x2,y2, …]` list |
| 56 | `drawText` | `listId, -, -` | Draw text string encoded as a list of doubles (ASCII) |
| 57 | `beginDrawing` | `-, -, -` | Begin frame (raylib `BeginDrawing`) |
| 58 | `endDrawing` | `-, -, -` | End frame (raylib `EndDrawing`) |
| 59 | `isKeyDown` | `keyPos, dst, -` | `film[dst] = IsKeyDown((int)film[keyPos])` |
| 60 | `mousePressed` | `typePos, dst, -` | `film[dst] = IsMouseButtonDown(film[typePos])` (0=left, 1=right, 2=middle) |
| 61 | `getMouseX` | `dst, -, -` | |
| 62 | `getMouseY` | `dst, -, -` | |
| 63 | `getScroll` | `dst, -, -` | Mouse wheel delta |
| 64 | `getWidth` | `dst, -, -` | Screen width in pixels |
| 65 | `getHeight` | `dst, -, -` | Screen height in pixels |

---

## Building

**Dependencies:** a C++20 compiler and [raylib](https://github.com/raysan5/raylib) installed system-wide.

### CMake

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Manual (Linux)

```sh
g++ -O3 -std=c++20 -Wall -Wextra -Wpedantic \
    main.cpp \
    -o runner \
    -lraylib -lm -ldl -lpthread -lGL -lX11
```

---

## Usage

```sh
./runner path/to/program.filmstock
```

On exit the VM prints:

```
[Program finished in 42ms with exit code: 0]
```

To produce `.filmstock` files, see [FilmstockCASM](https://github.com/Vovencio/FilmstockCASM).
