# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'IOLITE'
copyright = '2023, Missing Deadlines. All Rights Reserved'
author = 'Missing Deadlines (Benjamin Wrensch)'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinxext.opengraph",
    "sphinx_copybutton",
    "sphinx_inline_tabs"
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db',
                    '.DS_Store', 'api/lua_generated.rst']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
html_theme_options = {
}
html_logo = '_static/images/logo.png'
html_static_path = ['_static']

latex_engine = 'xelatex'
