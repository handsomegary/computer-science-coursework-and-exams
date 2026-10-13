"""Independent arithmetic, exhaustive-search and CNN oracles for the 2025 projects."""
import argparse
import math
import os
from pathlib import Path
import random
import re
import struct
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
ASSIGNMENTS = ROOT / "assignments/2025"


def compile_program(cxx, directory, name, sources):
    executable = directory / (name + (".exe" if os.name == "nt" else ""))
    result = subprocess.run([cxx, "-std=c++11", "-O2", "-Wall", "-Wextra", "-Wpedantic", *map(str, sources), "-o", str(executable)], capture_output=True, text=True, check=True)
    assert not result.stderr, result.stderr
    return executable


def run(executable, text="", args=(), cwd=None, timeout=30):
    result = subprocess.run([str(executable), *map(str, args)], input=text, text=True, capture_output=True, cwd=cwd, timeout=timeout)
    assert result.returncode == 0, (result.returncode, result.stderr)
    assert not result.stderr, result.stderr
    return result.stdout


def dinosaur_tests(executable):
    assert "The game ended when it just started." in run(executable, "-1\n")
    text = "00000000\n1\n0\n1\n0.1\n0\n0\n0\n0\n0\n0\n0\n-1\n"
    output = run(executable, text, ["--no-echo"])
    assert output.count("The dinosaur cannot jump again") == 1
    assert output.count("The dinosaur doesn't understand this command") == 1
    rows = re.findall(r"\|(?: [ DCP]\|){9}", output)
    heights = []
    for first in range(0, len(rows), 4):
        occupied = [3-r for r, line in enumerate(rows[first:first+4]) if line[2] == "D"]
        heights.append(min(occupied))
    assert heights == [0, 1, 2, 1, 0], heights
    assert "WIN" in output
    assert "What a pity" in run(executable, "10000000\n0\n0\n")
    assert "WIN" in run(executable, "10000000\n0\n-1\n")
    for obstacle, command, survives in [(1, 1, True), (2, 1, False), (3, 2, True), (4, 1, False)]:
        text = f"{obstacle}0000000\n{command}\n0\n0\n-1\n"
        output = run(executable, text)
        assert ("What a pity" not in output) == survives
    # Full double ascent, landing, unsupported fall, and forced crouch fall.
    text = "0\n0\n000011111\n"
    actions = [1,0,1,0,0,0,0,0,1,2,0,0,0,2]
    for tick, action in enumerate(actions, 1):
        text += f"{action}\n0\n0\n{0 if tick == 3 else 1}\n"
    text += "0\n-1\n"
    output = run(executable, text, ["--double"])
    rows = re.findall(r"\|(?: [ DCP]\|){9}", output)
    heights = [min(7-r for r, line in enumerate(rows[start:start+8]) if line[2] == "D") for start in range(0,len(rows),8)]
    assert heights == [0,1,2,3,4,5,6,5,4,5,4,3,2,1,0], heights
    # Auto mode explores the complete time/state graph and avoids close obstacles.
    auto = "01234023\n0\n0\n1\n0\n1\n2\n4\n1\n4\n4\n1\n1\n1\n0\n0\n0\n1\n0\n0\n0\n-1\n"
    output = run(executable, auto, ["--auto"])
    assert "WIN" in output and "What a pity" not in output
    assert "cannot jump" not in output
    long_auto = "0\n0\n111111111\n" + "0\n0\n1\n" * 10000 + "-1\n"
    assert "WIN" in run(executable, long_auto, ["--auto"], timeout=10)


