# 16 — Codex Task Rules

Codex must respect the active development phase before writing code.

---

## Required Workflow

1. Read the relevant documentation for the requested task.
2. Confirm the task belongs to the current phase in [15_DEVELOPMENT_PHASES.md](15_DEVELOPMENT_PHASES.md).
3. Inspect the current repository state before making edits.
4. Modify only the files required for the current phase.
5. Build the affected target.
6. Run the manual verification steps listed for the phase.
7. Report changed files, commands run, and anything intentionally deferred.

---

## Hard Rules

- Do not implement work from a future phase early.
- Maintain the one-way dependency rule: `editor/` may depend on `engine/`, `engine/` must not depend on `editor/`.
- Do not introduce undeclared third-party dependencies.
- Keep Phase 0 limited to repository layout, documentation layout, and build skeleton only.
- If a required document is missing, create it only when that is part of repository setup.

---

## Reporting

Every completed task report must include:

- Files created
- Files modified
- Build commands run
- Manual test result
- Deferred items that belong to later phases
