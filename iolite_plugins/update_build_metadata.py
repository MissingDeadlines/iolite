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
    {"name": "voxel_editing", "filename": "IoliteVoxelEditingPlugin"}
]
platforms = [("windows", "dll"), ("linux", "so")]


for plat in platforms:
  for p in plugins:
      dll_filepath = "{}/{}.{}".format(plat[0], p["filename"], plat[1])
      json_filepath = "{}/plugins.json".format(plat[0])

      if os.path.isfile(dll_filepath):
        with open(dll_filepath, "rb") as plugin:
            plugin_data = plugin.read()
            checksum = hash_djb2(plugin_data)
            checksum_with_salt = hash_djb2(bytes("{}{}".format(checksum, sys.argv[1]), "ascii"))
            p["checksum"] = checksum_with_salt

  with open(json_filepath, "w") as plugins_json:
      json.dump(plugins, plugins_json, indent=2)
