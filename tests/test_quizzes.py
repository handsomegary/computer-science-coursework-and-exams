"""C++11 quiz drivers: independent arithmetic, sorting and trace models."""

import argparse
import math
import os
from pathlib import Path
import random
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
CXX = os.environ.get('CXX', 'g++')
COUNTS = {'executions': 0, 'models': 0}


def comparison_trace(first, second):
    original = 'Original:\n'
    if first > second:
        original += f'{first} is larger than {second}\n'
    original += f'{first} is bigger than {second}\n'
    if second != 0:
        original += f'{second} is equal to {second}\n'
    corrected = 'Corrected:\n'
    if first > second:
        corrected += f'{first} is larger than {second}\n'
    corrected += f'{first} is bigger than {second}\n'
    if first == second:
        corrected += f'{first} is equal to {second}\n'
    return original + corrected


def insertion_trace(values):
    # Each state is independently reconstructed by sorting the processed prefix.
    result = ''
    for size in range(2, len(values) + 1):
        state = sorted(values[:size]) + values[size:]
        result += f'prefix {size}: ' + ' '.join(map(str, state)) + '\n'
    return result + 'sorted: ' + ' '.join(map(str, sorted(values))) + '\n'


def sparse_trace(rows, columns, values):
    zeros = sum(value == 0 for value in values)
    bad = 'sparse' if 2 * zeros >= rows * columns // 2 else 'not sparse'
    good = 'sparse' if zeros > sum(value != 0 for value in values) else 'not sparse'
    return (f'Original: {bad}; reported zeros: {2 * zeros}\n'
            f'Corrected: {good}; zeros: {zeros}\n')


# Every visible question has a scenario. These are anonymous teaching fixtures.
SAMPLES = [
    (1, ['2'], '', 'A: invalid\nB: valid\nC: valid\nD: valid\n', {}),
    (1, ['3'], '9\n', 'A: number2=2\nB: sum=4; number2=2\nC: number2=9\nD: 2; number2=2\n', {}),
    (1, ['4'], '', 'a < b\nc != d\n', {}),
    (1, ['5'], '100 2\n', comparison_trace(100, 2), {}),
    (2, ['2'], '', 'INPUT a, b\nWHILE b is not zero\n    replace (a, b) with (b, a modulo b)\nOUTPUT a\n', {}),
    (2, ['3'], '7\n', 'A: correct\nB: ill-formed in C++11\nC: correct\nD: correct\n', {}),
    (2, ['4'], '', 'B: 7\nC: 4\nA and D: ill-formed\n', {}),
    (2, ['5'], '165 987\n', '3\n', {}),
    (3, ['2'], '', 'floating iterations: 11\ninteger iterations: 10\n', {}),
    (3, ['3'], '', '1 2 3 4 5 6 7 8 9 10 \n', {}),
    (3, ['4'], '', 'C: 0 1 2 3 4 5 6 7 8 9 \nD: 0 1 2 3 4 5 6 7 8 9 \n', {}),
    (3, ['5'], '', '[    7][8]\n1.25 2.50\n', {}),
    (4, ['2'], '', 'A: 1 2 3\nB: 0 1 2\nC: 0 0 0\nD: ill-formed (duplicate enumerator name)\n', {}),
    (4, ['3'], '4\n', '24\n', {}),
    (4, ['4'], '', 'int maximum: 5\ndouble maximum: 5.2\n', {}),
    (4, ['5'], '3 4\n', '7\n', {}),
    (5, ['2'], '', 'A: ill-formed (six initializers for five elements)\nB: 7 0 0 0 0\nC: 0 7 0 3 8 2\nD: 9 1 9 0 0\n', {}),
    (5, ['3'], '5 4 2 4 -1 3\n', insertion_trace([4, 2, 4, -1, 3]), {}),
    (5, ['4'], '2 2 0 1 1 1\n', sparse_trace(2, 2, [0, 1, 1, 1]), {}),
    (5, ['5'], '3 7 -2 5\n', 'explicit size: 3\nsum: 10\n', {}),
    (6, ['2'], '', 'iostream: standard input/output\nfstream: file streams\n', {}),
    (6, ['3', 'sample.dat'], '', 'A: 2\nB: 18\nC: 2\nD: 2\n', {'sample.dat': 'abcdefghij'}),
    (6, ['4'], '-12.5\n', 'atof (double): -12.5\natoi (int): -12\natol (long): -12\n', {}),
    (6, ['5'], '', 'A: 1 2\nB: 1 2\nC (C++11): 1 2\nD: 1 2\n', {}),
]


class QuizTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix='quiz2021_')
        cls.work = Path(cls.temp.name)
        cls.environment = os.environ.copy()
        compiler = Path(CXX)
        if compiler.is_absolute():
            cls.environment['PATH'] = str(compiler.parent) + os.pathsep + cls.environment.get('PATH', '')
        cls.binaries = {}
        for number in range(1, 7):
            source = ROOT / f'exams/2021/quiz{number:02d}/main.cpp'
            executable = cls.work / (f'quiz{number:02d}' + ('.exe' if os.name == 'nt' else ''))
            built = subprocess.run([CXX, '-std=c++11', '-Wall', '-Wextra', '-Wpedantic',
                                    '-Werror', str(source), '-o', str(executable)],
                                   text=True, encoding='utf-8', capture_output=True,
                                   env=cls.environment, timeout=60)
            if built.returncode:
                raise RuntimeError(built.stdout + built.stderr)
            cls.binaries[number] = executable

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_program(self, number, args, stdin='', files=None, code=0):
        for name, content in (files or {}).items():
            (self.work / name).write_bytes(content.encode('utf-8'))
        process = subprocess.run([str(self.binaries[number])] + args, input=stdin,
                                 text=True, encoding='utf-8', capture_output=True,
                                 cwd=self.work, env=self.environment, timeout=10)
        COUNTS['executions'] += 1
        self.assertEqual(process.returncode, code, process.stderr)
        if code == 0:
            self.assertEqual(process.stderr, '')
        for name, content in (files or {}).items():
            self.assertEqual((self.work / name).read_bytes(), content.encode('utf-8'))
        return process.stdout

    def test_all_24_visible_questions(self):
        for number, args, stdin, expected, files in SAMPLES:
            with self.subTest(quiz=number, question=args[0]):
                self.assertEqual(self.run_program(number, args, stdin, files), expected)
                COUNTS['models'] += 1

    def test_quiz1_assignment_trace_and_corrected_comparison(self):
        pairs = [(0, 0), (1, 0), (0, 1), (-1, -1), (2, 100),
                 (-2147483648, 2147483647), (2147483647, -2147483648)]
        for first, second in pairs:
            self.assertEqual(self.run_program(1, ['5'], f'{first} {second}\n'),
                             comparison_trace(first, second))
            COUNTS['models'] += 1

    def test_quiz2_condition_and_euclid_independent_models(self):
        for answer in (-7, 0, 6, 7, 8, 2147483647):
            word = 'correct' if answer == 7 else 'incorrect'
            self.assertEqual(self.run_program(2, ['3'], f'{answer}\n'),
                             f'A: {word}\nB: ill-formed in C++11\nC: {word}\nD: {word}\n')
            COUNTS['models'] += 1
        rng = random.Random(20210205)
        pairs = [(0, 0), (0, 99), (99, 0), (7, 7), (1, 2147483647),
                 (2147483647, 2147483646)]
        pairs += [(rng.randrange(2147483648), rng.randrange(2147483648)) for _ in range(25)]
        for first, second in pairs:
            self.assertEqual(self.run_program(2, ['5'], f'{first} {second}\n'),
                             str(math.gcd(first, second)) + '\n')
            COUNTS['models'] += 1

    def test_quiz4_factorial_domain_and_named_arguments(self):
        for value in range(13):
            self.assertEqual(self.run_program(4, ['3'], f'{value}\n'),
                             str(math.factorial(value)) + '\n')
            COUNTS['models'] += 1
        for first, second in [(-1000000000, -1000000000),
                              (1000000000, 1000000000), (-7, 2), (0, 0)]:
            self.assertEqual(self.run_program(4, ['5'], f'{first} {second}\n'),
                             f'{first + second}\n')
            COUNTS['models'] += 1

    def test_quiz5_insertion_and_explicit_array_length(self):
        rng = random.Random(20210503)
        arrays = [[7], [2, 1], [1, 1, 1], [-1000000, 1000000],
                  list(range(20, 0, -1)), list(range(20))]
        arrays += [[rng.randrange(-100, 101) for _ in range(rng.randrange(1, 21))]
                   for _ in range(15)]
        for values in arrays:
            stdin = f'{len(values)} ' + ' '.join(map(str, values)) + '\n'
            self.assertEqual(self.run_program(5, ['3'], stdin), insertion_trace(values))
            self.assertEqual(self.run_program(5, ['5'], stdin),
                             f'explicit size: {len(values)}\nsum: {sum(values)}\n')
            COUNTS['models'] += 2

    def test_quiz5_sparse_correctness_and_original_bug(self):
        rng = random.Random(20210504)
        matrices = [(1, 1, [0]), (1, 1, [1]), (1, 2, [0, 1]),
                    (2, 2, [0, 0, 1, 1]), (10, 10, [0] * 100),
                    (10, 10, [1] * 100)]
        for _ in range(30):
            rows, columns = rng.randrange(1, 11), rng.randrange(1, 11)
            values = [rng.choice([0, 1, -1, 7]) for _ in range(rows * columns)]
            matrices.append((rows, columns, values))
        for rows, columns, values in matrices:
            stdin = f'{rows} {columns} ' + ' '.join(map(str, values)) + '\n'
            self.assertEqual(self.run_program(5, ['4'], stdin),
                             sparse_trace(rows, columns, values))
            COUNTS['models'] += 1

    def test_quiz6_safe_conversion_and_fixture_unchanged(self):
        for token, floating, integer in [('0', '0', 0), ('3.25', '3.25', 3),
                                         ('-3.25', '-3.25', -3),
                                         ('+1000000', '1e+06', 1000000)]:
            self.assertEqual(self.run_program(6, ['4'], token + '\n'),
                             f'atof (double): {floating}\natoi (int): {integer}\natol (long): {integer}\n')
            COUNTS['models'] += 1
        for content in ('0123456789', '0000000000'):
            self.assertEqual(self.run_program(6, ['3', 'ten.dat'], files={'ten.dat': content}),
                             'A: 2\nB: 18\nC: 2\nD: 2\n')
            COUNTS['models'] += 1

    def test_invalid_demo_inputs(self):
        for number in range(1, 7):
            self.run_program(number, [], code=2)
            self.run_program(number, ['9'], code=2)
        for value in (-1, 13, 2147483647):
            self.run_program(4, ['3'], str(value), code=1)
        for stdin in ('0', '21', '2 1', '1 1000001'):
            self.run_program(5, ['3'], stdin, code=1)
        for stdin in ('0 1', '11 1', '1 1', '1 11'):
            self.run_program(5, ['4'], stdin, code=1)
        for token in ('abc', 'nan', '1e300', '1000001', '999999999999999999e-19'):
            self.run_program(6, ['4'], token, code=1)
        self.run_program(6, ['3', 'short.dat'], files={'short.dat': 'short'}, code=1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=CXX)
    options, unittest_args = parser.parse_known_args()
    CXX = options.cxx
    program = unittest.main(argv=[__file__] + unittest_args, exit=False)
    print('Program executions:', COUNTS['executions'],
          '; independent trace/numeric checks:', COUNTS['models'])
    sys.exit(0 if program.result.wasSuccessful() else 1)
