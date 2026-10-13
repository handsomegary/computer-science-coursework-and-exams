"""Independent truth-table / set-cover and circuit checks; C++11, stdlib only."""
import argparse
import itertools
import json
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
CXX = os.environ.get("CXX", "g++")
EXECUTIONS = 0


def compile_cpp(source, output):
    env = os.environ.copy()
    if Path(CXX).parent != Path("."):
        env["PATH"] = str(Path(CXX).parent) + os.pathsep + env.get("PATH", "")
    result = subprocess.run([CXX, "-std=c++11", "-Wall", "-Wextra", "-Wpedantic",
                             str(ROOT / source), "-o", str(output)],
                            capture_output=True, text=True, env=env, timeout=60)
    if result.returncode or result.stderr:
        raise AssertionError("Compilation failed or warned:\n" + result.stdout + result.stderr)
    return env


def execute(exe, cwd, args=(), stdin="", env=None):
    global EXECUTIONS
    EXECUTIONS += 1
    return subprocess.run([str(exe), *args], cwd=cwd, input=stdin, text=True,
                          capture_output=True, env=env, timeout=30)


def eqn_text(n, on, extra=""):
    names = [chr(97 + i) for i in range(n)]
    terms = ["(" + "*".join(("" if m & (1 << (n - 1 - i)) else "!") + name
                             for i, name in enumerate(names)) + ")" for m in sorted(on)]
    return ("INORDER = " + " ".join(names) + ";\nOUTORDER = output;\noutput = "
            + ("+".join(terms) if terms else "0") + extra + "\n")


def expression_cubes(text, n):
    expr = text.split("=", 1)[1].strip().rstrip(";")
    if expr == "0":
        return []
    cubes = []
    for term in expr.split("+"):
        term = term.strip().strip("()")
        if term == "1":
            cubes.append((0, 0))
            continue
        mask = bits = 0
        for literal in term.split("*"):
            name = literal[-1]
            bit = 1 << (n - 1 - (ord(name) - 97))
            mask |= bit
            if not literal.startswith("!"):
                bits |= bit
        cubes.append((bits, mask))
    return cubes


def cube_on(cubes, m):
    return any(m & mask == bits for bits, mask in cubes)


def exact_small_cover(n, on):
    """Enumerate all 3^n implicants, then independently solve coverage DP."""
    on = sorted(on)
    if not on:
        return (0, 0)
    allowed = set(on)
    valid = []
    for p in itertools.product((0, 1, 2), repeat=n):
        values = {m for m in range(1 << n)
                  if all(v == 2 or v == ((m >> (n - 1 - i)) & 1) for i, v in enumerate(p))}
        if values and values <= allowed:
            cover = sum(1 << i for i, m in enumerate(on) if m in values)
            valid.append((cover, sum(v != 2 for v in p)))
    dp = {0: (0, 0)}
    for cover, literals in valid:
        for old, cost in list(dp.items()):
            merged = old | cover
            new = (cost[0] + 1, cost[1] + literals)
            if new < dp.get(merged, (100, 1000)):
                dp[merged] = new
    return dp[(1 << len(on)) - 1]


class LogicTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory()
        cls.exe = Path(cls.build.name) / "logic_min.exe"
        cls.env = compile_cpp("assignments/2021/pa05/main.cpp", cls.exe)

    @classmethod
    def tearDownClass(cls):
        cls.build.cleanup()

    def solve(self, text, expected_code=0):
        with tempfile.TemporaryDirectory() as directory:
            p = Path(directory)
            (p / "in.eqn").write_text(text, encoding="utf-8")
            r = execute(self.exe, p, ["in.eqn", "out.eqn"], env=self.env)
            self.assertEqual(r.returncode, expected_code, r.stderr)
            if expected_code:
                self.assertFalse((p / "out.eqn").exists())
                return r
            self.assertEqual(r.stdout, "")
            self.assertEqual(r.stderr, "")
            return (p / "out.eqn").read_text(encoding="utf-8")

    def test_all_three_variable_functions_minimum(self):
        for bitmap in range(256):
            on = {m for m in range(8) if bitmap & (1 << m)}
            answer = self.solve(eqn_text(3, on))
            cubes = expression_cubes(answer.splitlines()[2], 3)
            self.assertEqual({m for m in range(8) if cube_on(cubes, m)}, on)
            cost = len(cubes), sum(mask.bit_count() for _, mask in cubes)
            self.assertEqual(cost, exact_small_cover(3, on), bitmap)

    def test_four_variable_cover_and_published_case(self):
        rng = random.Random(2021)
        cases = [{0, 5, 6, 7, 9, 10, 13, 14, 15}]
        cases += [{m for m in range(16) if rng.randrange(2)} for _ in range(28)]
        for on in cases:
            cubes = expression_cubes(self.solve(eqn_text(4, on)).splitlines()[2], 4)
            self.assertEqual({m for m in range(16) if cube_on(cubes, m)}, on)
            self.assertEqual((len(cubes), sum(mask.bit_count() for _, mask in cubes)), exact_small_cover(4, on))

    def test_published_sparse_and_twenty_six_variables(self):
        cases = [(7, {1, 3, 4, 5, 9, 13, 14, 15, 16, 19, 22, 24, 26, 29, 30, 31, 36, 37,
                      38, 40, 72, 75, 79, 80, 85, 87, 91, 94, 101}),
                 (13, {1, 2, 3, 4, 8, 9, 16, 17, 32, 33, 65, 66, 67, 129, 130, 131, 256,
                       257, 513, 514, 515, 1025, 1026, 1027, 2050, 2051, 4100, 4101, 4102})]
        for n, on in cases:
            cubes = expression_cubes(self.solve(eqn_text(n, on)).splitlines()[2], n)
            self.assertEqual({m for m in range(1 << n) if cube_on(cubes, m)}, on)
        on = {0, 1, 2, 3, (1 << 26) - 1}
        cubes = expression_cubes(self.solve(eqn_text(26, on)).splitlines()[2], 26)
        self.assertEqual(len(cubes), 2)
        self.assertTrue(all(cube_on(cubes, m) for m in on))
        self.assertEqual(sum(1 << (26 - mask.bit_count()) for _, mask in cubes), len(on))

    def test_extensions_headers_and_validation(self):
        text = "INORDER = a b c; // preserve me\nOUTORDER = output;\noutput = (a+b)*!c + a*!a;\n"
        answer = self.solve(text)
        self.assertEqual(answer.splitlines()[0], text.splitlines()[0])
        cubes = expression_cubes(answer.splitlines()[2], 3)
        for a, b, c in itertools.product((0, 1), repeat=3):
            self.assertEqual(cube_on(cubes, 4*a + 2*b + c), bool((a or b) and not c))
        self.assertIn("output = 1\n", self.solve(eqn_text(26, set()).replace("output = 0", "output = 1")))
        for text in ["", "INORDER = a a;\nOUTORDER = output;\noutput=a;\n",
                     "INORDER = a;\nOUTORDER = output;\nwrong=a;\n",
                     "INORDER = a;\nOUTORDER = output;\noutput=(!a;\n",
                     "INORDER = a;\nOUTORDER = output;\noutput=z;\n",
                     eqn_text(26, set()).replace("output = 0", "output = a")]:
            self.solve(text, 1)


PUBLIC_TABLE1 = """Flip-Flop: D
//Present(A,B)
0 0
0 0
0 1
0 1
1 0
1 0
1 1
1 1
//Input(x)
0
1
0
1
0
1
0
1
//Next(A,B)
0 0
0 1
0 0
1 0
0 0
1 1
0 0
1 1
//Output(y)
0
0
0
0
0
0
1
1
"""
PUBLIC_TABLE2 = """Flip-Flop: D
//Present(A)
0
0
0
0
1
1
1
1
//Input(x,y)
0 0
0 1
1 0
1 1
0 0
0 1
1 0
1 1
//Next(A)
0
0
0
1
1
1
1
1
//Output(0)
"""


