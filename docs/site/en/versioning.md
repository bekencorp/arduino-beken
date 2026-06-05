# Versioning and Release Channel Notes

## Purpose

This page documents how the online documentation should describe release channels when the site is published through a local or CI-driven Sphinx pipeline.

## Recommended Channel Meanings

Use these meanings consistently:

- `latest`: the current development branch documentation
- `stable`: the most recent tagged release intended for normal users

## What Should Go into `latest`

`latest` is the right place for:

- in-progress targets
- newly added examples
- documentation for features that already landed in the default branch
- staging and packaging behavior that has changed but is not yet part of a released package

## What Should Go into `stable`

`stable` should describe:

- the most recent tagged platform release
- supported targets and workflows that are expected to work for general users
- installation guidance that matches published release artifacts

## Documentation Discipline

When a behavior differs between development and release channels:

- state which channel the page assumes
- avoid mixing unreleased commands into a stable installation path
- prefer documenting release-oriented URLs and package indexes only in `stable`

## Repository Version Source

The documentation configuration currently reads the repository version from the top-level `VERSION` file. That is sufficient for the initial site setup, but hosted release builds should also align tag names, package versions, and documentation channel labels.

## Practical Publishing Rule

If a page depends on unreleased repository state, treat it as `latest` content unless and until the same workflow is represented in a published release artifact.
