# Anonymous PA4 reference: jade-lynx-c0ee

`original/basic.cpp` is the **known-buggy submitted source**, kept for comparison.
`fixed/basic.cpp` is the repaired derivative. The random alias identifies this
reference consistently without publishing a student name or ID. The archived
source contains no identifying strings and is byte-identical to the local source;
its SHA-256 is `5ee004fb8b1e39b6eb5b7ad2759e31346ef322b04f3ac376111031059b88e78a`.

The validation baseline is the [January 4 specification](../../originals/PA4_MySpice_HO_20220104_1730.pdf),
the course Q&A, three public cases, and the middle-merge example. It includes
1–15 stages and 1–15 resistors in each RS/RP path, bare Ohms, and `u/m/K/Meg/G`.

| Original defect | Repaired behavior |
| --- | --- |
| Two-digit stage IDs are parsed by adding digits: `RS10_1` is put in stage 1 instead of stage 10. A legal 15-stage circuit later indexes empty resistor vectors and can crash. | Parse the complete decimal ID; validate stage/path bounds before installing a circuit. |
| The formatter decrements a six-element prefix-array index below zero for currents smaller than `1uA`; zero and negative values can loop or access invalid memory. | Use the absolute magnitude, explicitly handle zero, and support `n/p/f` plus scientific notation for extreme valid attenuation. |
| Voltage arrays are variable-length arrays, which are a GCC extension rather than C++11. A one-stage calculation also reads `node_voltage[2]` outside the array. | Use sized vectors and remove the unused out-of-range read. |
| Loading a second circuit appends resistors to the first; exporting without a fresh calculation uses uninitialized/stale currents. | Parse into fresh stage storage; every calculation and export recomputes the current circuit. |
| Stage voltage uses subtraction of nearly equal quantities, losing very small legal node voltages. | Use the same ladder resistance reduction with a multiplicative voltage divider and `long double` storage. |
| No-circuit messages and file-result formatting drift from the updated wording. | All unloaded operations print `There is no circuit`; exports use three-place values, capital `K`, and the file-only calculation heading. |

This is a substantial repair and refactoring of the input and duplicated output
functions. It retains the submission's `resistor`, `stage_information`,
`vector<resistor>` RS/RP paths, interactive menu, backward ladder reduction,
stage swapping, and adjacent-stage merge formula. The repaired source is
self-contained; it does not include the repository's primary implementation.
The three required `void` operations are `calculation`, `swap_stage`, and
`merge_stage`. Original circuit names move with swapped stages, and following
stages are reconnected after every swap or merge.

Build the repaired program:

```sh
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror -pedantic-errors fixed/basic.cpp -o myspice
./myspice
```

From the repository root, run the [reproducible validation](../../../../../tests/test_repaired_pa4.py):

```sh
python tests/test_repaired_pa4.py --cxx g++ --originals
```

The tests build sources afresh. They compare displayed and exported voltages,
currents, topology, and rounded resistances against an independent 90-digit
Decimal nodal KCL solve. Coverage includes all six public load/operation cases,
middle and early merges, both maximum stage/path sizes, every official prefix,
zero/negative/int-maximum voltage, 36 fixed-seed random circuits, very small
15-stage voltages, export before calculation, file retry, repeated load, and
unloaded-operation guards. `--originals` separately reproduces the 15-stage
original failure under a two-second execution and one-MiB output cap.

Verified with MinGW GCC 14.2.0 using optimized C++11 and all the strict flags
above: zero compiler warnings and **64 passing repaired-program scenarios**.
The archived-original 15-stage failure was reproduced separately.

The repaired input remains an ordered official netlist reader, not a general
SPICE parser. Extra engineering prefixes and scientific fallback are documented
format extensions needed to preserve nonzero results outside `u..G`.
