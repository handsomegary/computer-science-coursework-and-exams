"""Verify preserved official downloads and run public fixtures with portable oracles.

The supplied Linux checker binaries are never executed by this script.
"""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import re
import subprocess
import tempfile
import zipfile

from test_pa2025 import csv_values, fp8, knight_oracle, validate_knight

ROOT = Path(__file__).resolve().parents[1]
ASSIGNMENTS = ROOT / 'assignments/2025'
PROGRAMS = {
    'dinosaur': ['pa01/main.cpp'],
    'knight': ['pa02/main.cpp'],
    'cnn': ['final/main.cpp', 'final/hardware_architecture.cpp'],
    'preprocess': ['final/bonus.cpp'],
}
HEADERS = {
    'cnn': ['final/hardware_architecture.h', 'final/quantization.h'],
    'preprocess': ['final/quantization.h'],
}


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def resource_checks():
    manifest = json.loads((ASSIGNMENTS / 'resources.json').read_text(encoding='utf-8'))
    count = 0
    for item in manifest['standalone'] + manifest['generated_support_files']:
        path = ROOT / item['path']
        assert path.is_file() and len(path.read_bytes()) == item['size'] and sha(path) == item['sha256'], item['path']
        count += 1
    for archive in manifest['archives']:
        path = ROOT / archive['path']
        assert path.is_file() and sha(path) == archive['sha256'] and path.stat().st_size == archive['size']
        with zipfile.ZipFile(path) as package:
            assert package.testzip() is None
            for member in archive['members']:
                destination = ROOT / member['path']
                if member.get('directory'):
                    assert destination.is_dir(), member['path']
                else:
                    assert destination.read_bytes() == package.read(member['source_member']), member['path']
                    assert destination.stat().st_size == member['size'] and sha(destination) == member['sha256']
                    count += 1
    for path in [ASSIGNMENTS / 'pa01/course_support/checker.out', ASSIGNMENTS / 'pa02/course_support/starter/checker']:
        assert path.read_bytes()[:6] == b'\x7fELF\x02\x01', 'Official checker must remain its original Linux ELF file.'
        assert path.read_bytes()[18:20] == b'\x3e\x00', 'Official checker architecture is x86-64.'
    return {'archives': len(manifest['archives']), 'preserved_files_checked': count, 'checker_binaries_executed': False}


def binary(directory, name):
    return directory / (name + ('.exe' if os.name == 'nt' else ''))


def source_hashes(name):
    return {p: sha(ASSIGNMENTS / p) for p in PROGRAMS[name] + HEADERS.get(name, [])}


