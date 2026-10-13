"""Independent integer, tokenization and file-state checks for the rewritten exams."""
import argparse
import math
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--cxx', default=os.environ.get('CXX', 'g++'))
    args = parser.parse_args()
    count = 0
    with tempfile.TemporaryDirectory(prefix='exams_') as temporary:
        folder = Path(temporary)
        exes = {}
        for group in ['midterm','final']:
            exe = folder / (group + ('.exe' if os.name == 'nt' else ''))
            subprocess.run([args.cxx,'-std=c++11','-O2','-Wall','-Wextra','-Wpedantic','-Werror',str(ROOT/f'exams/2021/{group}/main.cpp'),'-o',str(exe)],check=True)
            exes[group] = exe
        def check(group, mode, data='', expected='', code=0):
            nonlocal count
            result = subprocess.run([str(exes[group]),mode],input=data,text=True,capture_output=True,cwd=folder,timeout=10)
            assert (result.returncode,result.stdout,result.stderr) == (code,expected,''), (group,mode,result.returncode,result.stdout,result.stderr,expected)
            count += 1
        # Numeric oracle uses integer halving, not the recursive implementation.
        check('midterm','area',expected=''.join(f'{n} {1024 >> (n-1)}\n' for n in range(1,11)))
        check('midterm','conversion',expected='8\n')
        check('midterm','counter',expected=''.join(f'{i} ' for i in range(7))+'\n')
        check('midterm','letters',expected=''.join(f'{chr(i)} = {i}\n' for i in range(66,78) if i%4))
        rng = random.Random(20211024)
        for _ in range(40):
            values = [rng.randrange(-15,25) for _ in range(rng.randrange(0,18))]
            pairs = [(i,j) for i in range(len(values)) for j in range(i+1,len(values)) if values[i]+values[j]==12]
            expected = ''.join(f'{i} {j}: {values[i]} + {values[j]} = 12\n' for i,j in pairs) or 'No pair\n'
            check('midterm','pairs',str(len(values))+'\n'+' '.join(map(str,values))+'\n',expected)
        check('midterm','pairs','2\n2147483647 -2147483635\n','0 1: 2147483647 + -2147483635 = 12\n')
        for number in [2,3,4,12,1260,65536,2147483647,2147483646]+[rng.randrange(2,1000000) for _ in range(40)]:
            remaining = number
            factors = []
            # Prime candidates generated independently with a primality predicate.
            primes = [p for p in range(2,math.isqrt(number)+1) if all(p%d for d in range(2,math.isqrt(p)+1))]
            for prime in primes:
                while remaining%prime==0:
                    factors.append(prime); remaining//=prime
                if remaining==1: break
            if remaining>1: factors.append(remaining)
            check('midterm','factor',str(number)+'\n','Enter a number: '+str(number)+' = '+' x '.join(map(str,sorted(factors,reverse=True)))+'\n')
        invalid = ['x','12x','1','-1','2147483648','9999999999999999999999999']
        check('midterm','factor','\n'.join(invalid+['12'])+'\n','Enter a number: '+'Please enter a correct number: '*len(invalid)+'12 = 3 x 2 x 2\n')
        check('midterm','factor','','Enter a number: ',1)
        check('midterm','grades','59 60 60\n','59.667\nFAIL\n')
        check('midterm','grades','101 0 0\n','',1)
        check('midterm','grades','60.5 60 60\n','',1)
        check('midterm','grades','60 59 61.5\n','',1)
        check('midterm','grades','60 59 61x\n','',1)
        check('midterm','switch','1\n','one\ntwo\n')
        check('midterm','switch','2\n','two\n')
        check('midterm','switch','9\n','other\n')
        check('midterm','width',expected='....7|8|....9\n')
        check('final','array3d',expected=str(sum([3,4,2,3,0,-3,9,11,23,12,23,2,13,4,56,3,5,9,3,5,5,1,4,9]))+'\n')
        check('final','passing',expected='5\n6\n7\n')
        check('final','pointers',expected='A\nC\nB\nA\nB\nC\nB\nC\nC\nC\nB\n')
        for _ in range(30):
            words = [rng.choice(['alpha','beta','C++','array','42']) for _ in range(rng.randrange(0,10))]
            text = ''.join(rng.choice([' ,','!?','.-;',' ','--'])+word for word in words)+'?!'
            check('final','tokens',text+'\n',''.join(token+'\n' for token in re.split(r'[ ,.\-;!?]+',text) if token))
        for a,b in [(2147483647,2147483647),(-2147483648,-2147483648),(0,0)]+[(rng.randrange(-2147483648,2147483648),rng.randrange(-2147483648,2147483648)) for _ in range(30)]:
            check('final','class',f'{a} {b}\n',f'{a+b}\n')
        check('final','compare',expected='-1\n0\n1\n')
        check('final','constptr',expected='9\n')
        check('final','file',expected='character:1\ninteger:23\ncharacter:4\ninteger:56\n')
        assert (folder/'integers.txt').read_text()=='123 456'
    print(f'PASS: {count} independent exam executions')


if __name__=='__main__':
    main()
