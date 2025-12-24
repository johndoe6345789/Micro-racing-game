#!/usr/bin/env python3
from __future__ import annotations

import argparse
import importlib
import importlib.util
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Dict, Iterable, List

RECOMMENDED_ACTION_VERSIONS: Dict[str, str] = {
    "actions/checkout": "v4",
    "actions/setup-python": "v5",
    "actions/upload-artifact": "v4",
    "docker/setup-qemu-action": "v3",
}

FLOATING_VERSION_TOKENS = {"x", "latest", "*"}


@dataclass
class StepFinding:
    step_name: str
    message: str


@dataclass
class JobReport:
    name: str
    runs_on: str
    matrix: List[str]
    has_permissions: bool
    warnings: List[str] = field(default_factory=list)
    step_findings: List[StepFinding] = field(default_factory=list)


@dataclass
class WorkflowReport:
    path: Path
    name: str
    triggers: List[str]
    has_permissions: bool
    jobs: List[JobReport]
    parse_errors: List[str] = field(default_factory=list)


class WorkflowDiagnoser:
    def __init__(self, workflows_dir: Path) -> None:
        self.workflows_dir = workflows_dir
        self.yaml = self._ensure_yaml()

    @staticmethod
    def _ensure_yaml():
        spec = importlib.util.find_spec("yaml")
        if spec is None:
            print("PyYAML is missing; attempting installation...", file=sys.stderr)
            subprocess.check_call([sys.executable, "-m", "pip", "install", "pyyaml"])
            spec = importlib.util.find_spec("yaml")
            if spec is None:
                raise RuntimeError("PyYAML not available even after installation.")
        return importlib.import_module("yaml")

    def load_workflow_files(self) -> Iterable[Path]:
        return sorted(self.workflows_dir.glob("*.yml")) + sorted(self.workflows_dir.glob("*.yaml"))

    def diagnose(self) -> List[WorkflowReport]:
        reports: List[WorkflowReport] = []
        for path in self.load_workflow_files():
            reports.append(self._diagnose_workflow(path))
        return reports

    def _diagnose_workflow(self, path: Path) -> WorkflowReport:
        content = path.read_text()
        parse_errors: List[str] = []
        try:
            data = self.yaml.safe_load(content) or {}
        except Exception as exc:  # noqa: BLE001
            data = {}
            parse_errors.append(f"Failed to parse YAML: {exc}")

        name = data.get("name") or path.stem
        on_section = data.get("on")
        if on_section is None and True in data:
            on_section = data[True]
            parse_errors.append("Workflow key 'on' parsed as boolean; quote it to avoid YAML 1.1 pitfalls.")

        triggers = self._extract_triggers(on_section)
        has_permissions = "permissions" in data
        jobs_data = data.get("jobs", {}) if isinstance(data.get("jobs"), dict) else {}
        jobs = [self._diagnose_job(job_name, job_body) for job_name, job_body in jobs_data.items()]

        return WorkflowReport(
            path=path,
            name=name,
            triggers=triggers,
            has_permissions=has_permissions,
            jobs=jobs,
            parse_errors=parse_errors,
        )

    @staticmethod
    def _extract_triggers(on_section: Any) -> List[str]:
        if isinstance(on_section, dict):
            return list(on_section.keys())
        if isinstance(on_section, list):
            return [str(item) for item in on_section]
        if isinstance(on_section, str):
            return [on_section]
        return []

    def _diagnose_job(self, name: str, body: Dict[str, Any]) -> JobReport:
        runs_on = str(body.get("runs-on", "unknown"))
        has_permissions = "permissions" in body
        matrix = self._extract_matrix_axes(body.get("strategy", {}))
        warnings: List[str] = []

        steps = body.get("steps", []) if isinstance(body.get("steps"), list) else []
        step_findings: List[StepFinding] = []
        for step in steps:
            step_name = str(step.get("name") or step.get("id") or step.get("uses") or "Unnamed step")
            step_findings.extend(self._inspect_step(step_name, step))

        if not has_permissions:
            warnings.append("Job is missing explicit permissions; consider defining least privilege permissions.")

        return JobReport(name=name, runs_on=runs_on, matrix=matrix, has_permissions=has_permissions, warnings=warnings, step_findings=step_findings)

    @staticmethod
    def _extract_matrix_axes(strategy: Dict[str, Any]) -> List[str]:
        matrix = strategy.get("matrix") if isinstance(strategy, dict) else None
        if not isinstance(matrix, dict):
            return []
        axes: List[str] = []
        for axis_name, axis_values in matrix.items():
            if axis_name == "include":
                continue
            if isinstance(axis_values, list):
                axes.append(f"{axis_name}={len(axis_values)} value(s)")
        return axes

    def _inspect_step(self, step_name: str, step: Dict[str, Any]) -> List[StepFinding]:
        findings: List[StepFinding] = []
        uses = step.get("uses")
        with_section = step.get("with", {}) if isinstance(step.get("with"), dict) else {}

        if isinstance(uses, str):
            findings.extend(self._check_action_version(step_name, uses))
            findings.extend(self._check_action_specifics(step_name, uses, with_section))

        return findings

    def _check_action_version(self, step_name: str, uses: str) -> List[StepFinding]:
        findings: List[StepFinding] = []
        if "@" not in uses:
            findings.append(StepFinding(step_name, "Action is not version pinned; use a tagged release or commit SHA."))
            return findings

        action_name, version = uses.split("@", 1)
        if version.lower() in {"master", "main", "head", "latest"} or version.endswith("-latest"):
            findings.append(StepFinding(step_name, f"Action {action_name} uses floating ref '{version}'. Prefer a stable tag."))

        recommended = RECOMMENDED_ACTION_VERSIONS.get(action_name)
        if recommended and not version.startswith(recommended):
            findings.append(StepFinding(step_name, f"Upgrade {action_name} to {recommended} for the latest security and features."))

        return findings

    def _check_action_specifics(self, step_name: str, uses: str, with_section: Dict[str, Any]) -> List[StepFinding]:
        findings: List[StepFinding] = []
        normalized = uses.split("@", 1)[0]

        if normalized == "actions/setup-python":
            python_version = str(with_section.get("python-version", ""))
            if any(token in python_version for token in FLOATING_VERSION_TOKENS):
                findings.append(
                    StepFinding(
                        step_name,
                        "Python version is floating; pin to a specific minor version to avoid surprise upgrades.",
                    )
                )

        if normalized == "actions/upload-artifact":
            if "retention-days" not in with_section:
                findings.append(
                    StepFinding(
                        step_name,
                        "Upload-artifact retention is not set; define retention-days to control storage costs.",
                    )
                )

        return findings


