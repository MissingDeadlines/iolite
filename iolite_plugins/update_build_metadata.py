import os
import json
import sys


def hash_djb2(b):
    h = 5381
    for x in b:
        h = ((h << 5) + h) + x
        h &= 0xFFFFFFFF
    return h


plugins = [
    {"name": "terrain", "filename": "IoliteTerrainPlugin"},
    {"name": "lua", "filename": "IoliteLuaPlugin"},
    {"name": "denoiser_oidn", "filename": "IoliteDenoiserOIDNPlugin"},
    {"name": "voxel_editing", "filename": "IoliteVoxelEditingPlugin"},
    {"name": "benchmark", "filename": "IoliteBenchmarkPlugin"}
]
platforms = [("windows", "dll"), ("linux", "so")]

for plat in platforms:
  plugins_filtered = []

  for plugin in plugins:
      dll_filepath = "{}/{}.{}".format(plat[0], plugin["filename"], plat[1])
      json_filepath = "{}/plugins.json".format(plat[0])

      if os.path.isfile(dll_filepath):
        with open(dll_filepath, "rb") as plugin_file:
            plugin_data = plugin_file.read()
            checksum = hash_djb2(plugin_data)
            checksum_with_salt = hash_djb2(bytes("{}{}".format(checksum, sys.argv[1]), "ascii"))

            plugin_with_checksum = plugin.copy()
            plugin_with_checksum["checksum"] = checksum_with_salt
            plugins_filtered.append(plugin_with_checksum)
      else:
        print("Plugin '{}' not found. Skipping checksum generation...".format(dll_filepath))

  # Only write the file if we've found at least one of the plugins
  if len(plugins_filtered) > 0:
    with open(json_filepath, "w") as plugins_json:
        json.dump(plugins_filtered, plugins_json, indent=2)
  else:
    print("No plugins found for platform '{}'. Not writing 'plugins.json' file...".format(plat[0]))
