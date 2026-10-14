"""Recheck anonymous PA1 references against transcripts and an independent wallet oracle."""
import argparse
import hashlib
import itertools
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import threading

ROOT = Path(__file__).resolve().parents[1]
VALUES = (100, 50, 10, 5, 1)


def execute(executable, stdin, environment, directory):
    process = subprocess.Popen([str(executable)], cwd=directory, env=environment,
                               stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    outputs = [[], []]
    limit_reached = []

    def read(stream, chunks):
        size = 0
        while True:
            chunk = stream.read(4096)
            if not chunk:
                break
            size += len(chunk)
            if size > 65536:
                limit_reached.append(True)
                process.kill()
                break
            chunks.append(chunk)

    readers = [threading.Thread(target=read, args=(stream, chunks))
               for stream, chunks in zip([process.stdout, process.stderr], outputs)]
    for reader in readers:
        reader.start()
    try:
        process.stdin.write(stdin.encode('utf-8'))
        process.stdin.close()
        process.wait(timeout=3)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()
        raise AssertionError('Reference program exceeded timeout')
    except BrokenPipeError:
        process.wait(timeout=3)
    finally:
        for reader in readers:
            reader.join(timeout=3)
    assert process.returncode == 0 and not limit_reached
    stdout, stderr = [b''.join(chunks).decode('utf-8') for chunks in outputs]
    assert not stderr, stderr
    return stdout


def vouchers(text):
    result = [0] * 5
    for count, value in re.findall(r'(\d+)\s*\$\s*(100|50|10|5|1)\s+voucher', text):
        result[VALUES.index(int(value))] += int(count)
    return result


def check_minimum_loss(stdout, transactions):
    blocks = re.split(r'The price you need to pay:\s*(\d+)\r?\n', stdout)
    count = (len(blocks) - 1) // 2
    assert 0 < count <= len(transactions), 'Missing or extra transaction output'
    wallet = [5] * 5
    for i in range(count):
        price, kind, member = transactions[i]
        rate = 90 if kind == 'p' else 72 if member == 'y' else 80
        need = price * rate // 100
        assert int(blocks[2 * i + 1]) == need
        block = blocks[2 * i + 2]
        if need > sum(n * value for n, value in zip(wallet, VALUES)):
            assert "You don't have enough voucher" in block
            assert 'The payment is processed as follows:' not in block
            assert 'You do not remain any voucher' not in block
            if 'The remaining vouchers are as follows:' in block:
                assert vouchers(block.split('The remaining vouchers are as follows:', 1)[1]) == wallet
            continue
        assert 'The payment is processed as follows:' in block
        receipt = block.split('The payment is processed as follows:', 1)[1]
        receipt = receipt.split('The remaining vouchers', 1)[0].split('You do not remain', 1)[0]
        paid = vouchers(receipt)
        assert all(0 <= used <= available for used, available in zip(paid, wallet))
        feasible_values = (sum(n * value for n, value in zip(choice, VALUES))
                           for choice in itertools.product(*(range(n + 1) for n in wallet)))
        optimum = min(value for value in feasible_values if value >= need)
        assert sum(n * value for n, value in zip(paid, VALUES)) == optimum
        wallet = [available - used for available, used in zip(wallet, paid)]
        if 'The remaining vouchers are as follows:' in block:
            assert vouchers(block.split('The remaining vouchers are as follows:', 1)[1]) == wallet
        else:
            assert wallet == [0] * 5 and 'You do not remain any voucher' in block
    assert count == len(transactions) or wallet == [0] * 5, 'Program skipped a valid transaction'
    return count


def check(compiler):
    fixtures = json.loads((ROOT / 'tests/fixtures/reference_pa1_cases.json').read_text(encoding='utf-8'))
    environment = os.environ.copy()
    if Path(compiler).parent != Path('.'):
        environment['PATH'] = str(Path(compiler).parent) + os.pathsep + environment.get('PATH', '')
    processed = 0
    with tempfile.TemporaryDirectory(prefix='anonymous-pa1-') as temporary:
        directory = Path(temporary)
        for role, record in fixtures.items():
            source = ROOT / record['source']
            assert hashlib.sha256(source.read_bytes()).hexdigest() == record['sha256']
            executable = directory / (role + ('.exe' if os.name == 'nt' else ''))
            flags = ['-std=c++11', '-O0', '-Wall', '-Wextra', '-Wpedantic', '-Wconversion']
            if os.name == 'nt':
                flags += ['-static', '-static-libgcc', '-static-libstdc++']
            compilation = subprocess.run([compiler, *flags, str(source), '-o', str(executable)],
                                         env=environment, capture_output=True, text=True, timeout=60)
            assert compilation.returncode == 0, compilation.stderr
            for case in record['cases']:
                stdout = execute(executable, case['input'], environment, directory)
                if role == 'regular':
                    assert re.sub(r'\s+', '', stdout) == re.sub(r'\s+', '', case['expected_stdout']), case['label']
                else:
                    processed += check_minimum_loss(stdout, case['transactions'])
            print(f'Anonymous PA1 {role}: {len(record["cases"])} cases passed')
    print(f'Minimum-loss transaction outputs checked: {processed}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=os.environ.get('CXX', 'g++'))
    check(parser.parse_args().cxx)
