"""MkDocs build hooks.

Reshape the primary navigation into a single flat list:

    Home
    About
    ----------------  (divider, drawn in CSS)
    AI
    Agile
    Containers
    Java
    Sicurezza Informatica

The Material `blog` plugin appends a collapsible "Categorie" section whose
position can't be configured. We drop that section and splice its category
views in as top-level links, after "About".
"""

from mkdocs.plugins import event_priority

_CATEGORY_SECTION_TITLES = {"Categorie", "Categories"}


@event_priority(-100)  # run after the blog plugin has attached its views
def on_nav(nav, config, files):
    head = []          # Home (everything that isn't About or the category section)
    about = []
    categories = []

    for item in nav.items:
        title = getattr(item, "title", None)
        if title == "About":
            about.append(item)
        elif title in _CATEGORY_SECTION_TITLES and getattr(item, "children", None):
            for child in item.children:
                child.parent = None
                categories.append(child)
        else:
            head.append(item)

    nav.items[:] = head + about + categories
    return nav
