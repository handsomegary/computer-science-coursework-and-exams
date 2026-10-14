"""Validate the repaired anonymous DFF synthesis reference by truth-table simulation."""
import argparse
import hashlib
import itertools
import json
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile

from test_pa2021_04_05_final import PUBLIC_TABLE1, PUBLIC_TABLE2, evaluate_bench, state_table

ROOT = Path(__file__).resolve().parents[1]
REFERENCE = ROOT / 'assignments/2021/final/references/jade-lynx-c0ee'
COURSE = ROOT / 'assignments/2021/final/course_support/kmap.cpp'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def cases(exhaustive=False):
    result = []
    rows = [([a, b], [x], [int(x and (a or b)), int(x and (a or not b))], [int(a and b)])
            for a, b, x in itertools.product((0, 1), repeat=3)]
    result.append(('public1', PUBLIC_TABLE1, ['A', 'B'], ['x'], ['y'], rows))
    rows = [([a], [x, y], [int(a or x and y)], []) for a, x, y in itertools.product((0, 1), repeat=3)]
    result.append(('public2', PUBLIC_TABLE2, ['A'], ['x', 'y'], [], rows))
    rng = random.Random(202110)
    for width in (2, 3, 4):
        for index in range(8):
            present, inputs, outputs = ['A'], ['x', 'y', 'z'][:width - 1], ['o', 'p']
            rows = [([bits[0]], list(bits[1:]), [rng.randrange(2)], [rng.randrange(2), rng.randrange(2)])
                    for bits in itertools.product((0, 1), repeat=width)]
            result.append((f'random_{width}_{index}', state_table('D', present, inputs, outputs, rows),
                           present, inputs, outputs, rows))
    for bit in (0, 1):
        rows = [([a], [x], [bit], [bit]) for a, x in itertools.product((0, 1), repeat=2)]
        result.append((f'constant_{bit}', state_table('D', ['A'], ['x'], ['o'], rows), ['A'], ['x'], ['o'], rows))
    for mask in range(4):
        rows = [([q], [], [(mask >> q) & 1], [1, 0]) for q in (0, 1)]
        result.append((f'one_variable_{mask}', state_table('D', ['Memory'], [], ['yes', 'no'], rows),
                       ['Memory'], [], ['yes', 'no'], rows))
    rows = [([], [], [], [1, 0])]
    result.append(('zero_variables', state_table('D', [], [], ['yes', 'no'], rows), [], [], ['yes', 'no'], rows))
    for width in (2, 3, 4):
        masks = range(1 << (1 << width)) if width < 4 or exhaustive else [0, 65535, *[1 << i for i in range(16)]]
        outputs = ['truth' + str(mask) for mask in masks]
        rows = [([bits[0]], list(bits[1:]), [bits[0]], [(mask >> value) & 1 for mask in masks])
                for value, bits in enumerate(itertools.product((0, 1), repeat=width))]
        inputs = ['DN1', 'control', 'N2'][:width - 1]
        result.append((f'all_functions_{width}', state_table('D', ['N1'], inputs, outputs, rows),
                       ['N1'], inputs, outputs, rows))
    rng = random.Random(87213)
    for width in range(1, 5):
        for index in range(20):
            present = ['Memory'] + (['N1'] if width > 2 else [])
            inputs = ['DMemory', 'control', 'N2'][:width - len(present)]
            outputs = ['data', 'zero', 'one']
            rows = [(list(bits[:len(present)]), list(bits[len(present):]),
                     [rng.randrange(2) for _ in present], [rng.randrange(2), 0, 1])
                    for bits in itertools.product((0, 1), repeat=width)]
            rng.shuffle(rows)
            table = state_table('D', present, inputs, outputs, rows, list(reversed(present)),
                                ['Output', 'Input', 'Next', 'Present'])
            result.append((f'renamed_shuffled_{width}_{index}', table, present, inputs, outputs, rows))
    rows = [([], [x], [], []) for x in (0, 1)]
    result.append(('no_functions', state_table('D', [], ['x'], [], rows), [], ['x'], [], rows))
    return result


