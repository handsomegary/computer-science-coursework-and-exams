"""Verify saved original companions, instruction references and declared gaps.

Uses only the Python standard library. A successful integrity check does not
claim that an explicitly unavailable original file has been supplied.
"""
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "assignments/document-companions.json"

STATUSES = {
    "preserved_original", "student_submission_requirement", "intentional_missing_filename",
    "illustrative_input_name", "generated_output_with_preserved_public_examples",
    "generated_or_illustrated_file", "student_source_or_compile_command_placeholder",
    "file_extension_description",
}


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def file_path(name):
    path = (ROOT / name).resolve()
    require(path.is_relative_to(ROOT) and path.is_file(), "Missing or unsafe path: " + name)
    return path


def check_bytes(row):
    path = file_path(row["path"])
    require(hashlib.sha256(path.read_bytes()).hexdigest() == row["sha256"], "SHA256 differs: " + row["path"])
    if "size" in row:
        require(path.stat().st_size == row["size"], "Size differs: " + row["path"])


def main():
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    require(manifest["schema_version"] == 1, "Unsupported manifest schema")
    require(manifest["instruction_version_policy"] == "latest_only_per_course_year_assignment_and_document_type",
            "Instruction documents must use the latest-only publication policy")
    specifications = json.loads((ROOT / "assignments/specifications.json").read_text(encoding="utf-8"))
    require(specifications["instruction_version_policy"] == "latest_only", "Superseded specifications must not be published")
    for specification in specifications["assignments"]:
        require(specification["archive"] == [], "Old instruction PDFs must not be archived in the repository")
        for old in specification["superseded_not_published"]:
            require(not any(ROOT.rglob(old["filename"])), "Superseded instruction PDF has been restored: " + old["filename"])
    groups = manifest["assignments"]
    require(len(groups) == 10 and len({row["assignment"] for row in groups}) == 10, "Expected ten distinct assignments")
    files = manifest["files"]
    require(len(files) == manifest["file_count"], "Companion file count differs")
    known = {row["path"]: row for row in files}
    require(len(known) == len(files), "Duplicate companion path")
    documents = {}
    for group in groups:
        require(group["status"] in ("all_locally_supplied_companions_preserved", "no_separate_official_companion_provided"),
                "Unresolved group status: " + group["assignment"])
        file_path(group["current_specification"])
        mains = [row["path"] for row in group["instruction_documents"] if row["role"] == "main_instruction"]
        require(mains == [group["current_specification"]], "Expected only the latest main instruction: " + group["assignment"])
        for path in group["companion_paths"]:
            require(path in known and known[path]["assignment"] == group["assignment"], "Uncatalogued companion: " + path)
        for row in group["instruction_documents"]:
            check_bytes(row)
            documents[row["path"]] = row
            limit = row.get("pages", row.get("paragraphs", row.get("lines", 0)))
            require(limit > 0, "No instruction pages/paragraphs: " + row["path"])
            for reference in row["file_references"]:
                location = reference.get("page", reference.get("paragraph", reference.get("line", 0)))
                require(1 <= location <= limit, "Invalid instruction reference location: " + row["path"])
        referenced_names = {ref["name"] for row in group["instruction_documents"] for ref in row["file_references"]}
        require(referenced_names == {row["name"] for row in group["reference_resolutions"]}, "Unresolved filename inventory: " + group["assignment"])
        for resolution in group["reference_resolutions"]:
            require(resolution["status"] in STATUSES, "Unresolved reference: " + resolution["name"])
            for path in resolution.get("paths", []):
                file_path(path)
    def evidence(reference):
        require(reference["document"] in documents, "Uncatalogued evidence document: " + reference["document"])
        row = documents[reference["document"]]
        location = reference.get("page", reference.get("paragraph", reference.get("line", 0)))
        require(1 <= location <= row.get("pages", row.get("paragraphs", row.get("lines", 0))), "Invalid evidence location")
        require(bool(reference.get("quote")), "Empty evidence quote")
        if "line" in reference:
            lines = file_path(reference["document"]).read_text(encoding="utf-8").splitlines()
            require(reference["quote"] in lines[location - 1], "Release-log quote differs at recorded line")
        if "paragraph" in reference:
            with zipfile.ZipFile(file_path(reference["document"])) as archive:
                xml = ET.fromstring(archive.read("word/document.xml"))
            ns = {"w": "http://schemas.openxmlformats.org/wordprocessingml/2006/main"}
            paragraphs = ["".join(t.text or "" for t in p.findall(".//w:t", ns)) for p in xml.findall(".//w:p", ns)]
            require(reference["quote"] in paragraphs[location - 1], "Announcement quote differs at recorded paragraph")
    for row in files:
        check_bytes(row)
        require(row["byte_exact"] is True, "Original companion must be byte-exact: " + row["path"])
        require(row["instruction_references"], "No instruction evidence: " + row["path"])
        for reference in row["instruction_references"]:
            evidence(reference)
    for row in manifest["unavailable"]:
        require(row["status"] == "not_provided" and row.get("note"), "Unexplained unavailable original")
        require(not any(Path(item["path"]).name == row["name"] for item in files), "Unavailable item is also preserved")
        for reference in row["instruction_references"]:
            evidence(reference)
        group = next(item for item in groups if item["assignment"] == row["assignment"])
        require(row["name"] in group["unavailable_names"], "Missing group gap declaration")
    for row in manifest["remote_external"]:
        require(row["status"] == "remote_external" and row["required_for_local_rewritten_solution"] is False,
                "Unexplained external course service")
        evidence(row["instruction_reference"])
    print(f"PASS original integrity: {len(files)} companions/archive members and {len(documents)} instruction documents; all filename references classified.")
    for row in manifest["unavailable"]:
        print("NOT PROVIDED: " + row["assignment"] + " / " + row["name"])
    print(f"REMOTE EXTERNAL: {len(manifest['remote_external'])} course video/OJ/optional environment references; none represented as local attachments.")


if __name__ == "__main__":
    main()
