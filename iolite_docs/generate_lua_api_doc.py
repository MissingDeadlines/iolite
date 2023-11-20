# MIT License
#
# Copyright (c) 2023 Missing Deadlines (Benjamin Wrensch)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import re
import json

api = []

with open("../iolite_plugins/lua_plugin/init_state.cpp", "r") as f:
    src = f.readlines()

    current_namespace = "Global"
    current_category = {"name": "Global", "description": "Global namespace."}
    current_func = None
    current_type = None
    cats_to_copy = []

    for l in src:
        # print(l)

        namespace = re.search("// @namespace (.*)", l)
        category = re.search("// @category ([\\w_]*) (.*)", l)
        function = re.search("// @function (.*)", l)
        summary = re.search("// @summary (.*)", l)
        param = re.search("// @param (\\w*) ([\\w|]*) (.*)", l)
        ret = re.search("// @return ([\\w|]*) (\\w*) (.*)", l)
        type = re.search("// @type (.*)", l)
        member = re.search("// @member ([\\w|]*) (\\w*) (.*)", l)
        member_simple = re.search("// @member ([\\w|]*) (\\w*)", l)
        hidden = re.search("// @hidden", l)
        copy_category = re.search("// @copy_category (\\w*)", l)

        if namespace:
            # Update the current namespace
            current_namespace = namespace.group(1)
        elif category:
            current_category = {"name": " ".join(category.group(
                1).split("_")), "description": category.group(2)}
        elif copy_category:
            cats_to_copy.append(copy_category.group(1))
        elif hidden:
            current_category["hidden"] = True
        elif type:
            # Start a new type
            current_type = {"name": type.group(1),  "members": []}
        elif function:
            # Start a new function
            current_func = {"name": function.group(
                1), "args": [], "returns": []}
        elif summary:
            if current_func:
                current_func["description"] = summary.group(1)
            elif current_type:
                current_type["description"] = summary.group(1)
        elif param:
            current_func["args"].append(
                {"name": param.group(1), "types": param.group(
                    2).split("|"), "description": param.group(3)}
            )
        elif ret:
            current_func["returns"].append(
                {"name": ret.group(2), "types": ret.group(
                    1).split("|"), "description": ret.group(3)}
            )
        elif member:
            current_type["members"].append(
                {"name": member.group(1), "type": member.group(
                    2), "description": member.group(3)}
            )
        elif member_simple:
            current_type["members"].append(
                {"name": member_simple.group(
                    1), "type": member_simple.group(2)}
            )
        else:
            if current_func or current_type or len(cats_to_copy) > 0:
                # Find a category for the function
                category = None
                for e in api:
                    if e["name"] == current_category["name"] and e["prefix"] == current_namespace:
                        category = e
                        break
                if not category:
                    category = {
                        "name": current_category["name"], "prefix": current_namespace, "description": current_category["description"]}
                    if "hidden" in current_category:
                        category["hidden"] = current_category["hidden"]
                    api.append(category)

                if current_func:
                    if not "functions" in category:
                        category["functions"] = []
                    # Append the function and reset
                    category["functions"].append(current_func)
                    current_func = None
                elif current_type:
                    if not "types" in category:
                        category["types"] = []
                    category["types"].append(current_type)
                    current_type = None

                # Append copied categories
                for name in cats_to_copy:
                    # Find the category
                    cat_to_copy = None
                    for cat in api:
                        if cat["name"] == name:
                            cat_to_copy = cat
                            break

                    if "functions" in cat_to_copy:
                        if not "functions" in category:
                            category["functions"] = []
                        category["functions"] += cat_to_copy["functions"]
                    if "types" in cat_to_copy:
                        if not "types" in category:
                            category["types"] = []
                        category["types"] += cat_to_copy["types"]

                cats_to_copy.clear()

# Sort categories, function, and types
# api = sorted(api, key=lambda x: x["name"])

# Delete hidden categories now
api[:] = [x for x in api if not "hidden" in x or not x["hidden"]]

# for cat in api:

#     if "functions" in cat:
#         cat["functions"] = sorted(cat["functions"], key=lambda x: x["name"])
#     if "types" in cat:
#         cat["types"] = sorted(cat["types"], key=lambda x: x["name"])

# Write out as JSON
with open("api/lua.json", "w") as f:
    f.write(json.dumps({"api": api}, indent=2))

# Generate the actual API documentation
with open("api/lua_generated.rst", "w") as f:
    for cat in api:
        f.write(cat["name"] + "\n" + "-"*len(cat["name"]) + "\n\n")
        if "functions" in cat:
            for function in cat["functions"]:

                arg_string = ', '.join(x["name"] for x in function["args"])
                arg_list = ""
                return_list = ""

                if len(function["args"]) > 0:
                    arg_list = '''   :params:{}'''.format('\n'.join('      - **{}** ({}) - {}'.format(x["name"], " or ".join(
                        ":class:`{}`".format(t) for t in x["types"]), x["description"]) for x in function["args"]))
                if len(function["returns"]) > 0:
                    return_list = '''   :returns:{}'''.format('\n'.join('      - **{}** ({}) - {}'.format(x["name"], " or ".join(
                        ":class:`{}`".format(t) for t in x["types"]), x["description"]) for x in function["returns"]))

                prefix = ""
                if cat["prefix"] != "Global":
                    prefix = cat["prefix"] + "."

                s = '''.. function:: {}{}({})\n\n{}\n\n{}\n\n   {}\n\n'''.format(
                    prefix, function["name"], arg_string, arg_list, return_list, function["description"])
                f.write(s)

        if "types" in cat:
            for type in cat["types"]:

                member_list = '\n'.join('''   :var {}: {}\n'''.format(
                    x["name"], x["type"]) for x in type["members"])
                s = '''.. class:: {}\n\n{}\n\n   {}\n\n'''.format(
                    type["name"], member_list, type["description"])
                f.write(s)
