"""Reproduce the six repaired anonymous PA1/PA2 reference checks.

Never executes archived originals. Separate Python integer/combination models
check latest specifications; signed default uses true minimal two's complement.
"""
import argparse
from contextlib import nullcontext
import importlib.util
import json
import os
from pathlib import Path
import random
import re
import subprocess
import tempfile
import threading

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location('oracle', ROOT/'tests/test_pa2021_01_03.py')
ORACLE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(ORACLE)
A, B = 'jade-lynx-c0ee', 'moss-lynx-5375'
VALUES = (100, 50, 10, 5, 1)
RESULTS = []


def execute_capped(executable, stdin, environment, directory):
    process = subprocess.Popen([str(executable)],stdin=subprocess.PIPE,stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE,cwd=directory,env=environment)
    chunks = [[],[]]
    exceeded = []
    def read(stream, destination):
        size = 0
        while True:
            block = stream.read(4096)
            if not block: break
            size += len(block)
            if size > 262144:
                exceeded.append(True)
                process.kill()
                break
            destination.append(block)
    readers = [threading.Thread(target=read,args=(stream,destination))
               for stream,destination in zip((process.stdout,process.stderr),chunks)]
    for reader in readers: reader.start()
    try:
        process.stdin.write(stdin.encode('utf-8'))
        process.stdin.close()
        process.wait(timeout=4)
    except subprocess.TimeoutExpired:
        process.kill(); process.wait()
        raise AssertionError('Program exceeded four-second timeout')
    except BrokenPipeError:
        process.wait(timeout=4)
    finally:
        for reader in readers: reader.join(timeout=4)
    output,error = [b''.join(destination).decode('utf-8') for destination in chunks]
    assert process.returncode == 0 and not error and not exceeded, error
    return output


def normal(text):
    return re.sub(r'\s+', '', text)


def binary_model(tokens, course_width=False):
    if course_width:
        return ORACLE.binary_model(tokens, True)
    def least_signed(n):
        if n >= 0:
            return '0' + format(n, 'b')
        width = (~n).bit_length() + 1
        return format((1 << width) + n, 'b')
    output = '*** Welcome to Binary Adder and Subtractor ***\n'
    index, partial, error = 1, 0, 0
    for token in tokens:
        if error:
            output += ('The decimal number you entered is out of range. ' if error == 1
                       else 'The partial sum is out of range. ')
        output += 'Please enter the ' + ORACLE.ordinal(index)
        output += ' decimal number again: ' if error else ' number in decimal: '
        text = str(token)
        valid = re.fullmatch(r'[+-]?[0-9]+', text) is not None
        value = int(text) if valid else None
        if valid and value == 0:
            output += 'Bye\n'
            break
        if not valid or not -262144 <= value <= 524287:
            error = 1
            continue
        if index == 1:
            index, partial, error = 2, value, 0
            continue
        result = partial + value
        if not -262144 <= result <= 524287:
            error = 2
            continue
        output += (f'Calculating... {least_signed(partial)} + {least_signed(value)}'
                   f' = {least_signed(result)}\nPartial Sum: {result}\n\n')
        index, partial, error = index + 1, result, 0
    return '\n'.join(map(str, tokens)) + '\n', output


def voucher_table(text):
    counts = [0]*5
    for count, value in re.findall(r'(\d+)\s*\$\s*(100|50|10|5|1)\s+voucher', text):
        counts[VALUES.index(int(value))] += int(count)
    return counts


def verify_return_change(output, transactions):
    wallet = [5]*5
    blocks = re.split(r'The price you need to pay:\s*(\d+)\r?\n', output)
    visited = 0
    for position in range(1, len(blocks), 2):
        need, block = int(blocks[position]), blocks[position+1]
        assert visited < len(transactions), 'Unexpected transaction'
        price, kind, member = transactions[visited]
        visited += 1
        rate = 90 if kind == 'p' else 72 if member == 'y' else 80
        assert need == price * rate // 100, 'Incorrect discount truncation'
        total = sum(c*v for c,v in zip(wallet, VALUES))
        if total < need:
            assert "You don't have enough voucher" in block
            assert 'The payment is processed as follows:' not in block
            assert 'You do not remain any voucher' not in block
            if 'The remaining vouchers are as follows:' in block:
                assert voucher_table(block.split('The remaining vouchers are as follows:',1)[1]) == wallet
            continue
        if 'The payment is processed as follows:' in block:
            payment = block.split('The payment is processed as follows:',1)[1]
            payment = re.split(r'Change is as follows:|The remaining vouchers|You do not remain',payment)[0]
            used = voucher_table(payment)
            assert all(c <= available for c, available in zip(used, wallet)), 'Spent absent vouchers'
        else:
            assert need == 0, 'Missing nonzero payment'
            used = [0]*5
        change = [0]*5
        if 'Change is as follows:' in block:
            change = voucher_table(re.split(r'The remaining vouchers|You do not remain',
                                           block.split('Change is as follows:',1)[1])[0])
        assert sum(c*v for c,v in zip(used,VALUES)) - sum(c*v for c,v in zip(change,VALUES)) == need
        remaining = (voucher_table(block.split('The remaining vouchers are as follows:',1)[1])
                     if 'The remaining vouchers are as follows:' in block else [0]*5)
        assert remaining == [held-paid+returned for held,paid,returned in zip(wallet,used,change)]
        assert total - sum(c*v for c,v in zip(remaining, VALUES)) == need, 'Money not conserved'
        if need == 0:
            assert remaining == wallet, 'Zero purchase changed wallet'
        wallet = remaining
        if not any(wallet):
            break
    assert visited == len(transactions) or not any(wallet), 'Transaction was skipped'