def state_table(ff, present, inputs, outputs, rows, next_order=None, section_order=None):
    next_order = next_order or present
    sections = {"Present": (present, [r[0] for r in rows]),
                "Input": (inputs, [r[1] for r in rows]),
                "Next": (next_order, [[r[2][present.index(name)] for name in next_order] for r in rows]),
                "Output": (outputs, [r[3] for r in rows])}
    text = "Flip-Flop: " + ff + "\n"
    for label in section_order or ("Present", "Input", "Next", "Output"):
        names, values = sections[label]
        text += "//" + label + "(" + (",".join(names) if names else "0") + ")\n"
        if names:
            text += "".join(" ".join(map(str, row)) + "\n" for row in values)
    return text


def evaluate_bench(text, present, input_values):
    values = {"0": False, "1": True, **present, **input_values}
    definitions, flipflops = {}, {}
    for line in text.splitlines():
        match = re.fullmatch(r"([!A-Za-z_][!A-Za-z_0-9]*) = ([A-Z]+)\(([^()]*)\)", line)
        if match:
            node, operation, args = match.groups()
            if operation in ("D", "T", "JK"):
                flipflops[node] = (operation, args.split(",")[:-1])
            else:
                definitions[node] = (operation, args.split(","))

    def get(name):
        if name in values:
            return values[name]
        operation, args = definitions[name]
        bits = [get(a) for a in args]
        if operation == "AND":
            result = all(bits)
        elif operation == "OR":
            result = any(bits)
        elif operation == "NOT":
            result = not bits[0]
        elif operation == "BUFF":
            result = bits[0]
        else:
            raise AssertionError("unknown gate " + operation)
        values[name] = result
        return result

    future = {}
    for name, (ff, args) in flipflops.items():
        if ff == "D":
            future[name] = get(args[0])
        elif ff == "T":
            future[name] = bool(present[name]) ^ get(args[0])
        else:
            j, k = map(get, args)
            future[name] = (j and not present[name]) or (not k and present[name])
    output_names = re.findall(r"^OUTPUT\(([^()]+)\)$", text, re.M)
    return future, {name: get(name) for name in output_names}


class SynthesisTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory()
        cls.base = Path(cls.build.name) / "synth.exe"
        cls.bonus = Path(cls.build.name) / "synth_bonus.exe"
        cls.env = compile_cpp("assignments/2021/final/main.cpp", cls.base)
        compile_cpp("assignments/2021/final/bonus.cpp", cls.bonus)

    @classmethod
    def tearDownClass(cls):
        cls.build.cleanup()

    def solve(self, table, extended=False, incomplete=False, code=0):
        with tempfile.TemporaryDirectory() as directory:
            p = Path(directory)
            (p / "states.txt").write_text(table, encoding="utf-8")
            args = ["states.txt", "circuit.bench"] + (["--allow-incomplete"] if incomplete else [])
            r = execute(self.bonus if extended else self.base, p, args, env=self.env)
            self.assertEqual(r.returncode, code, r.stderr)
            self.assertEqual(r.stdout, "")
            if code:
                return r
            self.assertEqual(r.stderr, "")
            return {name: (p / name).read_text(encoding="utf-8")
                    for name in ("kmap_in.txt", "kmap_out.txt", "circuit.bench")}

    def verify_rows(self, bench, present, inputs, outputs, rows):
        for p, i, future, y in rows:
            got_p, got_y = evaluate_bench(bench, dict(zip(present, p)), dict(zip(inputs, i)))
            self.assertEqual(got_p, dict(zip(present, map(bool, future))))
            self.assertEqual(got_y, dict(zip(outputs, map(bool, y))))

    def test_published_kmap_and_bench(self):
        result = self.solve(PUBLIC_TABLE1)
        self.assertEqual(result["kmap_in.txt"], "3\n3\n0 0 1 0 0 1 1 0\n3\n0 1 0 0 0 1 1 0\n3\n0 0 0 0 0 0 1 1\n")
        self.assertIn("# 6 gates (4 AND + 2 OR)", result["circuit.bench"])
        for a, b, x in itertools.product((0, 1), repeat=3):
            future, output = evaluate_bench(result["circuit.bench"], {"A": a, "B": b}, {"x": x})
            self.assertEqual(future, {"A": bool(x and (a or b)), "B": bool(x and (a or not b))})
            self.assertEqual(output, {"y": bool(a and b)})
        result = self.solve(PUBLIC_TABLE2)
        self.assertEqual(result["kmap_in.txt"], "1\n3\n0 0 1 0 1 1 1 1\n")
        self.assertIn("# 2 gates (1 AND + 1 OR)", result["circuit.bench"])
        for a, x, y in itertools.product((0, 1), repeat=3):
            future, output = evaluate_bench(result["circuit.bench"], {"A": a}, {"x": x, "y": y})
            self.assertEqual(future, {"A": bool(a or x and y)})
            self.assertEqual(output, {})

    def test_random_tables_renaming_permutations_and_bonus(self):
        rng = random.Random(431)
        for n in range(1, 5):
            for ff in ("D", "T", "JK"):
                present = ["N1"] + (["Memory"] if n > 2 else [])
                inputs = ["control" + str(i) for i in range(n - len(present))]
                outputs = ["signal", "empty"]
                rows = []
                for bits in itertools.product((0, 1), repeat=n):
                    p, i = list(bits[:len(present)]), list(bits[len(present):])
                    rows.append((p, i, [rng.randrange(2) for _ in p], [rng.randrange(2), 0]))
                rng.shuffle(rows)
                table = state_table(ff, present, inputs, outputs, rows, list(reversed(present)),
                                    ["Output", "Input", "Next", "Present"])
                result = self.solve(table, ff != "D")
                self.verify_rows(result["circuit.bench"], present, inputs, outputs, rows)

    def test_constants_zero_variables_and_extended_width(self):
        rows = [([], [], [], [1, 0])]
        result = self.solve(state_table("D", [], [], ["yes", "no"], rows))
        self.assertEqual(evaluate_bench(result["circuit.bench"], {}, {}), ({}, {"yes": True, "no": False}))
        self.assertEqual(result["kmap_out.txt"], "#1\n1\n#2\n")
        rows = [([q], list(bits), [q ^ bits[0]], [int(all(bits))])
                for q in (0, 1) for bits in itertools.product((0, 1), repeat=7)]
        table = state_table("T", ["Q"], ["x" + str(i) for i in range(7)], ["all"], rows)
        result = self.solve(table, True)
        self.verify_rows(result["circuit.bench"], ["Q"], ["x" + str(i) for i in range(7)], ["all"], rows)
        self.solve(table.replace("Flip-Flop: T", "Flip-Flop: D"), code=1)

    def test_incomplete_and_invalid(self):
        rows = [([0], [0], [1], [1]), ([1], [1], [0], [0])]
        table = state_table("JK", ["Q"], ["x"], ["out"], rows)
        result = self.solve(table, True, True)
        self.verify_rows(result["circuit.bench"], ["Q"], ["x"], ["out"], rows)
        self.solve(table, True, code=1)
        for table in ["", PUBLIC_TABLE2.replace("0 0\n", "999999999999999999999999999999 0\n", 1),
                      PUBLIC_TABLE2.replace("//Next(A)", "//Next(B)"),
                      PUBLIC_TABLE2.replace("//Input(x,y)", "//Input(A,y)"),
                      PUBLIC_TABLE2.replace("//Present(A)\n0\n", "//Present(A)\n"),
                      PUBLIC_TABLE2.replace("//Input(x,y)\n0 0", "//Input(x,y)\n0 1"),
                      PUBLIC_TABLE2.replace("//Input(x,y)", "//Input(CLK,y)")]:
            self.solve(table, code=1)


# Independent nodal circuit model for PA4.
from decimal import Decimal, localcontext


def pa4_circuit(stages, voltage=5):
    """Generate the official ladder netlist; each tuple is (ID, series, shunt)."""
    lines = [f"stage {len(stages)}", f"v1 vdd gnd {voltage}V"]
    before = "vdd"
    edges = []
    for ident, series, shunt in stages:
        junction = f"n{ident}"
        node = before
        for j, resistance in enumerate(series, 1):
            target = junction if j == len(series) else f"ns{ident}_{j}"
            edge = (f"RS{ident}_{j}", node, target, Decimal(str(resistance)))
            edges.append(edge)
            lines.append(" ".join(map(str, edge)))
            node = target
        node = junction
        for j, resistance in enumerate(shunt, 1):
            target = "gnd" if j == len(shunt) else f"np{ident}_{j}"
            edge = (f"RP{ident}_{j}", node, target, Decimal(str(resistance)))
            edges.append(edge)
            lines.append(" ".join(map(str, edge)))
            node = target
        before = junction
    lines.append("end")
    return "\n".join(lines) + "\n", edges


