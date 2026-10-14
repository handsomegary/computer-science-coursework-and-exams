"""Materialize published examples, compile the real sources, and retain actual results.

Requires only Python's standard library and a g++-compatible C++11 compiler.
The output directory must not exist; supplied inputs and saved results are never
silently overwritten. Expected files are kept separate from program output.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def attachment(folder, name):
    path = (folder / name).resolve()
    if path == folder.resolve() or not path.is_relative_to(folder.resolve()):
        raise ValueError("Unsafe attachment path: " + name)
    path.parent.mkdir(parents=True, exist_ok=True)
    return path


def save(folder, name, body):
    with attachment(folder, name).open("x", encoding="utf-8", newline="\n") as stream:
        stream.write(body)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("problem", nargs="?", help="Problem ID, e.g. assignments/2021/pa04")
    parser.add_argument("--list", action="store_true", help="List problem IDs and example numbers")
    parser.add_argument("--example", type=int, default=1, help="Published example number (starts at 1)")
    parser.add_argument("--all", action="store_true", help="Run all examples of the selected problem")
    parser.add_argument("--all-assignments", action="store_true", help="Run all examples of all ten assignments")
    parser.add_argument("--prepare", action="store_true", help="Only write supplied inputs and the execution plan; do not compile or run")
    parser.add_argument("--output-dir", type=Path, help="New directory for inputs, build logs and actual results")
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--timeout", type=int, default=60, help="Timeout in seconds per build or execution")
    args = parser.parse_args()
    book = json.loads((ROOT / "solutions.json").read_text(encoding="utf-8"))
    problems = {problem["id"]: problem for problem in book["problems"]}
    for problem in problems.values():
        problem["examples"] = json.loads((ROOT / problem["id"] / "samples.json").read_text(encoding="utf-8"))
    if args.list:
        for identifier, problem in problems.items():
            print(identifier)
            for number, sample in enumerate(problem["examples"], 1):
                print("  " + str(number) + ": " + Path(sample["source"]).name
                      + " " + " ".join(sample.get("args", [])))
        return
    if args.timeout <= 0:
        parser.error("--timeout must be positive")
    if args.all_assignments:
        if args.problem:
            parser.error("Do not combine a problem ID with --all-assignments")
        selected = [problem for name, problem in problems.items() if name.startswith("assignments/")]
    else:
        if args.problem not in problems:
            parser.error("Specify a listed problem ID or --all-assignments")
        selected = [problems[args.problem]]
        if not args.all and not 1 <= args.example <= len(selected[0]["examples"]):
            parser.error("Example number is outside the published range")
    if args.output_dir is None:
        parser.error("--output-dir is required")
    output = Path(os.path.abspath(args.output_dir)).resolve()
    if output.exists():
        parser.error("Output directory already exists; choose a new directory")
    # samples.json is the published input/output source of truth; the solution
    # catalogue supplies only the real checked-in source/build relationships.
    for problem in selected:
        for variant in problem["solution"]["variants"]:
            for source in variant["sources"]:
                path = (ROOT / source).resolve()
                if not path.is_relative_to(ROOT) or not path.is_file():
                    parser.error("Missing source attachment: " + source)
    output.mkdir(parents=True)
    flags = ["-std=c++11", "-O2", "-Wall", "-Wextra", "-Wpedantic", "-Werror"]
    if os.name == "nt":
        flags += ["-static", "-static-libgcc", "-static-libstdc++", "-Wl,--no-insert-timestamp"]
    report = {"compiler": args.cxx, "compile_flags": flags, "examples": [],
              "status": "prepared" if args.prepare else "running", "passed": None if args.prepare else False}
    save(output, "README.txt", "run/ contains supplied inputs and actual program-generated files.\n"
         "expected/ contains published expected files, never copied to run/.\n"
         "stdout.txt and stderr.txt are actual captured process output.\n"
         "report.json records real exit codes and exact comparisons.\n")
    try:
        for problem in selected:
            folder = attachment(output, problem["id"] + "/placeholder").parent
            build = folder / "build"
            build.mkdir()
            executables = {}
            examples = list(enumerate(problem["examples"], 1))
            if not args.all and not args.all_assignments:
                examples = [examples[args.example - 1]]
            needed = {sample["source"] for _, sample in examples}
            for variant in problem["solution"]["variants"]:
                if variant["entry"] not in needed:
                    continue
                stem = Path(variant["entry"]).stem
                exe = build / (stem + (".exe" if os.name == "nt" else ".bin"))
                command = [args.cxx, *flags, *[str(ROOT / name) for name in variant["compile_sources"]], "-o", str(exe)]
                save(build, stem + ".compile.command.json", json.dumps(command, indent=2) + "\n")
                if not args.prepare:
                    compiled = subprocess.run(command, text=True, capture_output=True, timeout=args.timeout)
                    save(build, stem + ".compile.stdout.txt", compiled.stdout)
                    save(build, stem + ".compile.stderr.txt", compiled.stderr)
                    if compiled.returncode:
                        raise RuntimeError("Compilation failed: " + variant["entry"])
                executables[variant["entry"]] = exe
            for number, sample in examples:
                case = folder / ("example-" + str(number).zfill(2))
                run = case / "run"
                expected = case / "expected"
                run.mkdir(parents=True)
                expected.mkdir()
                for name, body in sample.get("files", {}).items():
                    save(run, name, body)
                for name, body in sample.get("outputs", {}).items():
                    save(expected, name, body)
                save(case, "stdin.txt", sample["stdin"])
                save(expected, "stdout.txt", sample["stdout"])
                save(expected, "stderr.txt", sample.get("stderr", ""))
                command = [str(executables[sample["source"]]), *sample.get("args", [])]
                save(case, "command.json", json.dumps({"argv": command, "cwd": str(run), "stdin": str(case / "stdin.txt"),
                                                      "mode": "prepare-only" if args.prepare else "run-requested"}, indent=2) + "\n")
                if args.prepare:
                    report["examples"].append({"problem": problem["id"], "example": number,
                                               "source": sample["source"], "executed": False, "passed": None,
                                               "result_directory": case.relative_to(output).as_posix()})
                    print("PREPARED (not executed) " + problem["id"] + " example " + str(number))
                    continue
                result = subprocess.run(command, input=sample["stdin"], text=True, capture_output=True,
                                        cwd=run, timeout=args.timeout)
                save(case, "stdout.txt", result.stdout)
                save(case, "stderr.txt", result.stderr)
                comparisons = {
                    "returncode": result.returncode == sample.get("returncode", 0),
                    "stdout": result.stdout == sample["stdout"],
                    "stderr": result.stderr == sample.get("stderr", ""),
                }
                for name, body in sample.get("outputs", {}).items():
                    path = attachment(run, name)
                    comparisons["file:" + name] = path.is_file() and path.read_text(encoding="utf-8") == body
                report["examples"].append({
                    "problem": problem["id"], "example": number, "source": sample["source"],
                    "source_sha256": digest(ROOT / sample["source"]), "returncode": result.returncode,
                    "executed": True,
                    "result_directory": case.relative_to(output).as_posix(), "comparisons": comparisons,
                    "passed": all(comparisons.values()),
                })
                if not all(comparisons.values()):
                    raise RuntimeError("Published example mismatch: " + problem["id"] + " #" + str(number))
                print("PASS " + problem["id"] + " example " + str(number))
        if not args.prepare:
            report["passed"] = True
            report["status"] = "passed"
    except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
        report["status"] = "failed"
        report["error"] = str(error)
        raise
    finally:
        save(output, "report.json", json.dumps(report, ensure_ascii=False, indent=2) + "\n")
    print(("Prepared inputs and plan; no execution: " if args.prepare else "Saved actual inputs and results to ") + str(output))


if __name__ == "__main__":
    main()