def run_all(options):
    environment = os.environ.copy()
    compiler = Path(options.cxx)
    if compiler.is_absolute():
        environment['PATH'] = str(compiler.parent) + os.pathsep + environment.get('PATH','')
    if options.build_dir:
        Path(options.build_dir).mkdir(parents=True,exist_ok=True)
    workspace = (nullcontext(str(Path(options.build_dir).resolve())) if options.build_dir else
                 tempfile.TemporaryDirectory(prefix='repaired_pa1_pa2_',dir=options.work_dir))
    with workspace as temporary:
        work = Path(temporary)
        for optimization in ('-O0','-O2'):
            binaries = {}
            for assignment, alias, variant in [('pa01',A,'basic'),('pa01',A,'return_change'),
                                                ('pa01',B,'early'),('pa02',A,'basic'),
                                                ('pa02',B,'basic'),('pa02',B,'signed')]:
                key = '/'.join((assignment,alias,variant))
                if options.only_signed and variant != 'signed':
                    continue
                if options.source and options.source != key:
                    continue
                source = ROOT/'assignments/2021'/assignment/'references'/alias/'fixed'/(variant+'.cpp')
                executable = work/(assignment+'_'+alias+'_'+variant+optimization+('.exe' if os.name=='nt' else ''))
                flags = ['-std=c++11',optimization,'-Wall','-Wextra','-Wpedantic','-Wconversion','-Werror']
                if os.name == 'nt': flags.extend(['-static','-static-libgcc','-static-libstdc++'])
                if variant == 'signed' and options.course_width: flags.append('-DCOURSE_WIDTH=1')
                if not options.reuse_build:
                    compiled = subprocess.run([options.cxx,*flags,str(source),'-o',str(executable)],
                                              capture_output=True,text=True,encoding='utf-8',env=environment,timeout=60)
                    assert compiled.returncode == 0, compiled.stderr
                binaries[key] = executable
            if options.compile_only:
                continue
            def execution(key, stdin):
                return execute_capped(binaries[key],stdin,environment,work)
            def check(key, label, stdin, expected=None, transactions=None):
                if key not in binaries:
                    return
                actual = execution(key,stdin)
                if expected is not None:
                    assert normal(actual) == normal(expected), (key,label,stdin,actual,expected)
                else:
                    verify_return_change(actual,transactions)
                RESULTS.append(dict(source=key,optimization=optimization,label=label,passed=True))
            public_pa1 = [
                [(255,'d','y'),(766,'p',''),(640,'p',''),(40,'d','n'),(-1,'','')],
                [(1000,'p',''),(789,'d','y'),(766,'p',''),(289,'p',''),(3,'d','n')],
                [(956,'d','y'),(148,'p',''),(-1,'','')],
                [(849,'d','n'),(167,'p',''),(2,'p','')],
                [(399,'d','n'),(408,'d','y'),(999,'p',''),(-1,'','')],
            ]
            pa1_cases = public_pa1 + [
                [(1,'d','y'),(-1,'','')],[(0,'p',''),(-1,'','')],
                [(100,'d','y'),(-1,'','')],[(200,'d','y'),(-1,'','')],
                [(2147483647,'d','y'),(923,'p',''),(-1,'','')],[(-1,'','')],
                [(0,'d','y'),(0,'d','n'),(0,'p',''),(100,'p',''),(-1,'','')],
            ]
            rng = random.Random(202101)
            pa1_cases += [[(rng.randrange(0,1200),rng.choice('dp'),rng.choice('yn'))
                           for _ in range(8)]+[(-1,'','')] for _ in range(80)]
            for alias, variant in [(A,'basic'),(B,'early')]:
                key = '/'.join(('pa01',alias,variant))
                for i, cases in enumerate(pa1_cases):
                    stdin, expected = ORACLE.vouchers_model(cases)
                    check(key,'payment-'+str(i),stdin,expected)
            key = '/'.join(('pa01',A,'return_change'))
            bonus_cases = [c for c in pa1_cases if c != [(-1,'','')]]
            rng = random.Random(202111)
            bonus_cases += [[(rng.randrange(1,700),rng.choice('dp'),rng.choice('yn'))
                              for _ in range(8)]+[(-1,'','')] for _ in range(35)]
            for i,cases in enumerate(bonus_cases):
                stdin = ''.join(str(price)+'\n'+(kind+'\n'+(member+'\n' if kind=='d' else '')
                                                   if price!=-1 else '')
                                for price,kind,member in cases)
                check(key,'return-change-'+str(i),stdin,transactions=[c for c in cases if c[0]!=-1])
            pa2_public = [
                [100,200,300,400,500,600,700,800,900,-1],
                [-123,9999987,12321,45654,1314,7414,-1],
                [123,321,456,654,-456,-456,12321456,4522733,522733,78987,0,-1],
                [0,0,123321,321321,456654,123321,78999874,123321,46711,-1],
                [214987654,123456,12345,778665,-1],
            ]
            pa2_cases = pa2_public + [
                [-20,-255,8,-77,-88,19,2147483647,152341,412386,999,-1],[-1],[5,-1],
                [-2,-1],[5,524288,-1],[524287,1,0,-1],[524286,1,1,0,-1],
                [2147483647,8,19,-1],[0]*123+[-1],[262143,1,262142,-1],
                ['9'*200,-1],['--1','1x','+','-2147483648','+5',6,-1],
                [5,6,'abc',-1], [524288,0,524287,-1],
            ]
            rng = random.Random(202102)
            pa2_cases += [[rng.randrange(0,600000) for _ in range(12)]+[-1] for _ in range(80)]
            for alias in (A,B):
                key = '/'.join(('pa02',alias,'basic'))
                for i,tokens in enumerate(pa2_cases):
                    stdin,expected = ORACLE.binary_model(tokens)
                    check(key,'recursive-adder-'+str(i),stdin,expected)
            signed_cases = [
                [8,-19,-11,0],[-1,-3,-123456,-451231,0],
                [8,19,2147483647,50,-222222,-444444,-1,0],[0],[5,0],
                [8,-3,0],[3,-8,0],[1,-1,5,0],[1,-1,-5,0],
                [-262143,-1,1,0],[524287,-1,1,0],
                [-262144,1,-1,-1,0],[524287,524287,-262144,0],
                [-262144,-262144,262144,0],['9'*200,0],
                ['--1','1x','+','-2147483648','+5',6,0],
                [5,6,'abc',0], [1]*123+[0],
            ]
            for power in range(19):
                signed_cases.append([-(1<<power),1,-1,0])
            rng = random.Random(202103)
            signed_cases += [[rng.randrange(-300000,600000) or 1 for _ in range(12)]+[0] for _ in range(100)]
            for i,tokens in enumerate(signed_cases):
                stdin,expected = binary_model(tokens,options.course_width)
                check('/'.join(('pa02',B,'signed')),'signed-adder-'+str(i),stdin,expected)
            for case in json.loads((ROOT/'tests/fixtures/repaired_pa1_pa2_cases.json').read_text(encoding='utf-8')):
                key=case['source']
                stdin=case['input']
                if key.endswith('/return_change'):
                    check(key,'prior-'+case['label'],stdin,transactions=case['transactions'])
                elif key.startswith('pa01/'):
                    tokens=iter(stdin.split())
                    transactions=[]
                    for token in tokens:
                        price=int(token)
                        if price==-1:
                            transactions.append((-1,'',''));break
                        kind=next(tokens)
                        member=next(tokens) if kind=='d' else ''
                        transactions.append((price,kind,member))
                    _,expected=ORACLE.vouchers_model(transactions)
                    check(key,'prior-'+case['label'],stdin,expected)
                else:
                    tokens=list(map(int,stdin.split()))
                    _,expected=(binary_model(tokens,options.course_width) if key.endswith('/signed')
                                else ORACLE.binary_model(tokens))
                    check(key,'prior-'+case['label'],stdin,expected)
    summaries = {}
    for result in RESULTS:
        summaries[result['source']] = summaries.get(result['source'],0)+1
    print(json.dumps(dict(course_width=options.course_width,executions=len(RESULTS),passed=summaries),indent=2))
    if options.report:
        Path(options.report).write_text(json.dumps(dict(course_width=options.course_width,summary=summaries,
                                                       executions=len(RESULTS),results=RESULTS),indent=2),encoding='utf-8')


if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx',default=os.environ.get('CXX','g++'))
    parser.add_argument('--course-width',action='store_true',help='Build signed reference with older example width convention')
    parser.add_argument('--report',help='Write an optional anonymous JSON report')
    parser.add_argument('--work-dir',help='Parent directory for temporary build/run files')
    parser.add_argument('--build-dir',help='Keep builds in this directory (allows separate compile/run stages)')
    parser.add_argument('--compile-only',action='store_true')
    parser.add_argument('--reuse-build',action='store_true')
    parser.add_argument('--only-signed',action='store_true',help='Check only the signed PA2 repair')
    parser.add_argument('--source',help='Limit execution to an assignment/alias/variant key')
    run_all(parser.parse_args())