def build(compiler, directory, with_originals=False):
    directory.mkdir(parents=True, exist_ok=True)
    manifest = {}
    targets = [('fixed', 'debug', '-O0'), ('fixed', 'optimized', '-O2')]
    if with_originals:
        targets += [('original', 'original', '-O0')]
    for folder, label, level in targets:
        source = REFERENCE / folder / 'basic.cpp'
        executable = directory / (label + ('.exe' if os.name == 'nt' else ''))
        flags = ['-std=c++11', level, '-Wall', '-Wextra', '-Wpedantic', '-D_GLIBCXX_ASSERTIONS']
        if folder == 'fixed':
            flags += ['-Werror', '-pedantic-errors']
        if os.name == 'nt':
            flags += ['-static', '-static-libgcc', '-static-libstdc++', '-Wl,--no-insert-timestamp']
        compiled = subprocess.run([compiler, *flags, str(source), '-o', str(executable)],
                                  capture_output=True, text=True, timeout=60)
        assert compiled.returncode == 0, compiled.stderr
        manifest[label] = {'source': source.relative_to(ROOT).as_posix(), 'source_sha256': sha(source),
                           'wrapper_sha256': sha(source.with_name('kmap.cpp')), 'course_sha256': sha(COURSE),
                           'binary': executable.name, 'binary_sha256': sha(executable), 'flags': flags}
    (directory / 'build_manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    return manifest


def run_case(executable, record, environment):
    label, table, present, inputs, outputs, rows = record
    with tempfile.TemporaryDirectory(prefix='repaired-final-case-') as temporary:
        directory = Path(temporary)
        (directory / 'states.txt').write_text(table, encoding='utf-8')
        process = subprocess.run([str(executable), 'states.txt', 'circuit.bench'], cwd=directory,
                                 env=environment, capture_output=True, text=True, timeout=30)
        if process.returncode != 0 or process.stdout or process.stderr:
            return {'case': label, 'passed': False, 'reason': 'execution failed', 'returncode': process.returncode}
        path = directory / 'circuit.bench'
        if not path.is_file():
            return {'case': label, 'passed': False, 'reason': 'missing circuit'}
        bench = path.read_text(encoding='utf-8')
        if re.findall(r'^INPUT\(([^()]+)\)$', bench, re.M) != inputs or re.findall(r'^OUTPUT\(([^()]+)\)$', bench, re.M) != outputs:
            return {'case': label, 'passed': False, 'reason': 'port declarations mismatch'}
        driven = re.findall(r'^([^\s]+) = [A-Z]+\(', bench, re.M)
        if len(driven) != len(set(driven)):
            return {'case': label, 'passed': False, 'reason': 'duplicate driven net'}
        if any(wire in inputs for wire in driven):
            return {'case': label, 'passed': False, 'reason': 'input signal driven by a gate'}
        for current, values, future, observed in rows:
            try:
                got_next, got_outputs = evaluate_bench(bench, dict(zip(present, current)), dict(zip(inputs, values)))
                if got_next != dict(zip(present, map(bool, future))) or got_outputs != dict(zip(outputs, map(bool, observed))):
                    return {'case': label, 'passed': False, 'reason': 'truth mismatch'}
            except Exception as error:
                return {'case': label, 'passed': False, 'reason': str(error)}
        return {'case': label, 'passed': True, 'rows': len(rows), 'output_functions': len(outputs)}


def invalid_cases(executable, environment):
    records = []
    rows = [([a], [x], [a ^ x], [a]) for a, x in itertools.product((0, 1), repeat=2)]
    valid = state_table('D', ['A'], ['x'], ['o'], rows)
    scenarios = [('missing_args', [], valid), ('same_file_alias', ['states.txt', './states.txt'], valid),
                 ('missing_input', ['missing.txt', 'circuit.bench'], valid),
                 ('bad_table', ['states.txt', 'circuit.bench'], valid.replace('//Next(A)', '//Next(B)')),
                 ('reserved_clock', ['states.txt', 'circuit.bench'], valid.replace('//Input(x)', '//Input(CLK)')),
                 ('scratch_input', ['kmap_in.txt', 'circuit.bench'], valid),
                 ('scratch_output_input', ['kmap_out.txt', 'circuit.bench'], valid),
                 ('scratch_destination', ['states.txt', 'kmap_out.txt'], valid),
                 ('scratch_unwritable', ['states.txt', 'circuit.bench'], valid),
                 ('hardlink_destination', ['states.txt', 'alias.txt'], valid)]
    for label, argv, table in scenarios:
        with tempfile.TemporaryDirectory(prefix='repaired-final-invalid-') as temporary:
            directory = Path(temporary)
            for filename in ('states.txt', 'kmap_in.txt', 'kmap_out.txt'):
                (directory / filename).write_text(table, encoding='utf-8')
            sentinel = b'existing circuit must be preserved\n'
            (directory / 'circuit.bench').write_bytes(sentinel)
            if label == 'scratch_unwritable':
                (directory / 'kmap_out.txt').unlink()
                (directory / 'kmap_out.txt').mkdir()
            if label == 'hardlink_destination':
                os.link(directory / 'states.txt', directory / 'alias.txt')
            before = {path.name: path.read_bytes() for path in directory.iterdir() if path.is_file()}
            run = subprocess.run([str(executable), *argv], cwd=directory, env=environment,
                                 capture_output=True, timeout=5)
            passed = run.returncode == 1 and bool(run.stderr) and not run.stdout
            passed = passed and all((directory / name).read_bytes() == value for name, value in before.items())
            records.append({'case': label, 'passed': passed})
    return records


def check(options):
    directory = options.work_dir.resolve()
    if options.reuse_build:
        manifest = json.loads((directory / 'build_manifest.json').read_text(encoding='utf-8'))
    else:
        manifest = build(options.cxx, directory, options.with_originals)
    if not options.with_originals:
        manifest = {label: record for label, record in manifest.items() if label != 'original'}
    if options.compile_only:
        print('Repaired Final: compiled ' + ', '.join(manifest))
        return
    environment = os.environ.copy()
    if Path(options.cxx).parent != Path('.'):
        environment['PATH'] = str(Path(options.cxx).parent) + os.pathsep + environment.get('PATH', '')
    report = {}
    suite = cases(options.exhaustive)
    for label, record in manifest.items():
        source = ROOT / record['source']
        executable = directory / record['binary']
        assert sha(source) == record['source_sha256'] and sha(executable) == record['binary_sha256']
        assert sha(COURSE) == record['course_sha256'] and sha(source.with_name('kmap.cpp')) == record['wrapper_sha256']
        selected = suite[:28] if label == 'original' else suite
        results = [run_case(executable, case, environment) for case in selected]
        if label != 'original':
            results += invalid_cases(executable, environment)
        report[label] = {'source': record['source'], 'sha256': record['source_sha256'], 'cases': len(results),
                         'passed': sum(case['passed'] for case in results), 'results': results}
        if options.report:
            options.report.parent.mkdir(parents=True, exist_ok=True)
            options.report.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
        print(f'Repaired Final {label}: {report[label]["passed"]}/{len(results)} cases passed', flush=True)
        if label != 'original':
            assert all(case['passed'] for case in results), [case for case in results if not case['passed']]


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=os.environ.get('CXX', 'g++'))
    parser.add_argument('--work-dir', type=Path, default=ROOT.parent / 'repair_final/build')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--compile-only', action='store_true')
    parser.add_argument('--reuse-build', action='store_true')
    parser.add_argument('--with-originals', action='store_true')
    parser.add_argument('--exhaustive', action='store_true', help='All 65,536 four-variable output functions')
    check(parser.parse_args())
