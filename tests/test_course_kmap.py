"""Check the supplied K-map program against complete truth tables and exact cover."""
import argparse
import os
from pathlib import Path
import random
import shutil
import subprocess
import tempfile

from test_pa2021_04_05_final import exact_small_cover

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'assignments/2021/final/course_support/kmap.cpp'


def gray(n, position):
    columns = (n + 1) // 2
    row, column = position >> columns, position & ((1 << columns) - 1)
    return ((row ^ (row >> 1)) << columns) | (column ^ (column >> 1))


def check(compiler):
    env = os.environ.copy()
    if Path(compiler).parent != Path('.'):
        env['PATH'] = str(Path(compiler).parent) + os.pathsep + env.get('PATH', '')
    cases = [(n, mask) for n in (2, 3, 4) for mask in range(1 << (1 << n))]
    with tempfile.TemporaryDirectory(prefix='course-kmap-') as temporary:
        directory = Path(temporary)
        shutil.copy2(SOURCE, directory / 'kmap.cpp')
        (directory / 'runner.cpp').write_text('#include "kmap.cpp"\nint main() { top(); }\n', encoding='utf-8')
        executable = directory / ('runner.exe' if os.name == 'nt' else 'runner')
        result = subprocess.run(
            [compiler, '-std=c++11', '-O2', '-Wall', '-Wextra', '-Wpedantic',
             str(directory / 'runner.cpp'), '-o', str(executable)],
            cwd=directory, env=env, capture_output=True, text=True, timeout=60)
        assert result.returncode == 0 and not result.stderr, result.stderr
        with (directory / 'kmap_in.txt').open('w', encoding='ascii', newline='\n') as handle:
            handle.write(str(len(cases)) + '\n')
            for n, mask in cases:
                handle.write(str(n) + '\n')
                handle.write(' '.join(str((mask >> gray(n, position)) & 1)
                                      for position in range(1 << n)) + '\n')
        execution = subprocess.run([str(executable)], cwd=directory, env=env,
                                   capture_output=True, timeout=60)
        assert execution.returncode == 0 and not execution.stdout and not execution.stderr
        blocks = []
        for line in (directory / 'kmap_out.txt').read_text(encoding='ascii').splitlines():
            if line.startswith('#'):
                assert int(line[1:]) == len(blocks) + 1
                blocks.append([])
            elif line.strip():
                blocks[-1].append([int(value) for value in line.split()])
        assert len(blocks) == len(cases)
        for (n, mask), cubes in zip(cases, blocks):
            assert all(len(cube) == n and set(cube) <= {0, 1, 2} for cube in cubes)
            actual = 0
            for value in range(1 << n):
                if any(all(bit == 2 or bit == ((value >> (n - 1 - j)) & 1)
                           for j, bit in enumerate(cube)) for cube in cubes):
                    actual |= 1 << value
            assert actual == mask, (n, mask, actual, cubes)
        indices = list(range(272)) + [272, len(cases) - 1]
        indices += random.Random(20261013).sample(range(272, len(cases)), 40)
        nonminimal = 0
        for index in indices:
            n, mask = cases[index]
            on = {value for value in range(1 << n) if mask & (1 << value)}
            optimum = exact_small_cover(n, on)
            cubes = blocks[index]
            cost = len(cubes), sum(bit != 2 for cube in cubes for bit in cube)
            assert cost >= optimum
            nonminimal += cost != optimum
        print(f'Course K-map: {len(cases)} truth tables equivalent; '
              f'{nonminimal}/{len(indices)} checked outputs are not exact minima')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=os.environ.get('CXX', 'g++'))
    check(parser.parse_args().cxx)
