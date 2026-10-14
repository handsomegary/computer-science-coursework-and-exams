#!/usr/bin/env python3
"""Check the local-source archive's declared files, provenance and references.

This is preservation verification only. It does not compile historical drafts,
repair them, or assert conformance with any assignment specification. Separate
notes, reports and privacy-transformed classroom captures are checked by their
own declared hashes, separately from the historical source-token checks.
"""
from __future__ import annotations

import hashlib
import json
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "source_archive/manifest.json"
STATUSES = {
    "original_local_draft_unverified",
    "original_student_submission_unverified",
    "original_student_header_unverified",
    "original_project_configuration_unverified",
    "original_local_command_sequence_unverified",
    "original_local_input_or_expected_output_copy_unverified",
    "program_generated_local_output",
}
TOKEN = re.compile(
    rb'//[^\r\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|[A-Za-z_][A-Za-z_0-9]*|\d+(?:\.\d*)?(?:[eE][+-]?\d+)?|[^\s]',
    re.M,
)


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def nonliteral_digest(data: bytes) -> str:
    tokens = [m.group() for m in TOKEN.finditer(data)
              if not m.group().startswith((b"//", b"/*"))]
    return digest(b"\0".join(b"<STRING_LITERAL>" if t.startswith(b'"') else t for t in tokens))


def within_repo(path: Path) -> Path:
    resolved = path.resolve()
    if not resolved.is_relative_to(ROOT):
        raise ValueError(f"Reference escapes repository: {path}")
    return resolved


def check_learning_materials() -> dict:
    notes = json.loads((ROOT / "source_archive/notes/manifest.json").read_text(encoding="utf-8"))
    reports = json.loads((ROOT / "source_archive/reports/manifest.json").read_text(encoding="utf-8"))
    for row in notes["files"]:
        raw = within_repo(ROOT / row["path"]).read_bytes()
        if len(raw) != row["size"] or digest(raw) != row["sha256"]:
            raise ValueError("Learning note changed: " + row["path"])
        if row["byte_exact"] and row["source_sha256"] != row["sha256"]:
            raise ValueError("Note incorrectly labelled byte-exact: " + row["path"])
    for row in reports["files"]:
        raw = within_repo(ROOT / row["path"]).read_bytes()
        if len(raw) != row["bytes"] or digest(raw) != row["published_sha256"]:
            raise ValueError("Anonymous report changed: " + row["path"])
        if not raw.startswith(b"%PDF-") or not all(p["final_render_visually_reviewed"] for p in row["pages"]):
            raise ValueError("Anonymous report missing PDF/privacy review: " + row["path"])
    captures = ROOT / "assignments/2021/pa04/originals/local_notes/classroom-notes.json"
    classroom = json.loads(captures.read_text(encoding="utf-8"))
    raw = within_repo(captures.parent / classroom["pdf"]).read_bytes()
    if len(raw) != classroom["bytes"] or digest(raw) != classroom["sha256"]:
        raise ValueError("Anonymous classroom document changed")
    if classroom["byte_exact"] or len(classroom["pages"]) != classroom["page_count"]:
        raise ValueError("Classroom captures incorrectly labelled original or missing pages")
    if not all(p["source_sha256_unchanged"] and p["retained_pixels_equal_source"]
               and p["all_removed_pixels_white"] and p["rendered_page_visually_reviewed"]
               for p in classroom["pages"]):
        raise ValueError("Classroom captures missing source-fidelity/privacy verification")
    if classroom["verification"]["embedded_source_png_stream_count"] != 0:
        raise ValueError("Private original capture stream must not remain embedded")
    return {"local_notes": len(notes["files"]), "anonymous_reports": len(reports["files"]),
            "anonymous_report_pages": sum(r["page_count"] for r in reports["files"]),
            "classroom_capture_pages": classroom["page_count"]}


def main() -> int:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    seen = set()
    code_count = 0
    generated_count = 0
    for entry in manifest["files"]:
        name = entry["path"]
        path = within_repo(ROOT / name)
        if not name.startswith("source_archive/") or name in seen:
            raise ValueError(f"Invalid or duplicate archive path: {name}")
        seen.add(name)
        raw = path.read_bytes()
        if len(raw) != entry["size"] or digest(raw) != entry["archived_sha256"]:
            raise ValueError(f"Archive hash or size mismatch: {name}")
        if not re.fullmatch(r"[0-9a-f]{64}", entry["source_sha256"]):
            raise ValueError(f"Invalid source hash: {name}")
        if entry["status"] not in STATUSES:
            raise ValueError(f"Unknown preservation status: {name}")
        if not entry["content_transformed"] and entry["source_sha256"] != entry["archived_sha256"]:
            raise ValueError(f"Incorrect unchanged-content flag: {name}")
        if re.search(r"(?<!\d)\d{9}(?!\d)", name + entry["source_filename_sanitized"]):
            raise ValueError(f"Unanonymized source filename: {name}")
        if re.search(rb"[A-Za-z]:[\\/]", raw):
            raise ValueError(f"Local absolute path remains in archive: {name}")
        if path.suffix.lower() in {".exe", ".obj", ".o", ".bin", ".depend", ".layout"}:
            raise ValueError(f"Regenerable build/IDE artifact was archived: {name}")
        if path.suffix.lower() in {".cpp", ".h"}:
            code_count += 1
            if nonliteral_digest(raw) != entry["original_cpp_nonliteral_token_sha256"]:
                raise ValueError(f"Nonliteral source tokens differ: {name}")
            for reference in re.findall(rb'^\s*#\s*include\s*"([^"\r\n]+)"', raw, re.M):
                target = within_repo(path.parent / reference.decode("ascii"))
                if not target.is_file():
                    raise ValueError(f"Missing preserved include in {name}: {reference!r}")
        elif path.suffix.lower() == ".cbp":
            for unit in ET.fromstring(raw).findall(".//Unit"):
                target = within_repo(path.parent / unit.attrib["filename"])
                if not target.is_file():
                    raise ValueError(f"Missing project source in {name}: {target}")
        generated_count += entry["status"] == "program_generated_local_output"
    for dependency in manifest["external_preserved_dependencies"]:
        path = within_repo(ROOT / dependency["path"])
        if digest(path.read_bytes()) != dependency["sha256"]:
            raise ValueError(f"Preserved instructor dependency changed: {dependency['path']}")
    learning = check_learning_materials()
    print(json.dumps({"archive_files": len(seen), "source_or_header_files": code_count,
                      "historical_generated_outputs": generated_count,
                      "learning_materials": learning,
                      "preservation_check": "passed",
                      "assignment_functional_validation": "not_performed"}))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (ValueError, KeyError, OSError) as exc:
        print(f"Source archive preservation check failed: {exc}", file=sys.stderr)
        raise SystemExit(1)
