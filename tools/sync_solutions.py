"""Embed complete C++ solutions and executable examples; build a portable solution book."""
import argparse
import json
from pathlib import Path
import re
import shlex

ROOT = Path(__file__).resolve().parents[1]
CODE_BEGIN, CODE_END = "<!-- SOLUTION_CODE_BEGIN -->", "<!-- SOLUTION_CODE_END -->"
SAMPLES_BEGIN, SAMPLES_END = "<!-- SAMPLES_BEGIN -->", "<!-- SAMPLES_END -->"


def relative(path):
    return path.relative_to(ROOT).as_posix()


def replace_section(text, start, end, body):
    assert text.count(start) == text.count(end) == 1, "Missing/duplicate document marker"
    before, rest = text.split(start)
    _, after = rest.split(end)
    return before + start + "\n\n" + body + "\n" + end + after


def examples_text(samples):
    parts = []
    for i, sample in enumerate(samples, 1):
        parts += [f"### 範例 {i}（`{Path(sample['source']).name}`）", ""]
        if sample.get("args"):
            parts += ["執行參數：`" + shlex.join(sample["args"]) + "`", ""]
        for name, text in sample.get("files", {}).items():
            parts += [f"輸入檔 `{name}`：", "```text", text.rstrip("\n"), "```", ""]
        parts += ["標準輸入：", "```text", sample.get("stdin", "").rstrip("\n"), "```", "",
                  "標準輸出：", "```text", sample.get("stdout", "").rstrip("\n"), "```", ""]
        if sample.get("stderr"):
            parts += ["標準錯誤：", "```text", sample["stderr"].rstrip("\n"), "```", ""]
        for name, text in sample.get("outputs", {}).items():
            parts += [f"產生檔 `{name}`：", "```text", text.rstrip("\n"), "```", ""]
        parts += [f"結束碼：`{sample.get('returncode', 0)}`。精確換行、空白及檔案內容以 `samples.json` 為準。", ""]
    return "\n".join(parts)


def generated_files():
    groups, targets = [], {}
    writes = {}
    manifests = sorted(ROOT.glob("assignments/*/*/samples.json")) + sorted(ROOT.glob("exams/*/*/samples.json"))
    project_folders = {p.parent for base in ["assignments", "exams"] for p in ROOT.glob(base + "/*/*/main.cpp")}
    assert {p.parent for p in manifests} == project_folders, "Every project needs samples.json"
    for manifest in manifests:
        folder = manifest.parent
        samples = json.loads(manifest.read_text(encoding="utf-8"))
        assert isinstance(samples, list) and samples, f"Empty examples: {manifest}"
        sources = {path.name: path.read_text(encoding="utf-8") for path in sorted(folder.iterdir())
                   if path.suffix in {".cpp", ".h", ".hpp"}}
        assert sources, f"No code: {folder}"
        variants = {}
        for sample in samples:
            for required in ["source", "stdin", "stdout"]:
                assert required in sample, f"Missing {required}: {manifest}"
            entry = sample["source"]
            required_sources = sample.get("sources", [entry])
            compile_sources = sample.get("compile_sources", [p for p in required_sources if p.endswith(".cpp")])
            assert entry in compile_sources, f"Entry not listed: {entry}"
            assert set(compile_sources) <= set(required_sources), f"Compile file not listed: {entry}"
            for source in required_sources:
                path = ROOT / source
                assert path.is_file() and path.parent == folder, f"Unexpected source: {source}"
            variant = {"sources": required_sources, "compile_sources": compile_sources}
            assert entry not in variants or variants[entry] == variant, f"Inconsistent variant: {entry}"
            variants[entry] = variant
        entries = {relative(p) for p in folder.glob("*.cpp") if re.search(r"\bint\s+main\s*\(", p.read_text(encoding="utf-8"))}
        assert set(variants) == entries, f"Every executable entry needs an example: {relative(folder)}"
        for document in sorted({sample.get("document", "solution.md") for sample in samples}):
            doc_path = folder / document
            text = doc_path.read_text(encoding="utf-8")
            code = "\n".join(f"### `{name}`\n\n```cpp\n{body.rstrip()}\n```\n" for name, body in sources.items())
            text = replace_section(text, CODE_BEGIN, CODE_END, code)
            text = replace_section(text, SAMPLES_BEGIN, SAMPLES_END,
                                   examples_text([s for s in samples if s.get("document", "solution.md") == document]))
            writes[doc_path] = text
        doc_path = folder / samples[0].get("document", "solution.md")
        explanation = re.sub(re.escape(CODE_BEGIN) + r".*?" + re.escape(CODE_END), "", writes[doc_path], flags=re.S)
        explanation = re.sub(re.escape(SAMPLES_BEGIN) + r".*?" + re.escape(SAMPLES_END), "", explanation, flags=re.S).strip()
        identifier = relative(folder)
        groups.append({"id": identifier, "title": explanation.splitlines()[0].lstrip("# "),
                       "document": relative(doc_path), "problem_and_explanation": explanation,
                       "solution": {"code": sources.get("main.cpp", ""), "files": sources,
                                    "variants": [{"entry": entry, **variant} for entry, variant in variants.items()]},
                       "examples": samples})
        for entry, variant in variants.items():
            target = re.sub(r"[^A-Za-z0-9_]", "_", entry[:-4])
            targets[target] = variant["compile_sources"]
    assert groups, "No projects"
    writes[ROOT / "solutions.json"] = json.dumps({"format_version": 1, "language": "C++11", "problems": groups}, ensure_ascii=False, indent=2) + "\n"
    lines = ["# Generated by tools/sync_solutions.py. Do not edit by hand."]
    for target, sources in sorted(targets.items()):
        lines += [f"add_executable({target} " + " ".join('"' + p + '"' for p in sources) + ")",
                  f'target_compile_options({target} PRIVATE "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>")']
    writes[ROOT / "build_targets.cmake"] = "\n".join(lines) + "\n"
    return writes, len(groups), len(targets)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    writes, group_count, entry_count = generated_files()
    stale = []
    for path, text in writes.items():
        if not path.exists() or path.read_text(encoding="utf-8") != text:
            if args.check: stale.append(relative(path))
            else: path.write_text(text, encoding="utf-8", newline="\n")
    if stale:
        raise SystemExit("Out of sync: " + ", ".join(stale))
    print(f"Solution book: {group_count} groups, {entry_count} executable entries; " + ("in sync" if args.check else "updated"))


if __name__ == "__main__":
    main()
