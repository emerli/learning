# AGENTS.md - Learning Blog

## Overview

Personal technical blog built with MkDocs using the `simple-blog` theme, hosted on GitLab Pages.

- **URL**: https://learning-9b8959.gitlab.io
- **Repo**: https://gitlab.com/ataru76/learning
- **Theme**: [mkdocs-simple-blog](https://github.com/FernandoCelmer/mkdocs-simple-blog)

---

## Project Structure

```
.
├── .gitlab-ci.yml          # CI pipeline for GitLab Pages
├── .gitignore              # Ignores site/ build output
├── mkdocs.yml              # MkDocs configuration
├── docs/
│   ├── index.md            # Homepage
│   ├── about.md            # About page
│   └── blog/
│       ├── index.md        # Blog index with post links
│       └── posts/          # Blog posts (Markdown files)
│           ├── clean-code.md
│           └── principi-solid.md
```

---

## Local Development

```bash
pip install mkdocs mkdocs-simple-blog pymdown-extensions
mkdocs serve
```

Open http://localhost:8000 to preview.

---

## Adding a New Blog Post

1. Create a new `.md` file in `docs/blog/posts/`
2. Add YAML frontmatter:
   ```yaml
   ---
   title: "Post Title"
   date: 2026-04-24
   description: "Short description"
   ---
   ```
3. Update `docs/blog/index.md` with a link to the new post
4. Commit and push — the site deploys automatically

---

## GitLab CI Pipeline

On every push to `main`:
1. Installs dependencies
2. Builds the static site with `mkdocs build`
3. Publishes to GitLab Pages

### Pages Settings (Important!)
- **Project visibility**: Public
- **Pages visibility**: Everyone (with access)
- If changes don't apply, toggle Pages off and on in Settings → Pages

---

## Current Content

| Post | File |
|------|------|
| Clean Code - Sintesi | `docs/blog/posts/clean-code.md` |
| Principi SOLID - Sintesi | `docs/blog/posts/principi-solid.md` |

---

## Known Issues & Fixes

| Issue | Fix |
|-------|-----|
| Pipeline failed: missing markdown extension | Removed unsupported `pymdownx.todo`/`pymdownx.task` |
| Theme requires no blog plugin | Removed `blog` plugin from `mkdocs.yml` |
| Pages requires login | Toggle Pages off/on in Settings → Pages after changing visibility |

---

## Future Ideas

- [ ] Add new technical summaries (OOP, Design Patterns, etc.)
- [ ] Customize theme colors/styles
- [ ] Add custom domain to GitLab Pages

---

**Author**: ataru76
**Created**: 2026-04-24