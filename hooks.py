"""MkDocs build hooks.

Keep "About" as the last top-level nav entry. The Material `blog` plugin
appends its generated "Categorie" section to the end of the nav and offers
no way to position it, so we move "About" back to the end afterwards.
"""

from mkdocs.plugins import event_priority


@event_priority(-100)  # run after the blog plugin has attached its views
def on_nav(nav, config, files):
    tail = [item for item in nav.items if getattr(item, "title", None) == "About"]
    for item in tail:
        nav.items.remove(item)
    nav.items.extend(tail)
    return nav