def pa4_linear_solve(matrix, rhs):
    """General dense Gaussian elimination, unrelated to ladder reduction."""
    n = len(rhs)
    a = [row[:] + [b] for row, b in zip(matrix, rhs)]
    for col in range(n):
        pivot = max(range(col, n), key=lambda row: abs(a[row][col]))
        a[col], a[pivot] = a[pivot], a[col]
        if not a[col][col]:
            raise AssertionError("singular circuit matrix")
        for row in range(col + 1, n):
            factor = a[row][col] / a[col][col]
            if factor:
                for j in range(col + 1, n + 1):
                    a[row][j] -= factor * a[col][j]
                a[row][col] = Decimal(0)
    values = [Decimal(0)] * n
    for row in range(n - 1, -1, -1):
        values[row] = (a[row][n] - sum(a[row][col] * values[col]
                                     for col in range(row + 1, n))) / a[row][row]
    return values


def pa4_nodal(edges, source, port):
    """Stamp KCL from resistor endpoints; inject 1 A to measure source-shorted Rth."""
    with localcontext() as ctx:
        ctx.prec = 90
        names = sorted({node for _, a, b, _ in edges for node in (a, b)} - {"vdd", "gnd"})
        index = {name: i for i, name in enumerate(names)}
        size = len(names)
        matrix = [[Decimal(0)] * size for _ in names]
        source_rhs = [Decimal(0)] * size
        for _, a, b, resistance in edges:
            conductance = Decimal(1) / resistance
            for node, other in ((a, b), (b, a)):
                if node not in index:
                    continue
                row = index[node]
                matrix[row][row] += conductance
                if other in index:
                    matrix[row][index[other]] -= conductance
                elif other == "vdd":
                    source_rhs[row] += conductance
        unit = pa4_linear_solve(matrix, source_rhs)
        volts = {name: value * Decimal(source) for name, value in zip(names, unit)}
        volts.update(vdd=Decimal(source), gnd=Decimal(0))
        currents = {name: (volts[a] - volts[b]) / resistance for name, a, b, resistance in edges}
        source_current = sum((Decimal(1) - unit[index[b]]) / resistance
                             for _, a, b, resistance in edges if a == "vdd")
        injected = [Decimal(0)] * size
        injected[index[port]] = Decimal(1)
        shorted = pa4_linear_solve(matrix, injected)
        return ({"input_resistance": float(Decimal(1) / source_current),
                 "vth": float(volts[port]), "rth": float(shorted[index[port]])},
                {k: float(v) for k, v in volts.items()},
                {k: float(v) for k, v in currents.items()})


