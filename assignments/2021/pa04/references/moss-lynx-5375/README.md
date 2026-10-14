# Anonymous PA4 reference: moss-lynx-5375

`original/basic.cpp` and `original/thevenin.cpp` are the **known-buggy submitted
sources**, kept for comparison. The corresponding `fixed/` files are repaired
derivatives. The random alias is consistent across this reference's assignments.
Both archives contain no identifying strings and preserve the local source bytes:

| Archived file | SHA-256 |
| --- | --- |
| `original/basic.cpp` | `1853738558f071db0d39fb50df24c2118c7c454dc1a8bf24fe265325963e8127` |
| `original/thevenin.cpp` | `283be3e1eaee761d6ca35d30e9715c4301fa7777fcf88dd7e21a41a125213588` |

The baseline is the [January 4 specification](../../originals/PA4_MySpice_HO_20220104_1730.pdf),
course Q&A, public cases, and middle-merge example. Each path has 1–15 resistors;
each circuit has 1–15 stages. Official resistance inputs include bare Ohms and
`u/m/K/Meg/G`; Vdd fits a signed integer and has no engineering prefix.

| Original defect | Repaired behavior |
| --- | --- |
| Reading resistance and suffix as separate stream tokens consumes the next resistor name when the previous value has no suffix, so a legal `100`/`200` Ohm case loses circuit data. | Read one complete value token and parse its optional suffix. |
| RS/RP arrays store `int`: `1u` truncates to zero and values above the integer limit, including legal giga-Ohms, overflow conversion. | Preserve the one-based stage/path arrays with `long double` resistance storage. |
| Value formatters have no return for zero, negative, nano/pico currents, mega/giga resistance, or large voltage, producing undefined behavior. | Total formatter with signed/zero handling, three-place engineering values, smaller/larger prefixes and scientific fallback. |
| An early merge shifts `myspice[merge2]` repeatedly instead of shifting every following stage; merging stages 1/2 in a five-stage circuit duplicates/drops later stages. | Shift `stages[x] = stages[x+1]` for each remaining position and reconnect subsequent RS inputs. |
| Loads accumulate old counters/sums; duplicated calculation/export code can diverge. | Fresh transactional stage storage, one computation routine and one result renderer. |
| Subtractive voltage updates erase tiny valid voltages. | Retain the original backward `rm` ladder recurrence but use a multiplicative voltage divider. |
| Bonus export reuses a cached/initially zero last-node voltage, even after loading or modifying the circuit. | Recompute the current circuit immediately before Thevenin export, and guard an unloaded circuit. |

These repairs refactor the token loader and duplicated output code while keeping
the submission's `Stage` structure, fixed one-based RS/RP arrays, sums/counters,
interactive menu, ladder reduction and stage operations. Both repaired sources
are self-contained; neither includes the repository's primary implementation.
Required `void` operations are named `calculation`, `swap_stage`, and
`merge_stage`. Component/node names travel with the stage during swaps.

The Thevenin port is the final stage node versus ground, with the existing final
RP path retained. `Vth` is the unloaded external port voltage and `Rth` is the
driving-point resistance with the ideal voltage source shorted. The original
forward resistance recurrence is preserved. A 5V source with a 2K RS and 3K RP
therefore produces `Vth=3V`, `Rth=1.2K`. Old bonus example tables that label
`5V`/`5K` for that circuit do not describe this port's physical Thevenin result;
the repaired version uses the physical port result, documented explicitly here.

Compile each entry separately:

```sh
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror -pedantic-errors fixed/basic.cpp -o myspice
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror -pedantic-errors fixed/thevenin.cpp -o myspice_bonus
```

Run either program interactively. Bonus menu option 7 asks for the Thevenin output
filename. Option 5 always exports the circuit and a freshly computed table.

From the repository root, run the [validation suite](../../../../../tests/test_repaired_pa4.py):

```sh
python tests/test_repaired_pa4.py --cxx g++ --originals
```

The independent 90-digit Decimal KCL oracle stamps actual resistor endpoints and
solves a nodal matrix for voltages/currents. A separate 1A injection with the
voltage source shorted checks `Rth`. Coverage includes all six public cases,
middle/early merges, 15 stages, 15 resistors per path, all official units, zero
and negative Vdd, maximum signed Vdd, 36 fixed-seed random cases, tiny valid
15-stage attenuation, export before calculation, repeated load, missing-file
retry, and all no-circuit guards. `--originals` reproduces each archived source's
bare-Ohm failure under bounded execution.

Verified with MinGW GCC 14.2.0 using optimized C++11 and all the strict flags
above: zero compiler warnings and **64 passing scenarios per repaired entry
(128 total)**. Both archived-original bare-Ohm failures were reproduced separately.

The input reader expects ordered official netlists. Extra engineering prefixes
and scientific fallback are output extensions to retain extreme nonzero values;
this reference is not a general SPICE implementation.
