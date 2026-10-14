"""Check repaired PA1's retained brute-force payment function in arbitrary wallets."""
import argparse
import importlib.util
import json
import os
from pathlib import Path
import random
import subprocess

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('repair_tests',ROOT/'tests/test_repaired_pa1_pa2.py')
tests=importlib.util.module_from_spec(spec)
spec.loader.exec_module(tests)

def main(options):
    work=Path(options.build_dir).resolve()
    work.mkdir(parents=True,exist_ok=True)
    environment=os.environ.copy()
    compiler=Path(options.cxx)
    if compiler.is_absolute():
        environment['PATH']=str(compiler.parent)+os.pathsep+environment.get('PATH','')
    source=ROOT/'assignments/2021/pa01/references/jade-lynx-c0ee/fixed/basic.cpp'
    harness=work/'wallet_harness.cpp'
    harness.write_text('#define main reference_main\n#include '+json.dumps(source.as_posix(),ensure_ascii=False)+
        '\n#undef main\nint main() { int wallet[5]; int price[1]; '
        'while(std::cin>>wallet[0]>>wallet[1]>>wallet[2]>>wallet[3]>>wallet[4]>>price[0]) {'
        'price_payment(price,wallet); std::cout<<"STATE"; '
        'for(int i=0;i<5;++i)std::cout<<" "<<wallet[i]; std::cout<<"\\n"; } }\n',encoding='utf-8')
    cases=[([5]*5,need) for need in range(831)]
    rng=random.Random(202104)
    for _ in range(500):
        wallet=[rng.randrange(6) for _ in range(5)]
        total=sum(n*v for n,v in zip(wallet,(1,5,10,50,100)))
        cases.append((wallet,rng.randrange(total+1)))
    count=0
    for optimization in ('-O0','-O2'):
        executable=work/('wallet_properties'+optimization+('.exe' if os.name=='nt' else ''))
        if not options.reuse_build:
            flags=['-std=c++11',optimization,'-Wall','-Wextra','-Wpedantic','-Wconversion','-Werror']
            if os.name=='nt':flags+=['-static','-static-libgcc','-static-libstdc++']
            p=subprocess.run([options.cxx,*flags,str(harness),'-o',str(executable)],
                             capture_output=True,text=True,encoding='utf-8',env=environment,timeout=60)
            assert not p.returncode,p.stderr
        if options.compile_only:continue
        for start in range(0,len(cases),100):
            batch=cases[start:start+100]
            stdin=''.join(' '.join(map(str,(*wallet,price)))+'\n' for wallet,price in batch)
            output=tests.execute_capped(executable,stdin,environment,work)
            states=[list(map(int,line.split()[1:])) for line in output.splitlines() if line.startswith('STATE')]
            assert len(states)==len(batch),'Skipped a wallet calculation'
            for (wallet,price),state in zip(batch,states):
                used=tests.ORACLE.payment_model(list(reversed(wallet)),price)
                expected=list(reversed([n-u for n,u in zip(reversed(wallet),used)]))
                assert state==expected,(wallet,price,state,expected)
                count+=1
    report=dict(wallet_states=len(cases),executions=count,passed=None if options.compile_only else True)
    print(json.dumps(report))
    if options.report:Path(options.report).write_text(json.dumps(report,indent=2),encoding='utf-8')

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cxx',default=os.environ.get('CXX','g++'))
    parser.add_argument('--build-dir',default=str(ROOT.parent/'repair_pa1_pa2/wallet_build'),
                        help='Build/run directory (defaults to a workspace sibling of the repository)')
    parser.add_argument('--compile-only',action='store_true')
    parser.add_argument('--reuse-build',action='store_true')
    parser.add_argument('--report')
    main(parser.parse_args())
