# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'IOLITE - Voxel Game Engine'
copyright = '2023, Missing Deadlines. All Rights Reserved'
author = 'Missing Deadlines (Benjamin Wrensch)'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
  'sphinx_rtd_theme',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store', 'api/lua_generated.rst']

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_options = {
  'logo_only': True, 
  "display_version": False,
  "collapse_navigation": False,
}
html_logo = 'https://media.missing-deadlines.com/iolite/images/iolite_logo2.png'
html_static_path = ['_static']