class PA4Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build = tempfile.TemporaryDirectory()
        cls.exe = Path(cls.build.name) / "myspice.exe"
        cls.bonus = Path(cls.build.name) / "myspice_bonus.exe"
        cls.env = compile_cpp("assignments/2021/pa04/main.cpp", cls.exe)
        compile_cpp("assignments/2021/pa04/bonus.cpp", cls.bonus)

    @classmethod
    def tearDownClass(cls):
        cls.build.cleanup()

    def solve(self, netlist, args=(), bonus=False):
        with tempfile.TemporaryDirectory() as directory:
            p = Path(directory)
            (p / "in.txt").write_text(netlist, encoding="utf-8")
            result = execute(self.bonus if bonus else self.exe, p,
                             ["in.txt", "out.txt", *args], env=self.env)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stdout, "")
            self.assertEqual(result.stderr, "")
            return (p / "out.txt").read_text(encoding="utf-8")

    def assert_nodal(self, stages, source=5, args=(), expected_edges=None, port=None):
        text, edges = pa4_circuit(stages, source)
        if expected_edges is not None:
            edges = expected_edges
        if port is None:
            port = f"n{stages[-1][0]}"
        answer = self.solve(text, [*args, "--raw"])
        scalars, volts, currents = {}, {}, {}
        for line in answer.splitlines():
            words = line.split()
            if words[0] == "voltage":
                volts[words[1]] = float(words[2])
            elif words[0] == "current":
                currents[words[1]] = float(words[2])
            else:
                scalars[words[0]] = float(words[1])
        want = pa4_nodal(edges, source, port)
        for actual, expected in zip((scalars, volts, currents), want):
            self.assertEqual(actual.keys(), expected.keys())
            for key in expected:
                self.assertTrue(abs(actual[key] - expected[key]) <=
                                max(abs(expected[key]) * 2e-12, 1e-290),
                                (key, actual[key], expected[key]))
        return scalars, volts, currents

    def test_general_nodal_random_and_shuffled_netlists(self):
        rng = random.Random(4042021)
        for _ in range(55):
            stages = [(i, [rng.randint(1, 999) * (10 ** rng.randint(-3, 6))
                           for _ in range(rng.randint(1, 4))],
                       [rng.randint(1, 999) * (10 ** rng.randint(-3, 6))
                        for _ in range(rng.randint(1, 4))])
                      for i in range(1, rng.randint(1, 7) + 1)]
            self.assert_nodal(stages, rng.choice([-12, 0, 1, 5, 123]))
        stages = [(1, [2000, 2000], [2000]), (2, [2000], [3000, 3000])]
        text, _ = pa4_circuit(stages)
        ordered = self.solve(text, ["--raw"])
        lines = text.splitlines()
        resistors = lines[2:-1]
        rng.shuffle(resistors)
        shuffled = "\n".join(lines[:2] + resistors + ["end"]) + "\n"
        self.assertEqual(self.solve(shuffled, ["--raw"]), ordered)
        self.assertEqual(self.solve("\ufeff" + text, ["--raw"]), ordered)

    def test_all_15_stages_and_15_resistors_per_path(self):
        stages = [(i, [i + 1] * 15, [20 - i] * 15) for i in range(1, 16)]
        self.assert_nodal(stages, 123)

    def test_engineering_range_and_deep_attenuation(self):
        for scale in (Decimal("0.000001"), Decimal("1000000000")):
            stages = [(1, [scale, scale * 999], [scale * 7]),
                      (2, [scale * 19], [scale * 2, scale * 3])]
            self.assert_nodal(stages, 3)
        stages = [(1, [Decimal("999000000000")], [Decimal("0.000001")]),
                  (2, [Decimal("999000000000")], [Decimal("0.000001")])]
        self.assert_nodal(stages, 1)
        deep = [(i, [Decimal("999000000000")], [Decimal("0.000001")]) for i in range(1, 16)]
        scalar, volts, _ = self.assert_nodal(deep, 1)
        self.assertGreater(volts["n15"], 0)
        self.assertLess(scalar["vth"], 1e-200)

    def test_public_case_1_swap_merge_and_true_thevenin(self):
        original = [(1, [2000, 2000], [2000]), (2, [2000], [3000, 3000])]
        edges = [("RS2_1", "vdd", "n2", Decimal(2000)),
                 ("RP1", "n2", "gnd", Decimal(3000))]
        scalars, volts, currents = self.assert_nodal(
            original, args=["--swap", "1", "2", "--merge", "1", "2"],
            expected_edges=edges, port="n2")
        self.assertEqual(volts["n2"], 3)
        self.assertEqual(currents, {"RP1": .001, "RS2_1": .001})
        self.assertEqual(scalars, {"input_resistance": 5000, "vth": 3, "rth": 1200})
        text, _ = pa4_circuit(original)
        ops = ["--swap", "1", "2", "--merge", "1", "2"]
        true = self.solve(text, ops, bonus=True)
        legacy = self.solve(text, [*ops, "--legacy-thevenin"], bonus=True)
        self.assertEqual(true, "***Thevenin equivalence circuit***\nRth = 1.200K\nVth = 3.000v\n")
        self.assertEqual(legacy, "***Thevenin equivalence circuit***\nRth = 5.000K\nVth = 5.000v\n")

    def test_public_case_2_swaps_and_reconnect_following_stage(self):
        original = [(1, [2000], [2000]), (2, [2000], [2000] * 4),
                    (3, [2000] * 3, [2000] * 2)]
        _, expected = pa4_circuit([original[1], original[2], original[0]], 6)
        scalars, volts, currents = self.assert_nodal(
            original, 6, ["--swap", "1", "2", "--swap", "2", "3"], expected, "n1")
        self.assertEqual([volts[n] for n in ("n2", "n3", "n1")], [4, 1, .5])
        self.assertEqual(scalars["input_resistance"], 6000)
        self.assertEqual(currents["RS2_1"], .001)
        self.assertEqual(currents["RP2_1"], .0005)
        self.assertEqual(currents["RP3_1"], .00025)

    def test_public_case_3_merges(self):
        original = [(1, [5000], [5000, 5000]), (2, [2000], [8000, 8000]),
                    (3, [8000], [8000, 8000]), (4, [12000], [4000, 4000]),
                    (5, [4000], [2000, 2000])]
        _, expected = pa4_circuit(original[:2] + [(3, [8000], [8000])])
        expected[-1] = ("RP3", *expected[-1][1:])
        _, volts, currents = self.assert_nodal(
            original, args=["--merge", "4", "5", "--merge", "3", "4"],
            expected_edges=expected, port="n3")
        self.assertEqual([volts[n] for n in ("n1", "n2", "n3")], [2.5, 2, 1])
        self.assertEqual(currents["RS1_1"], .0005)
        self.assertEqual(currents["RP2_1"], .000125)

    def test_middle_merge_recomputes_entire_network(self):
        original = [(1, [2000], [16000]), (2, [8000], [8000]), (3, [4000], [4000])]
        expected = [("RS1_1", "vdd", "n1", Decimal(2000)),
                    ("RP1", "n1", "gnd", Decimal(8000)),
                    ("RS3_1", "n1", "n3", Decimal(4000)),
                    ("RP3_1", "n3", "gnd", Decimal(4000))]
        scalars, volts, _ = self.assert_nodal(original, 6, ["--merge", "2", "1"], expected, "n3")
        self.assertEqual(scalars["input_resistance"], 6000)
        self.assertEqual([volts["n1"], volts["n3"]], [4, 2])
        before = self.assert_nodal(original, 6)[0]["input_resistance"]
        self.assertNotEqual(before, scalars["input_resistance"])

    def test_prefixes_rounding_export_reload_and_internal_node_display(self):
        netlist = "stage 1\nv1 vdd gnd 1V\nRS1_1 vdd n1 499.99975\nRP1_1 n1 gnd 499.99975\nend\n"
        legacy = self.solve(netlist, ["--legacy-thevenin"], bonus=True)
        self.assertIn("Rth = 1.000K\n", legacy)
        stages = [(1, [2000, 2000], [2000]), (2, [2000], [3000, 3000])]
        text, _ = pa4_circuit(stages)
        export = self.solve(text, ["--all-nodes"])
        self.assertIn("ns1_1", export)
        self.assertIn("np2_1", export)
        self.assertIn("end\n\n***calculation results***\n", export)
        self.assertEqual(self.solve(export, ["--raw"]), self.solve(text, ["--raw"]))
        table = export.split("***calculation results***\n")[1]
        self.assertEqual({len(line) for line in table.splitlines()}, {38})
        for unit, factor in (("u", "0.000001"), ("m", "0.001"), ("", "1"),
                             ("K", "1000"), ("Meg", "1000000"), ("G", "1000000000")):
            network = f"stage 1\nV1 vdd gnd -5v\nRS1_1 vdd n1 1{unit}\nRP1_1 n1 gnd 2{unit}\nend\n"
            answer = self.solve(network, ["--raw"])
            scalar = dict(line.split() for line in answer.splitlines() if len(line.split()) == 2)
            self.assertAlmostEqual(float(scalar["input_resistance"]), 3 * float(factor))
            self.assertAlmostEqual(float(scalar["vth"]), -10 / 3)

    def test_interactive_missing_file_guards_and_retries(self):
        with tempfile.TemporaryDirectory() as directory:
            p = Path(directory)
            text, _ = pa4_circuit([(1, [2000], [2000]), (2, [2000], [6000])])
            (p / "good.txt").write_text(text, encoding="utf-8")
            commands = "2\n3\n4\n5\n7\n1\nmissing.txt\ngood.txt\n3\n4\n1\n1\n2\n4\n1\n2\n5\nresult.txt\n7\nthevenin.txt\n6\n"
            result = execute(self.bonus, p, stdin=commands, env=self.env)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(result.stderr, "")
            self.assertEqual(result.stdout.count("There is no circuit"), 5)
            self.assertIn("The file is not found.\nPlease enter the name of the input file again: ", result.stdout)
            self.assertIn("The node doesn't exist. Please enter again.", result.stdout)
            self.assertIn("The stages must be adjacent. Please enter again.", result.stdout)
            self.assertIn("The stage is successfully swapped!", result.stdout)
            self.assertIn("The stage is successfully merged!", result.stdout)
            self.assertIn("Bye~\n", result.stdout)
            self.assertTrue((p / "result.txt").exists())
            self.assertTrue((p / "thevenin.txt").exists())

    def test_rejects_invalid_topology_and_invalid_operations(self):
        good = "stage 1\nv1 vdd gnd 5V\nRS1_1 vdd n1 2K\nRP1_1 n1 gnd 2K\nend\n"
        invalid = [good.replace("stage 1", "stage 0"),
                   good.replace("stage 1", "stage 2"),
                   good.replace("v1 vdd gnd 5V\n", ""),
                   good.replace("2K", "0K", 1),
                   good.replace("5V", "5mV"),
                   good.replace("RP1_1 n1 gnd 2K\n", ""),
                   good.replace("end", "RS2_1 extra gnd 2K\nend"),
                   good.replace("end", "RS1_1 vdd n1 2K\nend"),
                   good.replace("end", "RP2_1 n1 gnd 3K\nend")]
        overflow, _ = pa4_circuit([(1, [Decimal("1e4931")] * 15, [1])])
        underflow, _ = pa4_circuit([(1, [Decimal("1e4931")], [Decimal("1e-4900")])])
        invalid += [overflow, underflow]
        for network in invalid:
            with tempfile.TemporaryDirectory() as directory:
                p = Path(directory)
                (p / "in.txt").write_text(network, encoding="utf-8")
                result = execute(self.exe, p, ["in.txt", "out.txt"], env=self.env)
                self.assertEqual(result.returncode, 1)
                self.assertTrue(result.stderr)
                self.assertFalse((p / "out.txt").exists())
        three, _ = pa4_circuit([(1, [1], [2]), (2, [3], [4]), (3, [5], [6])])
        for args in (["--swap", "1", "3"], ["--merge", "0", "1"],
                     ["--swap", "1", "1"], ["--unknown"], ["--merge", "1"]):
            with tempfile.TemporaryDirectory() as directory:
                p = Path(directory)
                (p / "in.txt").write_text(three, encoding="utf-8")
                result = execute(self.exe, p, ["in.txt", "out.txt", *args], env=self.env)
                self.assertEqual(result.returncode, 1)
                self.assertFalse((p / "out.txt").exists())

    def test_repeated_merges_keep_names_unique(self):
        stages = [(i, [2000], [4000]) for i in range(1, 6)]
        text, _ = pa4_circuit(stages)
        output = self.solve(text, ["--merge", "2", "3", "--swap", "1", "2", "--merge", "2", "3"])
        names = [line.split()[0] for line in output.split("end\n", 1)[0].splitlines()
                 if line.startswith("R")]
        self.assertEqual(len(names), len(set(names)))
        self.assertIn("RP2_m1", names)
        self.assertTrue(self.solve(output, ["--raw"]))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=CXX)
    args, remaining = parser.parse_known_args()
    CXX = args.cxx
    try:
        unittest.main(argv=[__file__, *remaining])
    finally:
        print("Program executions:", EXECUTIONS)