def knight_oracle(n, length, energy, values, board, bonus):
    start = board.index("S")
    seeds = [p for p, token in enumerate(board) if token == "A"]
    def blocked(p, step):
        radius = min(step // 5, 3)
        return any(max(abs(p//n-q//n), abs(p%n-q%n)) <= radius for q in seeds) if bonus else False
    neighbors = [[q for q in range(n*n) if sorted([abs(p//n-q//n),abs(p%n-q%n)]) == [1,2]] for p in range(n*n)]
    best = 0
    def dfs(p, step, energy, score, visited):
        nonlocal best
        best = max(best, score)
        if step == length or energy == 0: return
        for q in neighbors[p]:
            if q in visited or board[q] == "#" or blocked(q, step+1): continue
            gain = 1 + (values[0] if board[q] == "B" else 0) - (values[2] if board[q] == "X" else 0)
            recharge = values[1] if board[q] == "E" else 0
            dfs(q, step+1, energy-1+recharge, score+gain, visited|{q})
    dfs(start, 0, energy, 0, {start})
    return best


def validate_knight(output, n, length, energy, values, board, bonus):
    summary = re.search(r"Best score: (\d+)\nSteps: (\d+)", output)
    assert summary, output
    declared_score, declared_steps = map(int, summary.groups())
    rows = re.findall(r"^(\d+)\s+\((\d+),(\d+)\)\s+(\w+)\s+(-?\d+)\s+(\d+)$", output, re.MULTILINE)
    assert len(rows) == declared_steps + 1
    score = 0; visited = set(); previous = None
    seeds = [p for p,t in enumerate(board) if t == "A"]
    for expected, (step, x, y, event, reported_score, reported_energy) in enumerate(rows):
        step,x,y,reported_score,reported_energy = map(int, (step,x,y,reported_score,reported_energy))
        assert step == expected and 0 <= x < n and 0 <= y < n
        p = (n-1-y)*n+x
        assert p not in visited and board[p] != "#"
        if step == 0: assert board[p] == "S" and event == "start"
        else:
            assert energy > 0
            assert sorted([abs(p//n-previous//n),abs(p%n-previous%n)]) == [1,2]
            score += 1 + (values[0] if board[p] == "B" else 0) - (values[2] if board[p] == "X" else 0)
            energy += -1 + (values[1] if board[p] == "E" else 0)
        if bonus: assert all(max(abs(p//n-q//n),abs(p%n-q%n)) > min(step//5,3) for q in seeds)
        assert (reported_score,reported_energy) == (score,energy)
        visited.add(p);previous=p
    assert declared_score == score and declared_steps <= length
    return declared_score


def knight_tests(executable):
    rng = random.Random(2025)
    for bonus in (False, True):
        for _ in range(24):
            n=4; length=rng.randrange(1,8); energy=rng.randrange(1,7)
            values=[rng.randrange(1,5) for _ in range(3)]
            board=[rng.choice("OOOBEX#"+("A" if bonus else "")) for _ in range(n*n)]
            board[rng.randrange(n*n)]="S"
            text=f"{n} {length} {energy}\n"+" ".join(map(str,values))+"\n"+"\n".join(" ".join(board[r*n:(r+1)*n]) for r in range(n))+"\n"
            output=run(executable,text,["--bonus"] if bonus else [])
            score=validate_knight(output,n,length,energy,values,board,bonus)
            assert score == knight_oracle(n,length,energy,values,board,bonus)
    sample="4 14 4\n2 2 1\nS O O O\nO X X #\nO X O O\nO O B O\n"
    assert run(executable,sample,["--summary"]).split()[0]=="4"
    # Growth after move 5 is checked against the destination, not the old square.
    sample="9 25 5\n5 4 3\nA O # O E O O O A\nO O O O O O E E O\nO B O O X O X O O\nO O E B O O B # O\n# X O X S O X O B\nX X X O B X O B O\nB X # O E O O O E\nO O E B O O B # O\nA X O X O X X O A\n"
    board=sample.splitlines()[2:];board=" ".join(board).split()
    output=run(executable,sample,["--bonus"],timeout=60)
    assert validate_knight(output,9,25,5,[5,4,3],board,True)==57


def radix(value, base):
    if value==0:return "0"
    sign="-" if value<0 else "";value=abs(value);digits=""
    while value: digits="0123456789ABCDEF"[value%base]+digits;value//=base
    return sign+digits


def energy_tests(executable):
    rng=random.Random(3100)
    for base in range(2,17):
        for bonus in (False,True):
            n=3;d=3
            energy=[[rng.randrange(-10**30 if bonus else 0,10**30) for _ in range(n)] for _ in range(n)]
            weights=[[rng.randrange(-10**20 if bonus else 0,10**20) for _ in range(d)] for _ in range(d)]
            text=f"{base}\n{n}\n"+"\n".join(" ".join(radix(x,base) for x in row) for row in energy)+f"\n{d}\n"+"\n".join(" ".join(radix(x,base) for x in row) for row in weights)+"\n"
            expected=[]
            for r in range(n):
                for c in range(n):
                    expected.append(sum(energy[r+i-1][c+j-1]*weights[i][j] for i in range(d) for j in range(d) if 0<=r+i-1<n and 0<=c+j-1<n))
            args=["--plain"]+(["--bonus"] if bonus else [])
            actual=run(executable,text,args).split()
            expected_text=[radix(value,base) for value in expected]
            if bonus:
                total=sum(expected);q,r=divmod(abs(total),n*n)
                expected_text.append(radix((-1 if total<0 else 1)*(q+(2*r>=n*n)),base))
            assert actual==expected_text, (base,actual,expected_text)
    # Negative half, normalized negative zero and long carry chains.
    assert run(executable,"10\n2\n-1 -1\n0 0\n1\n1\n",["--plain","--bonus"]).split()[-1]=="-1"
    assert run(executable,"16\n1\n-000\n1\nF\n",["--plain","--bonus"]).split()==["0","0"]
    hundred="9"*100
    assert run(executable,f"10\n1\n{hundred}\n1\n{hundred}\n",["--plain"]).strip()==str(int(hundred)**2)
    result=run(executable,"1\n16\n0\n1\nG\nF\n2\n1\n1\n",["--plain"])
    assert result.count("Invalid input!")==4 and result.splitlines()[-1]=="F"


def fp32(x):return struct.unpack("f",struct.pack("f",x))[0]
def fp8(x):
    x=fp32(x)
    if abs(x)<2**-6:return 0.0
    fraction,exponent=math.frexp(abs(x));field=exponent-1+7
    mantissa=math.floor((fraction*2-1)*8+0.5)
    if mantissa==8:field+=1;mantissa=0
    value=min(240.0,math.ldexp(1+mantissa/8,field-7))
    return -value if x<0 else value


def fixture(k):
    image=[0.0 if i%11==0 else [0.007,0.015625,0.1,0.23,0.75,1.0][i%6] for i in range(784)]
    w1=[fp32(((i%9)-4)/32) for i in range(2*k*k)]
    w2=[fp32(((i%11)-5)/64) for i in range(8*k*k)]
    b1=[0.125,-0.25];b2=[0.5,-0.25,0.125,0.0]
    size1=(28-k)//2+1;size2=(size1-k)//2+1;m=4*size2*size2
    fw=[fp32(((i%7)-3)/128) for i in range(m*10)];fb=[0,1,2,30,4,5,6,7,8,9]
    values={"img_label.txt":image,"conv1_weight.txt":w1,"conv2_weight.txt":w2,"conv1_bias.txt":b1,"conv2_bias.txt":b2,"fc_weight.txt":fw,"fc_bias.txt":fb}
    return values


def cpu_convolution(channels, weights, biases, outputs, k):
    size=len(channels[0]);out_size=(size-k)//2+1;result=[]
    for oc in range(outputs):
        rows=[]
        for r in range(out_size):
            row=[]
            for c in range(out_size):
                value=fp32(biases[oc])
                for ic in range(len(channels)):
                    for i in range(k):
                        for j in range(k):
                            value=fp32(value+fp32(channels[ic][2*r+i][2*c+j]*weights[((oc*len(channels)+ic)*k+i)*k+j]))
                row.append(value)
            rows.append(row)
        result.append(rows)
    return result


def csv_values(path):return [float(x) for x in path.read_text().replace("\n",",").split(",") if x]


def cnn_tests(executable,bonus_executable,directory):
    for k in (3,5,7):
        values=fixture(k)
        for name,data in values.items():(directory/name).write_text(",".join(map(str,data))+"\n")
        run(executable,cwd=directory)
        quantized=[fp8(x) for x in values["img_label.txt"]]
        first=cpu_convolution([[quantized[r*28:(r+1)*28] for r in range(28)]],values["conv1_weight.txt"],values["conv1_bias.txt"],2,k)
        second=cpu_convolution(first,values["conv2_weight.txt"],values["conv2_bias.txt"],4,k)
        flattened=[x for channel in second for row in channel for x in row]
        classes=[]
        for c in range(10):
            value=fp32(values["fc_bias.txt"][c])
            for p,x in enumerate(flattened):value=fp32(value+fp32(x*values["fc_weight.txt"][c*len(flattened)+p]))
            classes.append(value)
        for name,tensor in [("result_of_conv1.txt",first),("result_of_conv2.txt",second)]:
            expected=[x for channel in tensor for row in channel for x in row];actual=csv_values(directory/name)
            assert len(actual)==len(expected)
            assert max(abs(x-y) for x,y in zip(actual,expected))<0.00105
        actual=csv_values(directory/"result_of_fc.txt")
        assert max(abs(x-y) for x,y in zip(actual,classes))<0.0011
        metrics=(directory/"execution_result.txt").read_text()
        error=sum(abs(fp32(x)-q)/abs(fp32(x)) if x else 0 for x,q in zip(values["img_label.txt"],quantized))*100/784
        assert f"Relative Error after Quantization: {error:.2f}%" in metrics
        assert "Result of the Prediction: 3" in metrics
        size1=len(first[0]);size2=len(second[0]);steps=5*math.ceil(k/3)**2*(2*size1**2+8*size2**2)
        assert f"Step Count: {steps}" in metrics
        energy=float(re.search(r"Energy Consumption: ([\d.]+)",metrics).group(1))
        assert 15*steps<=energy<=30*steps+50*math.ceil(k/3)**2
    # Independent interpolation and quantization oracle, plus comments in PGM.
    (directory/"image.pgm").write_text("P2\n# synthetic gradient\n2 2\n255\n0 255\n128 64\n")
    run(bonus_executable,args=["image.pgm","image.txt"],cwd=directory)
    expected=[];pixels=[[0,255],[128,64]]
    for r in range(28):
        y=max(0,min(1,(r+0.5)*2/28-0.5));y0=int(y);y1=min(y0+1,1);fy=y-y0
        for c in range(28):
            x=max(0,min(1,(c+0.5)*2/28-0.5));x0=int(x);x1=min(x0+1,1);fx=x-x0
            gray=(1-fy)*((1-fx)*pixels[y0][x0]+fx*pixels[y0][x1])+fy*((1-fx)*pixels[y1][x0]+fx*pixels[y1][x1])
            expected.append(fp8(1-gray/255))
    assert csv_values(directory/"image.txt")==expected


def main():
    parser=argparse.ArgumentParser();parser.add_argument("--cxx",default=os.environ.get("CXX","g++"));args=parser.parse_args()
    compiler_path=Path(args.cxx)
    if compiler_path.is_absolute():os.environ["PATH"]=str(compiler_path.parent)+os.pathsep+os.environ.get("PATH","")
    with tempfile.TemporaryDirectory(prefix="pa2025-") as temp:
        directory=Path(temp)
        dinosaur=compile_program(args.cxx,directory,"dinosaur",[ASSIGNMENTS/"pa01/main.cpp"])
        knight=compile_program(args.cxx,directory,"knight",[ASSIGNMENTS/"pa02/main.cpp"])
        energy=compile_program(args.cxx,directory,"energy",[ASSIGNMENTS/"pa03/main.cpp"])
        cnn=compile_program(args.cxx,directory,"cnn",[ASSIGNMENTS/"final/main.cpp",ASSIGNMENTS/"final/hardware_architecture.cpp"])
        preprocess=compile_program(args.cxx,directory,"preprocess",[ASSIGNMENTS/"final/bonus.cpp"])
        dinosaur_tests(dinosaur);knight_tests(knight);energy_tests(energy);cnn_tests(cnn,preprocess,directory)
    print("2025 projects: dinosaur states/auto, 48 exhaustive knight oracles, all 15 big-number bases, 3 CNN kernels and PGM preprocessing passed.")


if __name__=="__main__":main()