def build(cxx, directory):
    directory.mkdir(parents=True, exist_ok=True)
    flags = ['-std=c++11', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-Werror']
    if os.name == 'nt':
        flags += ['-static', '-static-libgcc', '-static-libstdc++', '-Wl,--no-insert-timestamp']
    proof = {}
    for name, paths in PROGRAMS.items():
        compiled = subprocess.run([cxx, *flags, *(str(ASSIGNMENTS / p) for p in paths), '-o', str(binary(directory, name))], capture_output=True, text=True, timeout=60)
        assert compiled.returncode == 0 and not compiled.stderr, compiled.stderr
        proof[name] = {'source_sha256': source_hashes(name), 'exe_sha256': sha(binary(directory, name)), 'flags': flags}
    (directory / 'official_build_manifest.json').write_text(json.dumps(proof, indent=2) + '\n', encoding='utf-8')


def verify_build(directory):
    proof = json.loads((directory / 'official_build_manifest.json').read_text(encoding='utf-8'))
    for name in PROGRAMS:
        assert proof[name]['source_sha256'] == source_hashes(name), name + ': source changed since compilation'
        assert proof[name]['exe_sha256'] == sha(binary(directory, name)), name + ': executable changed since compilation'


def run(executable, text='', args=(), cwd=None, timeout=120):
    result = subprocess.run([str(executable), *map(str, args)], input=text, text=True, encoding='utf-8', capture_output=True, cwd=cwd, timeout=timeout)
    assert result.returncode == 0 and not result.stderr, (result.returncode, result.stderr)
    return result.stdout


def dinosaur_checks(executable):
    base = ASSIGNMENTS / 'pa01/course_support/public_testcases'
    checked = []
    for folder, stem, flags in [('Basic testcase', 'PA1', []), ('Bonus1 testcase', 'PA1_Bonus1', ['--double'])]:
        for input_file in sorted((base / folder).glob(stem + '_in*.txt')):
            output_file = input_file.with_name(input_file.name.replace('_in', '_out'))
            actual = run(executable, input_file.read_text(), flags)
            assert actual.rstrip('\n') == output_file.read_text().rstrip('\n'), input_file.name
            checked.append({'input': input_file.relative_to(ROOT).as_posix(), 'golden_matches': True})
    input_file = base / 'Bonus2 testcase/PA1_Bonus2_in1.txt'
    actual = run(executable, input_file.read_text(), ['--auto'])
    assert 'WIN' in actual and 'What a pity' not in actual and 'cannot jump' not in actual
    checked.append({'input': input_file.relative_to(ROOT).as_posix(), 'legal_winning_auto_path': True})
    return checked


def knight_checks(executable):
    base = ASSIGNMENTS / 'pa02/course_support/starter'
    checked = []
    for bonus, folder in [(False, 'PA2'), (True, 'PA2_bonus')]:
        for path in sorted((base / folder / 'Testcase').glob('*.txt')):
            values = path.read_text().split()
            n, length, energy = map(int, values[:3])
            bonuses = list(map(int, values[3:6]))
            board = values[6:]
            actual = run(executable, path.read_text(), ['--bonus'] if bonus else [])
            score = validate_knight(actual, n, length, energy, bonuses, board, bonus)
            record = {'input': path.relative_to(ROOT).as_posix(), 'score': score, 'legal_replay': True}
            if n == 4:
                assert score == knight_oracle(n, length, energy, bonuses, board, bonus)
                record['independent_exhaustive_optimum'] = True
            if bonus and n == 9:
                assert score == 57
                record['documented_bonus_optimum'] = 57
            checked.append(record)
    return checked


def cnn_checks(executable, scratch):
    base = ASSIGNMENTS / 'final/course_support/public_testcases'
    checked = []
    expected_resources = [(75170.0, 3130), (294825.0, 14130), (187400.0, 8320)]
    for i in range(1, 4):
        source = base / f'Testcase{i:02d}'
        golden = base / f'Testcase{i:02d}_output'
        output = scratch / f'cnn{i}'
        output.mkdir()
        run(executable, args=[source, output])
        for name in ['result_of_conv1.txt', 'result_of_conv2.txt', 'result_of_fc.txt']:
            assert csv_values(output / name) == csv_values(golden / name), (i, name)
        metrics = (output / 'execution_result.txt').read_text().splitlines()
        original = (golden / 'execution_result.txt').read_text().splitlines()
        assert metrics[:2] == original[:2], (i, metrics, original)
        energy = float(re.search(r'Energy Consumption: ([\d.]+)', metrics[2]).group(1))
        steps = int(re.search(r'Step Count: (\d+)', metrics[3]).group(1))
        assert (energy, steps) == expected_resources[i - 1]
        checked.append({'case': f'Testcase{i:02d}', 'kernel': [3, 7, 5][i - 1], 'convolution_and_fc_golden_matches': True, 'quantization_error_and_prediction_match': True, 'api_energy': energy, 'api_steps': steps})
    return checked


def pgm_checks(executable, scratch):
    base = ASSIGNMENTS / 'final/course_support/bonus_testcases'
    checked = []
    for i in range(1, 4):
        source = base / f'Testcase{i:02d}/img_label.pgm'
        tokens = re.sub(r'#[^\r\n]*', '', source.read_text()).split()
        assert tokens[0] == 'P2'
        width, height, maximum = map(int, tokens[1:4])
        pixels = list(map(int, tokens[4:]))
        assert len(pixels) == width * height
        output = scratch / f'bonus{i}.txt'
        run(executable, args=[source, output])
        expected = []
        for r in range(28):
            y = min(height - 1, max(0, (r + 0.5) * height / 28 - 0.5))
            y0 = int(y); y1 = min(height - 1, y0 + 1); fy = y - y0
            for c in range(28):
                x = min(width - 1, max(0, (c + 0.5) * width / 28 - 0.5))
                x0 = int(x); x1 = min(width - 1, x0 + 1); fx = x - x0
                gray = ((1 - fy) * ((1 - fx) * pixels[y0 * width + x0] + fx * pixels[y0 * width + x1]) + fy * ((1 - fx) * pixels[y1 * width + x0] + fx * pixels[y1 * width + x1]))
                expected.append(fp8(1 - gray / maximum))
        assert csv_values(output) == expected, i
        checked.append({'case': f'Testcase{i:02d}', 'pixels_checked': 784, 'latest_fp8_bilinear_oracle_matches': True, 'historical_quantization_text_is_not_current_golden': True})
    return checked


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx', default=os.environ.get('CXX', 'g++'))
    parser.add_argument('--build-dir', type=Path)
    parser.add_argument('--compile-only', action='store_true')
    parser.add_argument('--reuse-build', action='store_true')
    parser.add_argument('--report', type=Path)
    args = parser.parse_args()
    if Path(args.cxx).is_absolute():
        os.environ['PATH'] = str(Path(args.cxx).parent) + os.pathsep + os.environ.get('PATH', '')
    report = {'resources': resource_checks()}
    with tempfile.TemporaryDirectory(prefix='official-resources-2025-') as folder:
        scratch = Path(folder)
        directory = args.build_dir.resolve() if args.build_dir else scratch / 'build'
        if not args.reuse_build:
            build(args.cxx, directory)
        verify_build(directory)
        if args.compile_only:
            print('Four current solution programs compiled and checksummed; official downloads verified.')
            return
        report['dinosaur'] = dinosaur_checks(binary(directory, 'dinosaur'))
        report['knight'] = knight_checks(binary(directory, 'knight'))
        report['cnn'] = cnn_checks(binary(directory, 'cnn'), scratch)
        report['pgm'] = pgm_checks(binary(directory, 'preprocess'), scratch)
        report['passed'] = True
        if args.report:
            args.report.parent.mkdir(parents=True, exist_ok=True)
            args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(f"Official downloads: 5 ZIPs intact; {len(report['dinosaur'])} Dinosaur cases, {len(report['knight'])} Knight cases, 3 CNN public goldens and 3 latest-spec PGM oracles passed. Original checker binaries were not executed.")


if __name__ == '__main__':
    main()
