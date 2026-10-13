"""Compile the code actually embedded in solution.md, then replay all published examples."""
import argparse
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def safe_path(directory, name):
    path = (directory / name).resolve()
    assert path.is_relative_to(directory.resolve()), f"Unsafe attachment: {name}"
    path.parent.mkdir(parents=True, exist_ok=True)
    return path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"))
    args = parser.parse_args()
    book = json.loads((ROOT / "solutions.json").read_text(encoding="utf-8"))
    count = 0
    with tempfile.TemporaryDirectory(prefix="solution_book_") as temporary:
        temporary = Path(temporary)
        for problem in book["problems"]:
            folder = safe_path(temporary, problem["id"])
            folder.mkdir(parents=True, exist_ok=True)
            document = (ROOT / problem["document"]).read_text(encoding="utf-8")
            embedded = document.split("<!-- SOLUTION_CODE_BEGIN -->")[1].split("<!-- SOLUTION_CODE_END -->")[0]
            blocks = dict(re.findall(r"### `([^`]+)`\s+```cpp\n(.*?)\n```", embedded, re.S))
            assert set(blocks) == set(problem["solution"]["files"]), problem["id"]
            for name, body in problem["solution"]["files"].items():
                assert blocks[name] + "\n" == body.rstrip() + "\n", name
                safe_path(folder, name).write_text(blocks[name] + "\n", encoding="utf-8")
            exes = {}
            for variant in problem["solution"]["variants"]:
                entry = variant["entry"]
                name = Path(entry).stem
                executable = folder / (name + (".exe" if os.name == "nt" else ".bin"))
                cpp = [str(folder / Path(source).name) for source in variant["compile_sources"]]
                subprocess.run([args.cxx, "-std=c++11", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Werror", *cpp, "-o", str(executable)], check=True)
                exes[entry] = executable
            for sample in problem["examples"]:
                with tempfile.TemporaryDirectory(dir=folder, prefix="sample_") as run_dir:
                    run_dir = Path(run_dir)
                    for name, body in sample.get("files", {}).items():
                        safe_path(run_dir, name).write_text(body, encoding="utf-8", newline="\n")
                    result = subprocess.run([str(exes[sample["source"]]), *sample.get("args", [])],
                                            input=sample["stdin"], capture_output=True, text=True, cwd=run_dir, timeout=60)
                    context = f"{problem['id']} example {count + 1}"
                    assert result.returncode == sample.get("returncode", 0), (context, result.stderr)
                    assert result.stdout == sample["stdout"], (context, "stdout", result.stdout, sample["stdout"])
                    assert result.stderr == sample.get("stderr", ""), (context, "stderr", result.stderr)
                    for name, body in sample.get("outputs", {}).items():
                        assert safe_path(run_dir, name).read_text(encoding="utf-8") == body, (context, name)
                count += 1
    print(f"PASS: {len(book['problems'])} complete solution groups; {count} documented examples")


if __name__ == "__main__":
    main()
