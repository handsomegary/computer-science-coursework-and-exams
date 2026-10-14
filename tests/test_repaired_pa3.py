"""Validate repaired anonymous PA3 submissions with Python's integer arithmetic.

Only source files are compiled. Fresh programs run in temporary directories with
timeouts and output limits. Known-bug originals are opt-in, never passing answers.
Example: python tests/test_repaired_pa3.py --cxx g++ --with-originals --report out.json
"""
import argparse
from contextlib import nullcontext
import hashlib
import json
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile
import threading

ROOT = Path(__file__).resolve().parents[1]
REFERENCES = ROOT / 'assignments/2021/pa03/references'
SOURCES = [('moss-lynx-5375', 'basic.cpp', False),
           ('moss-lynx-5375', 'division.cpp', True),
           ('cobalt-heron-2230', 'basic.cpp', False)]
LATEST_SPEC = 'PA3_20211123_2330_Andy.pdf'


def execute(executable, stdin, environment, directory):
    process = subprocess.Popen([str(executable)], cwd=directory, env=environment,
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    outputs = [[], []]
    limits = []

    def read(stream, chunks):
        total = 0
        while True:
            chunk = stream.read(4096)
            if not chunk:
                break
            total += len(chunk)
            if total > 1048576:
                limits.append('output_limit')
                process.kill()
                break
            chunks.append(chunk)

    readers = [threading.Thread(target=read, args=(stream, chunks))
               for stream, chunks in zip([process.stdout, process.stderr], outputs)]
    for reader in readers:
        reader.start()
    try:
        process.stdin.write(stdin.encode('ascii'))
        process.stdin.close()
        process.wait(timeout=3)
    except subprocess.TimeoutExpired:
        limits.append('timeout')
        process.kill()
        process.wait()
    except BrokenPipeError:
        process.wait(timeout=3)
    finally:
        for reader in readers:
            reader.join(timeout=3)
    stdout, stderr = [b''.join(chunks).decode('utf-8', 'replace') for chunks in outputs]
    return {'returncode': process.returncode, 'status': limits[0] if limits else 'completed',
            'stdout': stdout, 'stderr': stderr}


def prior_cases():
    """The same 57 arithmetic cases as the previous independent review."""
    public = [(1, 11112222333344445555, 77776666555544443333),
              (3, 1111111111, 5), (2, 111111111, 33333333),
              (3, 29292929, 300000003), (2, 9999999999, 1212121212),
              (3, 29292929, 3)]
    boundary = [(1, 1, 2), (1, 9, 1), (1, 10**100 - 1, 1),
                (1, 10**100 - 1, 10**100 - 1), (2, 1, 2),
                (2, 100000000000000000000, 1), (2, 123, 123),
                (2, 1, 10**100 - 1), (3, 10**100 - 1, 10**100 - 1),
                (3, 10**99, 10**99), (3, 9, 9)]
    rng = random.Random(3002021)
    random_cases = [(rng.randrange(1, 4),
                     rng.randrange(1, 10**rng.randrange(1, 101)),
                     rng.randrange(1, 10**rng.randrange(1, 101))) for _ in range(40)]
    return [(f'prior_{kind}_{index}', op, str(a), str(b))
            for kind, items in [('public', public), ('boundary', boundary),
                                ('random', random_cases)]
            for index, (op, a, b) in enumerate(items)]


def arithmetic_cases(bonus):
    cases = prior_cases()
    extra = [(1, '11112222333344445555', '55554444333322221111'),
             (3, '111111111', '111111111'),
             (2, '0002', '10'), (2, '0000010', '2'),
             (2, '000123', '00123'), (1, '000001', '000002'),
             (3, '0000009', '0000009'), (2, '10', '1'),
             (2, '123', '124'), (2, '124', '123')]
    extra += [(op, '9' * 100, '9' * 100) for op in (1, 2, 3)]
    extra += [(op, '1' + '0' * 99, '9' * 99) for op in (1, 2, 3)]
    extra += [(op, '9' * 100, '1') for op in (1, 2, 3)]
    extra += [(op, '1', '9' * 100) for op in (1, 2, 3)]
    cases += [(f'extra_{index}', op, a, b) for index, (op, a, b) in enumerate(extra)]
    rng = random.Random(20211123)
    for index in range(600):
        a = rng.randrange(1, 10**rng.randrange(1, 101))
        b = rng.randrange(1, 10**rng.randrange(1, 101))
        cases.append((f'random_base_{index}', rng.randrange(1, 4), str(a), str(b)))
    if bonus:
        division = [(1, 2), (7, 7), (100, 10), (12345, 97),
                    (10**100 - 1, 10**100 - 1), (10**100 - 1, 1),
                    (10**100 - 1, 10**99), (10**99, 10**100 - 1)]
        cases += [(f'prior_division_{index}', 4, str(a), str(b))
                  for index, (a, b) in enumerate(division)]
        cases += [('leading_zero_division', 4, '0000012345', '00000097')]
        for index in range(300):
            a = rng.randrange(1, 10**rng.randrange(1, 101))
            b = rng.randrange(1, 10**rng.randrange(1, 101))
            cases.append((f'random_division_{index}', 4, str(a), str(b)))
    return cases


def check_arithmetic(executable, cases, environment, directory):
    stdin = ''.join(f'{op}\n{a}\n{b}\n' for _, op, a, b in cases) + '0\n'
    run = execute(executable, stdin, environment, directory)
    actual_results = re.findall(r'Result:[ \t]*([^\r\n]*)', run['stdout'])
    actual_means = re.findall(r'Mean:[ \t]*([^\r\n]*)', run['stdout'])
    actual_remainders = re.findall(r'Remainder:[ \t]*([^\r\n]*)', run['stdout'])
    checked = []
    rem_index = 0
    for index, (name, op, a, b) in enumerate(cases):
        x, y = int(a), int(b)
        expected = {1: lambda: x + y, 2: lambda: x - y,
                    3: lambda: x * y, 4: lambda: x // y}[op]()
        got = actual_results[index].strip() if index < len(actual_results) else None
        mean = actual_means[index].strip() if index < len(actual_means) else None
        record = {'name': name, 'scope': 'bonus_division' if op == 4 else 'base_valid',
                  'operation': op, 'a': a, 'b': b, 'expected': str(expected), 'actual': got,
                  'expected_mean': str((x + y) // 2), 'actual_mean': mean,
                  'passed': run['status'] == 'completed' and run['returncode'] == 0
                            and got == str(expected) and mean == str((x + y) // 2)}
        if op == 4:
            rem = actual_remainders[rem_index].strip() if rem_index < len(actual_remainders) else None
            record.update(expected_remainder=str(x % y), actual_remainder=rem)
            record['passed'] = record['passed'] and rem == str(x % y)
            rem_index += 1
        checked.append(record)
    assert len(actual_results) == len(cases), 'Wrong result count or early termination'
    assert len(actual_means) == len(cases), 'Wrong mean count'
    assert len(actual_remainders) == rem_index, 'Wrong remainder count'
    assert 'Good bye!' in run['stdout'] and not run['stderr'], run
    return checked


def check_guards(executable, bonus, environment, directory):
    cases = [('menu_5', '5\n0\n'), ('menu_negative', '-1\n0\n'),
             ('menu_text', 'abc\n0\n'), ('menu_decimal', '1.5\n0\n'),
             ('menu_huge', '9999999999999999999999999\n0\n')]
    cases += [('menu_negative_leading_zero', '-0001\n0\n'),
              ('menu_plus_out_of_range', '+99\n0\n'),
              ('menu_plus_without_digits', '+\n0\n'),
              ('menu_minus_without_digits', '-\n0\n'),
              ('menu_double_plus', '++1\n0\n'),
              ('menu_mixed_signs', '-+1\n0\n')]
    if not bonus:
        cases.append(('menu_division_not_enabled', '4\n0\n'))
    for label, operand in [('negative', '-1'), ('text', 'word'), ('decimal', '1.2'),
                           ('zero', '0'), ('plus', '+1'), ('101digits', '1' * 101),
                           ('all_zeroes', '0000')]:
        cases.append(('invalid_' + label, f'1\n{operand}\n2\n0\n'))
        if bonus:
            cases.append(('division_invalid_' + label, f'4\n2\n{operand}\n0\n'))
    checked = []
    for name, stdin in cases:
        run = execute(executable, stdin, environment, directory)
        checked.append({'name': name, 'scope': 'selection_guard' if name.startswith('menu') else 'invalid_operand_guard',
                        'passed': run['status'] == 'completed' and run['returncode'] == 0
                                  and run['stdout'].count('Error! Please try again.') == 1
                                  and 'Good bye!' in run['stdout']
                                  and 'Result:' not in run['stdout'] and not run['stderr']})
    for name, stdin in [('eof_menu', ''), ('eof_operand', '1\n2\n')]:
        run = execute(executable, stdin, environment, directory)
        checked.append({'name': name, 'scope': 'eof_robustness',
                        'passed': run['status'] == 'completed' and run['returncode'] == 0
                                  and 'Result:' not in run['stdout'] and not run['stderr']})
    for spelling, op in [('+1', 1), ('01', 1), ('0001', 1),
                         ('+2', 2), ('02', 2), ('+3', 3), ('03', 3)] + ([('+4', 4), ('04', 4)] if bonus else []):
        run = execute(executable, f'{spelling}\n7\n2\n0\n', environment, directory)
        results = re.findall(r'Result:[ \t]*([^\r\n]*)', run['stdout'])
        means = re.findall(r'Mean:[ \t]*([^\r\n]*)', run['stdout'])
        expected = {1: 9, 2: 5, 3: 14, 4: 3}[op]
        passed = run['status'] == 'completed' and run['returncode'] == 0 and results == [str(expected)] and means == ['4'] and 'Good bye!' in run['stdout'] and 'Error!' not in run['stdout'] and not run['stderr']
        if op == 4:
            passed = passed and re.findall(r'Remainder:[ \t]*([^\r\n]*)', run['stdout']) == ['1']
        checked.append({'name': 'integer_menu_' + spelling, 'scope': 'valid_integer_selection', 'passed': passed})
    for spelling in ('+0', '-0', '0000', '-0000', '0' * 80):
        run = execute(executable, spelling + '\n', environment, directory)
        checked.append({'name': 'integer_zero_menu_' + spelling, 'scope': 'valid_integer_selection',
                        'passed': run['status'] == 'completed' and run['returncode'] == 0
                                  and 'Good bye!' in run['stdout'] and 'Result:' not in run['stdout']
                                  and 'Error!' not in run['stdout'] and not run['stderr']})
    return checked


def original_witnesses(executable, alias, bonus, environment, directory):
    if alias == 'cobalt-heron-2230':
        cases = [('negative_subtraction', 2, '1', '2'),
                 ('addition_101digits', 1, '9' * 100, '1'),
                 ('mean_carry', 1, '9' * 100, '9' * 100),
                 ('multiplication_200digits', 3, '9' * 100, '9' * 100)]
    else:
        cases = [('negative_subtraction', 2, '1', '2')]
        if bonus:
            cases += [('equal_subtraction', 2, '123', '123'),
                      ('single_digit_division', 4, '1', '2'),
                      ('equal_division', 4, '7', '7'),
                      ('huge_division_by_one', 4, '9' * 100, '1')]
    checked = []
    for name, op, a, b in cases:
        run = execute(executable, f'{op}\n{a}\n{b}\n0\n', environment, directory)
        results = re.findall(r'Result:[ \t]*([^\r\n]*)', run['stdout'])
        means = re.findall(r'Mean:[ \t]*([^\r\n]*)', run['stdout'])
        rems = re.findall(r'Remainder:[ \t]*([^\r\n]*)', run['stdout'])
        x, y = int(a), int(b)
        expected = {1: lambda: x + y, 2: lambda: x - y,
                    3: lambda: x * y, 4: lambda: x // y}[op]()
        actual = results[0].strip() if results else None
        mean = means[0].strip() if means else None
        passed = run['status'] == 'completed' and run['returncode'] == 0 and actual == str(expected) and mean == str((x + y) // 2)
        record = {'name': name, 'operation': op, 'a': a, 'b': b,
                  'expected': str(expected), 'actual': actual,
                  'expected_mean': str((x + y) // 2), 'actual_mean': mean,
                  'status': run['status'], 'returncode': run['returncode'], 'passed': passed}
        if op == 4:
            rem = rems[0].strip() if rems else None
            record.update(expected_remainder=str(x % y), actual_remainder=rem)
            record['passed'] = passed and rem == str(x % y)
        checked.append(record)
    assert any(not record['passed'] for record in checked), 'Expected original defect did not reproduce'
    return checked


def check(compiler, with_originals=False, work_directory=None, report_path=None, only=None,
          compile_only=False, reuse_build=False):
    environment = os.environ.copy()
    if Path(compiler).parent != Path('.'):
        environment['PATH'] = str(Path(compiler).parent) + os.pathsep + environment.get('PATH', '')
    report = {'latest_spec': LATEST_SPEC, 'standard': 'C++11',
              'oracle': 'Python arbitrary-precision integer arithmetic; exact decimal strings, floor mean, divmod.',
              'source_review': 'Console I/O only, no system/process/network calls or downloaded binaries.',
              'sources': {}}
    def save_progress():
        if report_path:
            report_path.parent.mkdir(parents=True, exist_ok=True)
            report_path.write_text(json.dumps(report, indent=2), encoding='utf-8')
    if work_directory:
        work_directory = work_directory.resolve()
        work_directory.mkdir(parents=True, exist_ok=True)
    durable = compile_only or reuse_build
    if durable and not work_directory:
        raise ValueError('--work-dir is required for --compile-only and --reuse-build')
    context = nullcontext(str(work_directory)) if durable else tempfile.TemporaryDirectory(prefix='anonymous-pa3-', dir=work_directory)
    with context as temporary:
        directory = Path(temporary)
        manifest_path = directory / 'build_manifest.json'
        manifest = json.loads(manifest_path.read_text(encoding='utf-8')) if manifest_path.exists() else {}

        def build(alias, filename, mode, source):
            basename = alias + '-' + filename.replace('.cpp', '') + '-' + mode
            executable = directory / (basename + ('.exe' if os.name == 'nt' else ''))
            command = [compiler, '-std=c++11', '-Wall', '-Wextra', '-Wpedantic',
                       '-O2' if mode == 'optimized' else '-O0',
                       '-static-libgcc', '-static-libstdc++']
            if mode != 'original':
                command.append('-D_GLIBCXX_ASSERTIONS')
            if os.name == 'nt':
                command += ['-static', '-Wl,--no-insert-timestamp']
            command += [str(source), '-o', str(executable)]
            source_hash = hashlib.sha256(source.read_bytes()).hexdigest()
            if reuse_build:
                saved = manifest.get(basename)
                assert saved, f'Missing build manifest for {basename}; run --compile-only first'
                assert saved['source_sha256'] == source_hash, f'Source changed after compile: {basename}'
                assert executable.exists() and saved['executable_sha256'] == hashlib.sha256(executable.read_bytes()).hexdigest(), f'Executable changed after compile: {basename}'
                assert saved['compiler_arguments'] == command[1:-3], f'Compiler settings changed: {basename}'
                diagnostics = saved['compile_warnings']
            else:
                result = subprocess.run(command, capture_output=True, env=environment, timeout=60)
                assert result.returncode == 0, result.stderr.decode('utf-8', 'replace')
                diagnostics = result.stderr.decode('utf-8', 'replace')
                if durable:
                    manifest[basename] = {'source_sha256': source_hash,
                                          'executable_sha256': hashlib.sha256(executable.read_bytes()).hexdigest(),
                                          'compiler_arguments': command[1:-3],
                                          'compile_warnings': diagnostics}
                    manifest_path.write_text(json.dumps(manifest, indent=2), encoding='utf-8')
            return executable, command[1:-3], diagnostics

        for alias, filename, bonus in SOURCES:
            key = alias + '/' + filename
            if only and key != only:
                continue
            entry = {'fixed_builds': []}
            report['sources'][key] = entry
            for mode, level in [('debug', '-O0'), ('optimized', '-O2')]:
                source = REFERENCES / alias / 'fixed' / filename
                executable, arguments, diagnostics = build(alias, filename, mode, source)
                if compile_only:
                    print(f'{key} {mode}: compiled', flush=True)
                    continue
                cases = check_arithmetic(executable, arithmetic_cases(bonus), environment, directory)
                cases += check_guards(executable, bonus, environment, directory)
                record = {'mode': mode, 'compiler_arguments': arguments,
                         'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
                         'compile_warnings': diagnostics,
                         'passed': sum(record['passed'] for record in cases), 'tests': len(cases),
                         'cases': cases}
                entry['fixed_builds'].append(record)
                save_progress()
                failed = [case for case in cases if not case['passed']]
                assert not failed, f'{key} {mode}: {failed[:5]}'
                print(f'{key} {mode}: {len(cases)}/{len(cases)} passed', flush=True)
            if with_originals:
                source = REFERENCES / alias / 'original' / filename
                executable, _, _ = build(alias, filename, 'original', source)
                if compile_only:
                    print(f'{key} original: compiled', flush=True)
                    continue
                cases = original_witnesses(executable, alias, bonus, environment, directory)
                entry['original'] = {'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
                                     'status': 'known failing archive; not a passing solution',
                                     'cases': cases}
                save_progress()
                print(f'{key} original: {sum(not c["passed"] for c in cases)}/{len(cases)} defect witnesses reproduced', flush=True)
    return report


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx', default='g++')
    parser.add_argument('--with-originals', action='store_true')
    parser.add_argument('--report', type=Path)
    parser.add_argument('--work-dir', type=Path, help='Parent directory for temporary compiled programs')
    parser.add_argument('--only', choices=[a + '/' + f for a, f, _ in SOURCES])
    build_modes = parser.add_mutually_exclusive_group()
    build_modes.add_argument('--compile-only', action='store_true', help='Build reviewed sources and save their hashes for a later tool invocation')
    build_modes.add_argument('--reuse-build', action='store_true', help='Run only binaries matching a prior --compile-only manifest and current source hashes')
    args = parser.parse_args()
    result = check(args.cxx, args.with_originals, args.work_dir, args.report, args.only,
                   args.compile_only, args.reuse_build)
    if args.report and not args.compile_only:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(result, indent=2), encoding='utf-8')
