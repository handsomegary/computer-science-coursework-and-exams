"""Repaired anonymous PA4 submissions: C++11 + independent 90-digit nodal KCL.

Run: python tests/test_repaired_pa4.py --cxx g++ [--report report.json]
Optional --originals reproduces a known failure of each archived original. It
builds source in a temporary directory; it never runs a supplied executable.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile
import threading
import unittest

from test_pa2021_04_05_final import pa4_circuit, pa4_nodal

ROOT = Path(__file__).resolve().parents[1]
CXX = os.environ.get("CXX", "g++")
TEMP_BASE = os.environ.get("PA4_TEST_TEMP")
REPRODUCE_ORIGINALS = False
REUSE_CURRENT_BUILD = False
REPORT = {"specification": "PA4_MySpice_HO_20220104_1730 + course Q&A",
          "oracle": "90-digit Decimal KCL matrix stamped from resistor endpoints",
          "standard": "C++11", "optimization": "-O2", "sources": {}}
VARIANTS = ("jade-lynx-c0ee/basic", "moss-lynx-5375/basic", "moss-lynx-5375/thevenin")
NUMBER = r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?"
PREFIX = {"f": 1e-15, "p": 1e-12, "n": 1e-9, "u": 1e-6, "m": 1e-3,
          "": 1, "K": 1e3, "Meg": 1e6, "G": 1e9, "T": 1e12, "P": 1e15}


def number(text):
    match = re.fullmatch("(" + NUMBER + r")(f|p|n|u|m|K|Meg|G|T|P)?", text)
    if not match:
        raise AssertionError("invalid engineering value: " + text)
    raw, prefix = match.groups()
    multiplier = PREFIX[prefix or ""]
    exponent = int(raw.split("e")[-1]) if "e" in raw else int(raw.split("E")[-1]) if "E" in raw else 0
    return float(raw) * multiplier, .000501 * 10.0**exponent * multiplier


def results(text):
    found = {}
    for line in text.splitlines():
        match = re.fullmatch(r"\*\*\s+(\w+)\s+(" + NUMBER + r"(?:f|p|n|u|m|K|Meg|G|T|P)?)(v|A)\s+\*\*", line)
        if match:
            name, value, unit = match.groups()
            found[name] = (*number(value), unit, value)
    return found


def text_circuit(stages, voltage):
    text, edges = pa4_circuit(stages, voltage)
    lines = [f"stage {len(stages)}", f"v1 vdd gnd {voltage}V"]
    # Official inputs have a numeric part 1..999 and one of u,m,K,Meg,G, or none.
    for name, a, b, resistance in edges:
        token = str(resistance)
        for prefix in ("G", "Meg", "K", "", "m", "u"):
            scaled = float(resistance) / PREFIX[prefix]
            if 1 <= scaled <= 999 and abs(scaled - round(scaled)) < 1e-7:
                token = str(round(scaled)) + prefix
                break
        lines.append(f"{name} {a} {b} {token}")
    return "\n".join(lines) + "\nend\n", edges


def modified(stages, operations):
    stages = [(i, list(s), list(p)) for i, s, p in stages]
    merged_names = {}
    for kind, left, right in operations:
        left, right = sorted((left - 1, right - 1))
        if kind == "swap":
            stages[left], stages[right] = stages[right], stages[left]
        else:
            ident, series, shunt = stages[left]
            _, next_s, next_p = stages[right]
            a, b = sum(shunt), sum(next_s) + sum(next_p)
            stages[left] = (ident, series, [a / (1 + a / b)])
            merged_names[ident] = "RP" + str(left + 1)
            stages.pop(right)
    _, edges = pa4_circuit(stages)
    edges = [(merged_names.get(int(name[2:].split("_")[0]), name) if name.startswith("RP") else name, a, b, r)
             for name, a, b, r in edges]
    return stages, edges


def cases():
    public = [
        [(1, [2000, 2000], [2000]), (2, [2000], [3000, 3000])],
        [(1, [2000], [2000]), (2, [2000], [2000] * 4), (3, [2000] * 3, [2000] * 2)],
        [(1, [5000], [5000, 5000]), (2, [2000], [8000, 8000]), (3, [8000], [8000, 8000]),
         (4, [12000], [4000, 4000]), (5, [4000], [2000, 2000])],
    ]
    operations = [[("swap", 1, 2), ("merge", 1, 2)],
                  [("swap", 1, 2), ("swap", 2, 3)],
                  [("merge", 4, 5), ("merge", 3, 4)]]
    output = []
    for index, stages in enumerate(public, 1):
        for changed in (False, True):
            output.append((f"public{index}_{'operations' if changed else 'load'}", stages,
                           6 if index == 2 else 5, operations[index - 1] if changed else [], index))
    output += [
        ("middle_merge", [(1, [2000], [16000]), (2, [8000], [8000]), (3, [4000], [4000])], 6, [("merge", 2, 1)], None),
        ("early_merge_shift", [(i, [i * 1000], [i * 2000]) for i in range(1, 6)], 7, [("merge", 1, 2)], None),
        ("fifteen_stages", [(i, [1000], [100000]) for i in range(1, 16)], 5, [], None),
        ("fifteen_each_path", [(1, [1000] * 15, [2000] * 15)], 5, [], None),
        ("plain_ohms", [(1, [100], [200])], 5, [], None),
        ("micro_ohms", [(1, [1e-6], [2e-6])], 5, [], None),
        ("milli_ohms", [(1, [.001], [.002])], 5, [], None),
        ("meg_ohms", [(1, [1e6], [2e6])], 5, [], None),
        ("giga_ohms_small_current", [(1, [1e9], [999e9])], 5, [], None),
        ("zero_voltage", [(1, [1000], [2000]), (2, [3000], [4000])], 0, [], None),
        ("negative_voltage", [(1, [1000], [2000]), (2, [3000], [4000])], -5, [], None),
        ("maximum_signed_voltage", [(1, [999e9], [1e-6])], 2147483647, [], None),
        ("tiny_attenuation", [(i, [999e9], [1e-6]) for i in range(1, 16)], 5, [], None),
    ]
    rng = random.Random(4012022)
    values = [1e-6, 999e-6, .001, .999, 1, 999, 1000, 999000, 1e6, 999e6, 1e9, 999e9]
    for i in range(36):
        count = rng.randint(1, 6)
        stages = [(j, [rng.choice(values) for _ in range(rng.randint(1, 4))],
                   [rng.choice(values) for _ in range(rng.randint(1, 4))]) for j in range(1, count + 1)]
        output.append((f"random_{i}", stages, rng.choice([0, -11, 1, 5, 100]), [], None))
    return output


class RepairedPA4Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory(dir=TEMP_BASE)
        cls.build = Path(os.environ.get("PA4_TEST_BUILD", cls.temporary.name)).resolve()
        cls.build.mkdir(parents=True, exist_ok=True)
        cls.env = os.environ.copy()
        if Path(CXX).parent != Path("."):
            cls.env["PATH"] = str(Path(CXX).parent) + os.pathsep + cls.env.get("PATH", "")
        cls.executables = {}
        for variant in VARIANTS:
            alias, name = variant.split("/")
            source = ROOT / "assignments/2021/pa04/references" / alias / "fixed" / (name + ".cpp")
            exe = cls.build / (alias + "_" + name + ".exe")
            flags = ["-std=c++11", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-pedantic-errors"]
            if os.name == "nt": flags += ["-static", "-static-libgcc", "-static-libstdc++", "-Wl,--no-insert-timestamp"]
            source_hash = hashlib.sha256(source.read_bytes()).hexdigest()
            provenance = exe.with_suffix(".json")
            if REUSE_CURRENT_BUILD:
                data = json.loads(provenance.read_text())
                if data["source_sha256"] != source_hash or data["exe_sha256"] != hashlib.sha256(exe.read_bytes()).hexdigest() or data["flags"] != flags:
                    raise AssertionError("build provenance differs from current source: " + variant)
            else:
                compiled = subprocess.run([CXX, *flags, str(source), "-o", str(exe)], env=cls.env,
                                          text=True, capture_output=True, timeout=60)
                if compiled.returncode or compiled.stderr:
                    raise AssertionError(variant + " compile:\n" + compiled.stderr)
                provenance.write_text(json.dumps({"source_sha256": source_hash,
                    "exe_sha256": hashlib.sha256(exe.read_bytes()).hexdigest(), "flags": flags}))
            cls.executables[variant] = exe
            REPORT["sources"][variant] = {"sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
                                           "compile_flags": flags, "compile_warnings": 0, "tests": []}

    @classmethod
    def tearDownClass(cls):
        cls.temporary.cleanup()

    def execute(self, variant, stdin, files):
        with tempfile.TemporaryDirectory(dir=TEMP_BASE) as directory:
            work = Path(directory)
            for name, text in files.items(): (work / name).write_text(text, encoding="utf-8")
            run = subprocess.run([str(self.executables[variant])], cwd=work, input=stdin, text=True,
                                 capture_output=True, env=self.env, timeout=4)
            self.assertEqual(run.returncode, 0, variant + run.stderr)
            self.assertEqual(run.stderr, "")
            self.assertLess(len(run.stdout), 1 << 20)
            exported = {name: (work / name).read_text(encoding="utf-8")
                        for name in ("result.txt", "bonus.txt") if (work / name).exists()}
            return run.stdout, exported

    def assert_value(self, value, expected, description):
        actual, quantum, _, _ = value
        # Tolerance follows 3-place engineering rounding, including scientific fallback.
        self.assertLessEqual(abs(actual - expected), quantum + abs(expected) * 2e-14, description)

    def check_case(self, variant, case, directly_export=False):
        name, stages, voltage, operations, public_index = case
        netlist, _ = text_circuit(stages, voltage)
        if public_index:
            netlist = (ROOT / "tests/fixtures/repaired_pa4" / f"input{public_index}.txt").read_text()
        changed, edges = modified(stages, operations)
        port = "n" + str(changed[-1][0])
        expected, volts, currents = pa4_nodal(edges, voltage, port)
        sequence = ["1", "in.txt"]
        for kind, a, b in operations: sequence += ["3" if kind == "swap" else "4", str(a), str(b)]
        if not directly_export: sequence += ["2"]
        sequence += ["5", "result.txt"]
        if variant.endswith("thevenin"): sequence += ["7", "bonus.txt"]
        sequence += ["6"]
        output, files = self.execute(variant, "\n".join(sequence) + "\n", {"in.txt": netlist})
        exported = files["result.txt"]
        got = results(exported)
        wanted = {"vdd": volts["vdd"]}
        wanted.update({"n" + str(ident): volts["n" + str(ident)] for ident, _, _ in changed})
        wanted.update(currents)
        self.assertEqual(set(got), set(wanted), variant + "/" + name)
        for key, value in wanted.items(): self.assert_value(got[key], value, variant + "/" + name + "/" + key)
        if not directly_export: self.assertEqual(results(output), got)
        self.assertNotIn("***calculation results***", output)
        self.assertIn("***calculation results***", exported)
        for line in exported.splitlines():
            if line.startswith("** ") or set(line) == {"*"}: self.assertEqual(len(line), 38, line)
        actual_edges = []
        for line in exported.split("end")[0].splitlines():
            if line.startswith(("RS", "RP")):
                resistor, a, b, token = line.split()
                parsed = number(token)
                actual_edges.append((resistor, a, b, *parsed))
                self.assertNotRegex(token, r"k$")
        self.assertEqual([(a, b, c) for a, b, c, *_ in actual_edges], [(a, b, c) for a, b, c, _ in edges])
        for actual, expected_edge in zip(actual_edges, edges):
            self.assertLessEqual(abs(actual[3] - float(expected_edge[3])), actual[4] + float(expected_edge[3]) * 2e-14)
        if variant.endswith("thevenin"):
            bonus = files["bonus.txt"].splitlines()
            self.assertEqual(bonus[0], "***Thevenin equivalence circuit***")
            for token, value in ((bonus[1].split("=", 1)[1].strip(), expected["rth"]),
                                 (bonus[2].split("=", 1)[1].strip()[:-1], expected["vth"])):
                actual, quantum = number(token)
                self.assertLessEqual(abs(actual - value), quantum + abs(value) * 2e-14)
        REPORT["sources"][variant]["tests"].append({"name": name + ("_direct_export" if directly_export else ""), "passed": True})

    def test_latest_valid_domain(self):
        for variant in VARIANTS:
            for case in cases():
                with self.subTest(variant=variant, case=case[0]): self.check_case(variant, case)

    def test_recalculate_without_menu_calculation(self):
        for variant in VARIANTS:
            for case in cases()[:8]:
                with self.subTest(variant=variant, case=case[0]): self.check_case(variant, case, True)

    def test_file_retry_no_circuit_and_repeated_load(self):
        for variant in VARIANTS:
            menus = ["2", "3", "4", "5"] + (["7"] if variant.endswith("thevenin") else [])
            out, _ = self.execute(variant, "\n".join(menus + ["6"]) + "\n", {})
            self.assertEqual(out.count("There is no circuit"), len(menus))
            netlist, _ = text_circuit([(1, [100], [200])], 5)
            out, files = self.execute(variant, "1\nmissing.txt\nin.txt\n1\nin.txt\n5\nresult.txt\n6\n", {"in.txt": netlist})
            self.assertIn("The file is not found.", out)
            self.assertEqual(out.count("The input file successfully loaded!"), 2)
            self.assertEqual(len(results(files["result.txt"])), 4)
            REPORT["sources"][variant]["tests"].append({"name": "no_circuit_missing_file_repeated_load", "passed": True})

    def test_archived_originals_fail_known_case(self):
        if not REPRODUCE_ORIGINALS:
            self.skipTest("enable --originals for bounded archived-source regression")
        for variant in VARIANTS:
            alias, name = variant.split("/")
            source = ROOT / "assignments/2021/pa04/references" / alias / "original" / (name + ".cpp")
            exe = self.build / (alias + "_original_" + name + ".exe")
            command = [CXX, "-std=c++11", "-O2"]
            if os.name == "nt": command += ["-static", "-static-libgcc", "-static-libstdc++"]
            command += [str(source), "-o", str(exe)]
            compiled = subprocess.run(command, capture_output=True, env=self.env, timeout=60)
            self.assertEqual(compiled.returncode, 0, compiled.stderr.decode("utf-8", "replace"))
            circuit = [(i, [1000], [100000]) for i in range(1, 16)] if alias.startswith("jade") else [(1, [100], [200])]
            netlist, edges = text_circuit(circuit, 5)
            _, volts, currents = pa4_nodal(edges, 5, "n" + str(circuit[-1][0]))
            with tempfile.TemporaryDirectory(dir=TEMP_BASE) as directory:
                work = Path(directory)
                (work / "in.txt").write_text(netlist)
                with (work / "stdout.txt").open("wb") as output:
                    process = subprocess.Popen([str(exe)], cwd=work, stdin=subprocess.PIPE,
                                               stdout=output, stderr=subprocess.DEVNULL, env=self.env)
                    stopped = threading.Event()
                    def cap_output():
                        while not stopped.wait(.01):
                            if (work / "stdout.txt").stat().st_size > (1 << 20): process.kill(); return
                    guard = threading.Thread(target=cap_output, daemon=True); guard.start()
                    timeout = False
                    try: process.communicate(b"1\nin.txt\n2\n6\n", timeout=2)
                    except subprocess.TimeoutExpired: timeout = True; process.kill(); process.communicate()
                    stopped.set(); guard.join()
                text = (work / "stdout.txt").read_bytes()[:1 << 20].decode("utf-8", "replace")
                observed = results(text)
                correct = not timeout and process.returncode == 0
                wanted = {"vdd": volts["vdd"], **{"n" + str(i): volts["n" + str(i)] for i, _, _ in circuit}, **currents}
                correct = correct and set(observed) == set(wanted)
                if correct:
                    correct = all(abs(observed[key][0] - value) <= observed[key][1] + abs(value) * 2e-14 for key, value in wanted.items())
                self.assertFalse(correct, "known archived bug unexpectedly absent: " + variant)
                REPORT["sources"][variant]["original_regression"] = {"sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
                    "case": "fifteen_stages" if alias.startswith("jade") else "plain_ohms",
                    "timeout": timeout, "exit_code": process.returncode, "failed_expected": True}


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=CXX)
    parser.add_argument("--report")
    parser.add_argument("--originals", action="store_true")
    parser.add_argument("--reuse-current-build", action="store_true", help="verify source/executable hashes before reusing this task's freshly compiled build")
    args, unittest_args = parser.parse_known_args()
    CXX = args.cxx
    REPRODUCE_ORIGINALS = args.originals
    REUSE_CURRENT_BUILD = args.reuse_current_build
    suite = unittest.defaultTestLoader.loadTestsFromTestCase(RepairedPA4Tests)
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    REPORT.update(passed=result.wasSuccessful(), test_groups=result.testsRun,
                  failures=len(result.failures), errors=len(result.errors))
    if args.report:
        Path(args.report).write_text(json.dumps(REPORT, indent=2), encoding="utf-8")
    raise SystemExit(0 if result.wasSuccessful() else 1)
