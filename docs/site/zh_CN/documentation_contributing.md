# 文档贡献指南

本文面向在 `docs/site/` 下维护 **Sphinx 在线文档** 的贡献者。若你主要改代码、示例或平台打包流程，请先阅读 {doc}`contributing`，再按需回到本页。

## 技术栈

- **Sphinx** 8 及以上
- **MyST Parser**：正文以 Markdown（`.md`）为主，可在文中使用 Sphinx 指令
- **Read the Docs Sphinx Theme**（`sphinx_rtd_theme`），在 `docs/site/conf.py` 中配置

依赖列表见 `docs/site/requirements.txt`。

## 目录约定

| 路径 | 作用 |
| --- | --- |
| `docs/site/` | 英文 Markdown 源码与共享 Sphinx 配置 |
| `docs/site/zh_CN/` | 简体中文源码（与英文树结构对应，有中文页则尽量同步） |
| `docs/site/conf.py` | 共享配置；通过环境变量 `DOCS_LANGUAGE` 区分语言构建 |
| `docs/site/_templates/` | 主题模板覆盖（例如页眉语言切换） |
| `docs/site/_static/` | 静态资源 |

英文构建以 `docs/site/` 为源码根目录，并在配置中排除 `zh_CN/**`。中文构建以 `docs/site/zh_CN/` 为源码根目录，共用同一份 `conf.py`。

## 新增或重命名页面

1. 在 `docs/site/` 下创建或修改 `.md`；若该页应对外提供中文版，在 `docs/site/zh_CN/` 下维护对应文件。
2. 在 `docs/site/index.md` 的 `toctree` 段落中登记文档路径；中文站点在 `docs/site/zh_CN/index.md` 中同样登记，并与同级页面顺序保持一致。
3. 文档类改动尽量 **一次 PR 一个主题**，便于审阅与追溯。

若暂时只能维护一种语言，请在 PR 说明中写明，便于他人补全另一种语言。

## 编写约定（MyST / Sphinx）

- 标题使用 `#` 层级；项目已开启 `myst_heading_anchors`，便于锚点链接。
- 交叉引用使用 MyST 的 `{doc}` 角色；目标为文档路径，**不要**带 `.md` 后缀，例如：

  ```text
  {doc}`getting_started/installation`
  ```
- 代码块尽量带语言标记（如 `bash`、`text`）。
- 文风与现有指南一致：围绕本仓库的真实任务，少写泛化的 Arduino 教程。

## 本地构建

在仓库根目录（`arduino-beken/`）执行：

```bash
make docs-install   # 首次：创建 .venv-docs 并安装 Sphinx 相关依赖
make docs-html      # 生成英文与中文站点到 build/docs/html/
```

浏览器打开 `build/docs/html/index.html`（英文）与 `build/docs/html/zh_CN/index.html`（中文）即可预览。

Makefile 会各运行一次 `sphinx-build`，分别设置 `DOCS_LANGUAGE=en` 与 `DOCS_LANGUAGE=zh_CN`。若只有一种语言失败，请对照 `conf.py` 中的 `exclude_patterns` 与对应源码目录排查。

## 主题与「在 GitLab 上编辑」

`conf.py` 中的 `html_context` 配置了 GitLab 主机、用户、仓库与分支。CI 或托管环境可通过 `DOCS_GITLAB_HOST`、`DOCS_GITLAB_USER`、`DOCS_GITLAB_REPO`、`DOCS_GITLAB_VERSION` 覆盖。修改编辑链接或语言切换行为时，请同时查看 `docs/site/_templates/layout.html` 与 `conf.py`。

## 依赖升级

仅在确有需求时（安全修复、兼容性、依赖的新特性）再调整 `docs/site/requirements.txt` 中的版本。修改后请在干净或升级过的虚拟环境中执行 `make docs-install`，并确认 `make docs-html` 通过。

## 审阅时常见检查项

- 导航：`index.md` / `zh_CN/index.md` 的 toctree 是否包含新页面且位置合理。
- 双语内容若面向同一读者群，中英文是否同步更新。
- 命令与路径是否与当前 `Makefile`、`AGENTS.md` 中的工作流一致。
- 涉及发布渠道时，`latest` / `stable` 的表述是否与 {doc}`versioning` 一致。
