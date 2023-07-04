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

from PIL import Image

from math import floor
from glob import glob
import argparse
import os.path

# Set up command line parser
parser = argparse.ArgumentParser(prog="IOLITE Heightmap Generator",
                                 description="Generates a combined heightmap from multiple height, splat and grass maps usable for generating terrain in IOLITE.")

parser.add_argument(
    "dir_path", help="The directory containing the maps to combine.")
parser.add_argument("-o", "--output", default="heightmap.png",
                    help="The path of the combined heightmap to genrate.")
parser.add_argument("--height_pattern", default="height_*",
                    help="The file pattern for heightmaps, defaults to 'height_*'")
parser.add_argument("--splat_pattern", default="splat_*",
                    help="The file pattern for splat maps, defaults to 'splat_*'.")
parser.add_argument("--grass_pattern", default="grass_*",
                    help="The file pattern for grass maps, defaults to 'grass_*'.")
parser.add_argument("--max_grass_height", type=int, default=8,
                    help="The maximum height of grass (in voxels). Defaults to '8'.")
parser.add_argument("--num_sub_materials", type=int, default=8,
                    help="The number of sub materials. A sub material number of, e.g., 8 picks a random material from the first 8 materials in the voxel palette for the first splat map. Defaults to '8'.")
parser.add_argument("--grass_material_mask", type=int, nargs="*", default=[],
                    help="A list of material indexes (e.g. index 0 matches the first splat mask) which should be used to mask out grass.")
parser.add_argument("--max_grass_density", type=float, default=0.5,
                    help="The maximum density of grass. A value of 0.5 limits grass to 50%% of the area. Defaults to 0.5.")

args = parser.parse_args()

blue_noise = Image.open("blue_noise.png")
blue_noise = blue_noise.convert('RGB')
blue_noise_size = blue_noise.size
blue_noise = blue_noise.load()

os.chdir(args.dir_path)

paths_heightmaps = glob(args.height_pattern)
paths_splatmaps = glob(args.splat_pattern)
paths_grassmaps = glob(args.grass_pattern)

print("Heightmaps: {}".format(paths_heightmaps))
print("Splatmaps: {}".format(paths_splatmaps))
print("Grassmaps: {}".format(paths_grassmaps))

dim = [0, 0]


def load_images(paths):
    global dim
    images = []
    for s in paths:
        i = Image.open(s)
        dim = i.size
        images.append(i.convert('L').load())
    return images


def max_of_images(imgs, x, y):
    v = 0
    for i in imgs:
        v = max(i[x, y], v)
    return v


height_maps = load_images(paths_heightmaps)

if len(height_maps) == 0:
    print("Please provide at least one heightmap.")
    exit()

splat_maps = load_images(paths_splatmaps)
grass_maps = load_images(paths_grassmaps)


def halton(base, idx):
    result = 0.0
    frac = 1.0 / base
    i = idx + 1

    while i > 0:
        result += frac * (i % base)
        i = floor(float(i) / base)
        frac = frac / base

    return result


halton23 = []

for i in range(0, len(splat_maps)):
    halton23.append((halton(2, i), halton(3, i)))


def sample_noise(x, y, idx):
    offset = (halton23[idx][0] * blue_noise_size[0],
              halton23[idx][1] * blue_noise_size[1])
    return blue_noise[(x + offset[0]) % blue_noise_size[0], (y + offset[1]) % blue_noise_size[1]]


def combine_splats(splats, x, y):
    mat_idx = 0
    mat = 0
    for s in splats:
        thresh = sample_noise(x, y, mat_idx)[0] / 255.0

        sp = s[x, y]
        prob = sp / 255.0
        if prob > 0 and prob >= thresh:
            mat = mat_idx
        mat_idx = mat_idx + 1

    return mat


def calc_grass(grass_maps, grass_material_mask, material, x, y):
    g = 0

    if len(grass_material_mask) == 0 or not material in grass_material_mask:
        g = max_of_images(grass_maps, x, y)

        density = g / 255.0

        # Random value based on blue noise
        noise = sample_noise(x, y, 0)
        rand0 = noise[0] / 255.0
        rand1 = noise[1] / 255.0

        # Max spawn rate of 50%
        maxDensity = args.max_grass_density

        # Compute probability based on rand
        prob = 1.0 - density * maxDensity

        if rand0 <= prob:
            g = 0
        else:
            # Apply randomness to density
            density = density * rand1
            # Write out the height of the grass
            g = 1 + int(density * (args.max_grass_height - 1))

    return g


def apply_noise_to_material(material, x, y):
    rand = sample_noise(x, y, 0)[2]

    return material * args.num_sub_materials + rand % args.num_sub_materials


# Create target image
target = Image.new('RGB', dim)

for y in range(0, target.height):
    for x in range(0, target.width):

        height = max_of_images(height_maps, x, y)
        material = combine_splats(splat_maps, x, y)
        grass = calc_grass(
            grass_maps, args.grass_material_mask, material, x, y)
        material = apply_noise_to_material(material, x, y)

        target.putpixel((x, y), (height, grass, material))

target.save(args.output)
