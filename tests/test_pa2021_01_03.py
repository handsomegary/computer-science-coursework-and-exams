"""Compile PA1--3 independently and verify transcripts with separate models."""

import argparse
import itertools
import os
from pathlib import Path
import random
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
COUNTS = {"executions": 0, "models": 0}
CXX = os.environ.get("CXX", "g++")
PROJECTS = (1, 2, 3)
VALUES = (100, 50, 10, 5, 1)


def payment_model(wallet, need, bonus=False):
    combinations = list(itertools.product(*(range(n + 1) for n in wallet)))
    if bonus:
        feasible = [c for c in combinations
                    if sum(n * d for n, d in zip(c, VALUES)) >= need]
        return min(feasible, key=lambda c: (
            sum(n * d for n, d in zip(c, VALUES)), sum(c),
            tuple(-n for n in c)))
    # Reachable suffix sums enumerate every smaller combination; this model
    # does not assume the implementation's greedy exact-payment decision.
    used, rest = [], need
    for i, (available, value) in enumerate(zip(wallet, VALUES)):
        if rest <= 0:
            used.extend([0] * (5 - i))
            break
        take = min(available, rest // value)
        smaller = {sum(n * d for n, d in zip(c, VALUES[i + 1:]))
                   for c in itertools.product(
                       *(range(n + 1) for n in wallet[i + 1:]))}
        if take < available and rest - take * value not in smaller:
            take += 1
        used.append(take)
        rest -= take * value
    return tuple(used)


def voucher_lines(counts):
    return ''.join(f'    {n} ${value:3d} voucher\n'
                   for n, value in zip(counts, VALUES) if n)


def vouchers_model(transactions, bonus=False):
    wallet = [5] * 5
    output = '*** Welcome to Quintuple Stimulus Vouchers System ***\n'
    stdin = []
    for price, kind, member in transactions:
        stdin.append(str(price))
        output += 'Please enter the good price: '
        if price == -1:
            output += 'Bye~\n'
            break
        stdin.append(kind)
        output += 'Please enter the type of vouchers (d/p): '
        if kind == 'd':
            stdin.append(member)
            output += 'Are you a member of NCU Bank (y/n): '
        rate = 90 if kind == 'p' else 72 if member == 'y' else 80
        need = price * rate // 100
        output += f'The price you need to pay: {need}\n'
        if need > sum(n * d for n, d in zip(wallet, VALUES)):
            output += "You don't have enough voucher to buy the good\n\n"
            continue
        if need:
            used = payment_model(wallet, need, bonus)
            output += 'The payment is processed as follows:\n' + voucher_lines(used)
            wallet = [n - use for n, use in zip(wallet, used)]
            COUNTS['models'] += 1
        if not any(wallet):
            output += 'You do not remain any voucher\nBye~\n'
            break
        output += 'The remaining vouchers are as follows:\n'
        output += voucher_lines(wallet) + '\n'
    return '\n'.join(stdin) + '\n', output


def ordinal(n):
    if 11 <= n % 100 <= 13:
        suffix = 'th'
    else:
        suffix = {1: 'st', 2: 'nd', 3: 'rd'}.get(n % 10, 'th')
    return str(n) + suffix


def signed_binary(n):
    if n >= 0:
        return '0' + format(n, 'b')
    width = (-n).bit_length() + 1
    return format((1 << width) + n, 'b')


def binary_model(tokens, bonus=False):
    out = ('*** Welcome to Binary Adder and Subtractor ***\n' if bonus
           else '*** Welcome to Positive Binary adder ***\n')
    index, partial, error = 1, 0, 0
    for token in tokens:
        if error == 1:
            out += 'The decimal number you entered is out of range. '
        elif error == 2:
            out += 'The partial sum is out of range. '
        out += 'Please enter the ' + ordinal(index)
        out += ' decimal number again: ' if error else ' number in decimal: '
        text = str(token)
        valid = text.lstrip('+-').isascii() and text.lstrip('+-').isdigit()
        valid = valid and text.count('+') + text.count('-') <= 1
        value = int(text) if valid else None
        if valid and value == (0 if bonus else -1):
            out += 'Bye\n'
            break
        if (not valid or not -2147483648 <= value <= 2147483647 or
                not (-262143 <= value <= 524287 if bonus else 0 <= value <= 524287)):
            error = 1
            continue
        if index == 1:
            partial, index, error = value, 2, 0
            continue
        result = partial + value
        if not (-262143 <= result <= 524287 if bonus else 0 <= result <= 524287):
            error = 2
            continue
        representation = signed_binary if bonus else lambda n: format(n, 'b')
        out += (f'Calculating... {representation(partial)} + '
                f'{representation(value)} = {representation(result)}\n'
                f'Partial Sum: {result}\n\n')
        partial, index, error = result, index + 1, 0
        COUNTS['models'] += 1
    return '\n'.join(map(str, tokens)) + '\n', out


PA3_MENU = (
    "----Welcome to big number calculator----\n"
    "Please choose the function\n"
    "1. Addition\n"
    "2. Subtraction\n"
    "3. Multiplication\n"
    "0. Exit\n"
    "--------------------------\n"
)
PA3_BONUS_MENU = PA3_MENU.replace("0. Exit\n", "4. Division\n0. Exit\n")
PA3_HEADINGS = {
    1: "----Now for Addition----\n",
    2: "----Now for Subtraction ----\n",
    3: "----Now for Multiplication----\n",
    4: "----Now for Division----\n",
}
PA3_PROMPT = "Please enter the two numbers.\n"
PA3_ERROR = "Error! Please try again.\n\n"


def pa3_expected_calculation(operation, a, b):
    """Python int supplies an oracle independent of the digit algorithms."""
    a, b = int(a), int(b)
    COUNTS['models'] += 1
    result = PA3_HEADINGS[operation] + PA3_PROMPT
    if operation == 1:
        value = a + b
    elif operation == 2:
        value = a - b
    elif operation == 3:
        value = a * b
    else:
        value, remainder = divmod(a, b)
    result += f"Result: {value}\n"
    if operation == 4:
        result += f"Remainder: {remainder}\n"
    return result + f"Mean: {(a + b) // 2}\n\n"


def pa3_transcript(cases, bonus=False):
    menu = PA3_BONUS_MENU if bonus else PA3_MENU
    stdin = ""
    stdout = ""
    for operation, a, b in cases:
        stdin += f"{operation}\n{a}\n{b}\n"
        stdout += menu + pa3_expected_calculation(operation, a, b)
    return stdin + "0\n", stdout + menu + "Good bye!\n"


class Pa3Tests:
    """Mixin for a TestCase whose run_program(target, stdin) returns stdout."""

    def test_pa03_public_testcases(self):
        samples = [
            [(1, 11112222333344445555, 77776666555544443333),
             (3, 1111111111, 5)],
            [(2, 111111111, 33333333),
             (3, 29292929, 300000003)],
            [(2, 9999999999, 1212121212),
             (3, 29292929, 3)],
        ]
        for i, cases in enumerate(samples, 1):
            with self.subTest(public_testcase=i):
                first, second = cases
                stdin = (f"{first[0]}\n{first[1]}\n{first[2]}\n5\n"
                         f"{second[0]}\n{second[1]}\n{second[2]}\n0\n")
                expected = (
                    PA3_MENU + pa3_expected_calculation(*first)
                    + PA3_MENU + PA3_ERROR
                    + PA3_MENU + pa3_expected_calculation(*second)
                    + PA3_MENU + "Good bye!\n"
                )
                self.assertEqual(self.run_program("pa03_main", stdin), expected)

    def test_pa03_carries_borrows_negative_and_zero_results(self):
        cases = [
            (1, 9, 1), (1, 99999999999999999999, 1),
            (2, 100000000000000000000, 1), (2, 1, 100000000000000000000),
            (2, 123456789, 123456789), (2, 1, 2),
            (1, 1, 2), (3, 9, 9), (3, 1000000000, 1000000000),
        ]
        stdin, expected = pa3_transcript(cases)
        self.assertEqual(self.run_program("pa03_main", stdin), expected)

    def test_pa03_100_digit_boundaries(self):
        maximum = 10 ** 100 - 1
        smallest = 10 ** 99
        cases = [
            (1, maximum, maximum), (1, maximum, 1),
            (2, smallest, maximum), (2, maximum, maximum),
            (3, maximum, maximum), (3, smallest, smallest),
            (3, maximum, 1),
        ]
        stdin, expected = pa3_transcript(cases)
        self.assertEqual(self.run_program("pa03_main", stdin), expected)
        self.assertEqual(len(str(maximum + maximum)), 101)
        self.assertEqual(len(str(maximum * maximum)), 200)

    def test_pa03_random_base_against_python_integers(self):
        rng = random.Random(202103)
        cases = []
        for _ in range(90):
            a = rng.randrange(1, 10 ** rng.randrange(1, 101))
            b = rng.randrange(1, 10 ** rng.randrange(1, 101))
            cases.append((rng.randrange(1, 4), a, b))
        stdin, expected = pa3_transcript(cases)
        self.assertEqual(self.run_program("pa03_main", stdin), expected)

    def test_pa03_bonus_long_division_against_python_integers(self):
        maximum = 10 ** 100 - 1
        cases = [
            (4, 1, 2), (4, 7, 7), (4, 100, 10),
            (4, maximum, 1), (4, maximum, maximum),
            (4, maximum, 97), (4, 10 ** 99, 10 ** 99 + 1),
            (4, 12345678901234567890, 97),
        ]
        rng = random.Random(202104)
        for _ in range(60):
            a = rng.randrange(1, 10 ** rng.randrange(1, 101))
            b = rng.randrange(1, 10 ** rng.randrange(1, 101))
            cases.append((4, a, b))
        cases.extend([(1, maximum, maximum), (2, 1, maximum), (3, maximum, maximum)])
        stdin, expected = pa3_transcript(cases, bonus=True)
        self.assertEqual(self.run_program("pa03_bonus", stdin), expected)

    def test_pa03_invalid_tokens_recovery_and_leading_zeroes(self):
        invalid_pairs = [
            ("0", "1"), ("1", "0"), ("-1", "2"), ("1", "-2"),
            ("+1", "2"), ("1.5", "2"), ("abc", "3"),
            ("9" * 101, "1"), ("1", "0" * 100),
            ("0" * 100 + "1", "2"),
        ]
        for target, bonus in [("pa03_main", False), ("pa03_bonus", True)]:
            with self.subTest(target=target):
                menu = PA3_BONUS_MENU if bonus else PA3_MENU
                stdin, expected = "", ""
                for token in ["5", "-1", "abc", "1x", "1.5", "9" * 200]:
                    stdin += token + "\n"
                    expected += menu + PA3_ERROR
                if not bonus:
                    stdin += "4\n"
                    expected += menu + PA3_ERROR
                for a, b in invalid_pairs:
                    stdin += f"1 {a} {b}\n"
                    expected += menu + PA3_HEADINGS[1] + PA3_PROMPT + PA3_ERROR
                if bonus:
                    for a, b in [("2", "0"), ("0", "2"), ("12", "wrong")]:
                        stdin += f"4 {a} {b}\n"
                        expected += menu + PA3_HEADINGS[4] + PA3_PROMPT + PA3_ERROR
                padded_one = "0" * 99 + "1"
                stdin += f"+0001 {padded_one} 0002\n0\n"
                expected += (menu + pa3_expected_calculation(1, 1, 2)
                             + menu + "Good bye!\n")
                self.assertEqual(self.run_program(target, stdin), expected)

    def test_pa03_eof_ends_without_inventing_input(self):
        for target, menu in [("pa03_main", PA3_MENU), ("pa03_bonus", PA3_BONUS_MENU)]:
            with self.subTest(target=target, position="menu"):
                self.assertEqual(self.run_program(target, ""), menu)
            for stdin in ["1\n", "1\n123\n"]:
                with self.subTest(target=target, stdin=stdin):
                    self.assertEqual(self.run_program(target, stdin),
                                     menu + PA3_HEADINGS[1] + PA3_PROMPT)
            with self.subTest(target=target, position="after_result"):
                expected = menu + pa3_expected_calculation(1, 3, 4) + menu
                self.assertEqual(self.run_program(target, "1 3 4"), expected)


class Pa2021Tests(Pa3Tests, unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix='pa2021_01_03_')
        cls.work = Path(cls.temp.name)
        cls.binaries = {}
        cls.environment = os.environ.copy()
        compiler = Path(CXX)
        if compiler.is_absolute():
            cls.environment['PATH'] = str(compiler.parent) + os.pathsep + cls.environment.get('PATH', '')
        for lab in PROJECTS:
            for variant in ('main', 'bonus'):
                key = f'pa{lab:02d}_{variant}'
                source = ROOT / f'assignments/2021/pa{lab:02d}/{variant}.cpp'
                executable = cls.work / (key + ('.exe' if os.name == 'nt' else ''))
                process = subprocess.run(
                    [CXX, '-std=c++11', '-Wall', '-Wextra', '-Wpedantic', '-Werror',
                     str(source), '-o', str(executable)],
                    capture_output=True, text=True, encoding='utf-8',
                    env=cls.environment, timeout=60)
                if process.returncode:
                    raise RuntimeError(process.stdout + process.stderr)
                cls.binaries[key] = executable

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_program(self, key, stdin):
        process = subprocess.run([str(self.binaries[key])], input=stdin,
                                 text=True, encoding='utf-8', capture_output=True,
                                 cwd=self.work, env=self.environment, timeout=10)
        COUNTS['executions'] += 1
        self.assertEqual(process.returncode, 0, process.stderr)
        self.assertEqual(process.stderr, '')
        return process.stdout

    def check_vouchers(self, transactions, bonus=False):
        stdin, expected = vouchers_model(transactions, bonus)
        key = 'pa01_bonus' if bonus else 'pa01_main'
        self.assertEqual(self.run_program(key, stdin), expected)

    def check_binary(self, tokens, bonus=False):
        stdin, expected = binary_model(tokens, bonus)
        key = 'pa02_bonus' if bonus else 'pa02_main'
        self.assertEqual(self.run_program(key, stdin), expected)

    def test_pa1_public_cases_and_free_purchase(self):
        cases = [
            [(255, 'd', 'y'), (766, 'p', ''), (640, 'p', ''), (40, 'd', 'n'), (-1, '', '')],
            [(1000, 'p', ''), (789, 'd', 'y'), (766, 'p', ''), (289, 'p', ''), (3, 'd', 'n')],
            [(956, 'd', 'y'), (148, 'p', ''), (-1, '', '')],
            [(849, 'd', 'n'), (167, 'p', ''), (2, 'p', '')],
            [(399, 'd', 'n'), (408, 'd', 'y'), (999, 'p', ''), (-1, '', '')],
            [(1, 'd', 'y'), (0, 'p', ''), (-1, '', '')],
            [(2147483647, 'd', 'y'), (923, 'p', '')],
        ]
        for case in cases:
            with self.subTest(case=case):
                self.check_vouchers(case)

    def test_pa1_bonus_minimum_loss(self):
        for case in [
            [(255, 'd', 'y'), (640, 'p', ''), (40, 'd', 'n'), (-1, '', '')],
            [(399, 'd', 'n'), (408, 'd', 'y'), (-1, '', '')],
            [(1, 'd', 'y'), (923, 'p', '')],
        ]:
            self.check_vouchers(case, bonus=True)

    def test_pa1_random_stateful_payments(self):
        rng = random.Random(202101)
        for bonus in (False, True):
            for _ in range(25):
                case = []
                for _ in range(8):
                    case.append((rng.randrange(0, 1200), rng.choice('dp'), rng.choice('yn')))
                case.append((-1, '', ''))
                self.check_vouchers(case, bonus)

    def test_pa1_invalid_tokens_preserve_wallet(self):
        expected_prefix = (
            '*** Welcome to Quintuple Stimulus Vouchers System ***\n'
            'Please enter the good price: Invalid good price. Please try again.\n'
            'Please enter the good price: Invalid good price. Please try again.\n'
            'Please enter the good price: Invalid good price. Please try again.\n')
        _, tail = vouchers_model([(0, 'd', 'y'), (-1, '', '')])
        tail = tail.split('\n', 1)[1]
        tail = tail.replace('Please enter the type of vouchers (d/p): ',
                            'Please enter the type of vouchers (d/p): Invalid voucher type. Please try again.\n'
                            'Please enter the type of vouchers (d/p): ', 1)
        tail = tail.replace('Are you a member of NCU Bank (y/n): ',
                            'Are you a member of NCU Bank (y/n): Invalid member status. Please try again.\n'
                            'Are you a member of NCU Bank (y/n): ', 1)
        stdin = 'x -2 ' + '9' * 200 + ' 0 D d Y y -1'
        for key in ('pa01_main', 'pa01_bonus'):
            self.assertEqual(self.run_program(key, stdin), expected_prefix + tail)

    def test_pa2_public_positive_sequences(self):
        for case in [
            [100, 200, 300, 400, 500, 600, 700, 800, 900, -1],
            [-123, 9999987, 12321, 45654, 1314, 7414, -1],
            [123, 321, 456, 654, -456, -456, 12321456, 4522733, 522733, 78987, 0, -1],
            [0, 0, 123321, 321321, 456654, 123321, 78999874, 123321, 46711, -1],
            [214987654, 123456, 12345, 778665, -1],
        ]:
            self.check_binary(case)

    def test_pa2_signed_sequences_and_limits(self):
        for case in [
            [8, -19, -11, 0], [-1, -3, -123456, -451231, 0],
            [524287, -1, 1, 1, -262144, -262143, 0],
            [-262143, -1, 262143, -1, 1, 0],
            [262143, 262143, -262143, -262143, -1, 0],
            [1, -1, -1, -1, -2, -4, 0],
        ]:
            self.check_binary(case, True)

    def test_pa2_invalid_and_sentinel_at_every_input(self):
        for bonus in (False, True):
            sentinel = 0 if bonus else -1
            for case in [[sentinel], [5, sentinel], ['9' * 200, sentinel],
                         ['--1', '1x', '+', '-2147483648', '+5', 6, sentinel],
                         [5, 6, 'abc', sentinel]]:
                self.check_binary(case, bonus)
        # A huge token at EOF must report error before noticing EOF.
        for key, bonus in [('pa02_main', False), ('pa02_bonus', True)]:
            stdout = self.run_program(key, '9' * 200)
            self.assertIn('The decimal number you entered is out of range.', stdout)

    def test_pa2_random_signed_and_unsigned_models(self):
        rng = random.Random(202102)
        for bonus in (False, True):
            for _ in range(30):
                tokens = [rng.randrange(-262143 if bonus else 0, 524288)
                          for _ in range(12)]
                tokens.append(0 if bonus else -1)
                self.check_binary(tokens, bonus)

    def test_pa2_ordinal_suffixes(self):
        self.check_binary([0] * 23 + [-1])


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=CXX)
    parser.add_argument('--project', choices=('pa01', 'pa02', 'pa03'))
    options, unittest_args = parser.parse_known_args()
    CXX = options.cxx
    default_tests = None
    if options.project:
        PROJECTS = (int(options.project[-2:]),)
        prefix = 'test_pa03_' if options.project == 'pa03' else 'test_pa' + options.project[-1] + '_'
        default_tests = ['Pa2021Tests.' + name for name in dir(Pa2021Tests)
                         if name.startswith(prefix)]
    program = unittest.main(argv=[__file__] + unittest_args,
                            defaultTest=default_tests, exit=False)
    print('Program executions:', COUNTS['executions'],
          '; independent numeric/state checks:', COUNTS['models'])
    sys.exit(0 if program.result.wasSuccessful() else 1)
