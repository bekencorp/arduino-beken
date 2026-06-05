from __future__ import annotations

from pathlib import Path
import os


PROJECT_ROOT = Path(__file__).resolve().parents[2]
DOCS_ROOT = Path(__file__).resolve().parent
DOCS_LANGUAGE = os.environ.get("DOCS_LANGUAGE", "en")

project = "arduino-beken"
copyright = "2026, arduino-beken contributors"
author = "arduino-beken contributors"
release = PROJECT_ROOT.joinpath("VERSION").read_text(encoding="utf-8").strip()

extensions = [
    "myst_parser",
    "sphinx.ext.githubpages",
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

if DOCS_LANGUAGE == "zh_CN":
    root_doc = "index"
    html_title = "arduino-beken 文档"
    html_static_path = [str(DOCS_ROOT / "_static")]
    html_css_files = [
        'custom.css',
    ]
    conf_py_path = "/docs/site/zh_CN/"
else:
    root_doc = "index"
    html_title = "arduino-beken documentation"
    exclude_patterns.append("zh_CN/**")
    html_static_path = [str(DOCS_ROOT / "_static")]
    html_css_files = [
        'custom.css',
    ]
    conf_py_path = "/docs/site/en/"

source_suffix = {
    ".md": "markdown",
    ".rst": "restructuredtext",
}

myst_heading_anchors = 3

html_theme = "sphinx_rtd_theme"
html_theme_options = {
    "navigation_depth": 4,
    "collapse_navigation": True,
    "style_external_links": True,
    "vcs_pageview_mode": "edit",
}
html_context = {
    "docs_language": DOCS_LANGUAGE,
    "display_github": True,
    "github_user": "bekencorp",
    "github_repo": "arduino-beken",
    "github_version": "master",
    "conf_py_path": conf_py_path,
    "theme_vcs_pageview_mode": "edit",
}
html_show_sourcelink = False