def format_report(reports: Iterable[WorkflowReport]) -> str:
    lines: List[str] = []
    for report in reports:
        lines.append(f"Workflow: {report.name} ({report.path})")
        if report.parse_errors:
            lines.extend([f"  PARSE ISSUE: {err}" for err in report.parse_errors])

        triggers = ", ".join(report.triggers) if report.triggers else "<none>"
        lines.append(f"  Triggers: {triggers}")
        perm_text = "present" if report.has_permissions else "missing"
        lines.append(f"  Top-level permissions: {perm_text}")

        for job in report.jobs:
            lines.append(f"  Job: {job.name}")
            lines.append(f"    runs-on: {job.runs_on}")
            if job.matrix:
                lines.append(f"    matrix: {', '.join(job.matrix)}")
            perm_label = "present" if job.has_permissions else "missing"
            lines.append(f"    permissions: {perm_label}")
            for warning in job.warnings:
                lines.append(f"    warning: {warning}")
            for finding in job.step_findings:
                lines.append(f"    step finding: {finding.step_name} -> {finding.message}")
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description="Diagnose GitHub Actions workflow risks and common issues.")
    parser.add_argument(
        "--workflows-dir",
        type=Path,
        default=Path(".github/workflows"),
        help="Directory containing workflow YAML files.",
    )
    args = parser.parse_args()

    diagnoser = WorkflowDiagnoser(args.workflows_dir)
    reports = diagnoser.diagnose()

    output = format_report(reports)
    sys.stdout.write(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
