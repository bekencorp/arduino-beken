# Documentation contribution guide

This page is for people who edit the Sphinx site under `docs/site/`. For code, examples, and general repository expectations, see {doc}`contributing` first.

## What this site is built with

- **Sphinx** 8 or newer
- **MyST Parser** so pages are mostly Markdown (`.md`) with optional reStructuredText directives
- **Read the Docs Sphinx Theme** (`sphinx_rtd_theme`), configured in `docs/site/conf.py`

Python dependencies are pinned in `docs/site/requirements.txt`.

## Repository layout

| Path | Role |
| --- | --- |
| `docs/site/` | English Markdown sources and shared Sphinx config |
| `docs/site/zh_CN/` | Simplified Chinese sources (mirror the English tree where a page exists) |
| `docs/site/conf.py` | Shared `conf.py`; language is selected with `DOCS_LANGUAGE` |
| `docs/site/_templates/` | Theme overrides (for example the language switcher) |
| `docs/site/_static/` | Static assets referenced from the theme or pages |

The English build uses `docs/site/` as the source root and **excludes** `zh_CN/**`. The Chinese build uses `docs/site/zh_CN/` as the source root with the same `conf.py`.

## Add or rename a page

1. Create or edit the `.md` file under `docs/site/` (and under `docs/site/zh_CN/` if the page should appear in both languages).
2. Add the document to the Sphinx navigation by editing the `toctree` sections in `docs/site/index.md` and, for Chinese, `docs/site/zh_CN/index.md`, keeping the same order as sibling pages.
3. Prefer **one topic per pull request** for doc-only changes so reviewers can follow the narrative.

If you only maintain one language, say so in the pull request; another contributor can mirror the other tree.

## Authoring conventions (MyST / Sphinx)

- Use `#` headings; the site enables `myst_heading_anchors` so deep links to headings work.
- Cross-link other pages with the MyST `{doc}` role; the target is the document path **without** the `.md` suffix, for example:

  ```text
  {doc}`getting_started/installation`
  ```
- Use fenced code blocks with a language tag where it helps (`bash`, `text`, and so on).
- Keep tone aligned with existing guides: task-oriented, repository-specific, minimal generic Arduino tutorial material.

## Build the site locally

From the **repository root** (`arduino-beken/`):

```bash
make docs-install   # once: creates .venv-docs and installs Sphinx stack
make docs-html      # writes English + zh_CN into build/docs/html/
```

Open `build/docs/html/index.html` for English and `build/docs/html/zh_CN/index.html` for Chinese.

The Makefile runs `sphinx-build` twice: once with `DOCS_LANGUAGE=en` and once with `DOCS_LANGUAGE=zh_CN`. If something fails only for one language, check `conf.py` for `exclude_patterns` and the corresponding source tree.

## Theme and “Edit on GitLab”

`conf.py` sets `html_context` for GitLab host, user, repo, and branch. CI or hosting can override these with environment variables (`DOCS_GITLAB_HOST`, `DOCS_GITLAB_USER`, `DOCS_GITLAB_REPO`, `DOCS_GITLAB_VERSION`). When adjusting edit links or the language switcher, inspect `docs/site/_templates/layout.html` together with `conf.py`.

## Dependencies

Bump versions only when needed (security, compatibility, or a feature you rely on). After changing `docs/site/requirements.txt`, run `make docs-install` on a clean or upgraded venv and confirm `make docs-html` succeeds.

## What reviewers usually check

- Navigation: new pages appear in `index.md` / `zh_CN/index.md` where readers expect them.
- Both languages stay in sync when the content is meant for both audiences.
- Commands and paths match the current `Makefile` and `AGENTS.md` workflow descriptions.
- Release channel language (`latest` vs `stable`) stays consistent with {doc}`versioning`.
