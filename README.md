# 2×2×2 Cube Solver

A robot that reads a scrambled 2×2×2 cube with a camera, searches for an optimal
solution, and turns the cube to solve it. Runs on a Raspberry Pi driving two
stepper motors.

The whole loop is here — perception, search, and actuation — and the program
reports how long each phase took.

```
    2x2x2 Cube Solver
-------------------------
 Move Count     :      9

 Scan Time      :   6.31
 Search Time    :   0.42
 Actuation Time :  11.87
-------------------------
 Total Time     :  18.60
```

---

## How it works

### Scan

The cube sits in a two-motor cradle: one motor rotates the cube about the
vertical axis, the other flips it. `Sensor::scan_cube` drives the motors through
a fixed sequence that brings each of the six faces into view, captures an image
of each, samples the sticker positions, and classifies each sample in HSV.

The result is a 24-character state string, one character per sticker. A validity
check runs before search — a miscount of any color means the scan is wrong, not
the cube, and it is better to fail loudly than to search for a solution that
cannot exist.

### Search

`Solution::search` is a bidirectional breadth-first search. One frontier expands
from the scanned state, the other from the solved state, and the two are compared
after every level. When a state appears in both, the forward path and the
reversed inverse of the backward path are spliced into a full solution.

Meeting in the middle is what makes this cheap. God's number for the 2×2×2 is 11
in the half-turn metric, so a single-directional search would have to reach depth
11; two frontiers only need about half that each, which is why the loop runs six
levels. Moves are generated over `{R, U, F}` and their doubles, with consecutive
turns of the same face pruned.

### Actuation

The search returns face turns in cube notation, but the motors can only turn the
cube about two fixed axes — so the same notation means a different physical
motion depending on how the cube is currently oriented.

`Solution::get_commands` therefore tracks the cube's orientation as a `view`
index through the whole solution. For each move it looks up which axis that face
currently lies on, emits the reorientation commands needed to bring it under a
motor, emits the turn itself, and updates the view. The output is a flat list of
`(motor index, angle)` pairs that `main` plays back.

---

## Layout

```
cube_solver.cpp   main: scan → search → actuate, with phase timing
cube.hpp/.cpp     cube state, move application, bidirectional search,
                  notation-to-motor-command translation
solver.hpp/.cpp   Motor (GPIO stepper control), Sensor (capture and color classification)
```

## Building

Requires OpenCV and [wiringPi](https://github.com/WiringPi/WiringPi) on a
Raspberry Pi.

```bash
g++ -o cube_solver cube_solver.cpp cube.cpp solver.cpp \
    $(pkg-config --cflags --libs opencv4) -lwiringPi
sudo ./cube_solver
```

Motor step and direction pins are set in `main` (GPIO 8/7 and 14/15). Color
thresholds are tuned for the lighting of the rig it was built for and will need
adjusting elsewhere.
