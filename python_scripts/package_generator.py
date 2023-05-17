import lz4.block
import os
import glob
import json
import argparse


def hash_djb2(b):
    h = 5381
    for x in b:
        h = ((h << 5) + h) + x
        h &= 0xFFFFFFFF
    return h


parser = argparse.ArgumentParser(prog="IOLITE Package Generator",
                                 description="Generates an IOLITE Package File (IOPKG) from the provided data source directory.")

parser.add_argument("data_source_path",
                    help="The data source directory to create the package for.")
parser.add_argument("-o", "--output", required=True,
                    help="The filepath of the package file to create. Example: 'base.iopkg'.")

args = parser.parse_args()

data_source_path = args.data_source_path
package_path = args.output

toc = {}
current_byte_offset = 0

try:
    print("Creating package file '{}'...".format(package_path))

    with open(package_path + "_tmp", "wb") as temp_package_file:

        cwd = os.getcwd()
        os.chdir(data_source_path)

        files_to_pack = glob.glob("**/*.*", recursive=True)

        for file_path in files_to_pack:
            with open(file_path, "rb") as file_to_pack:
                file_path = file_path.replace("\\", "/")

                print("  Compressing file '{}'...".format(file_path))

                # Read file
                file_data = file_to_pack.read()
                size_uncompressed = len(file_data)

                # Compress
                file_data_compressed = lz4.block.compress(
                    file_data, store_size=False, mode="high_compression")
                size_compressed = len(file_data_compressed)
                checksum = hash_djb2(file_data_compressed)

                print("Size before: {:.3f} MB".format(size_uncompressed / 1024.0 / 1024.0))
                print("Size compressed: {:.3f} MB".format(size_compressed / 1024.0 / 1024.0))

                # Write to package
                temp_package_file.write(file_data_compressed)

                # Store offset and size in table of contents
                toc[file_path] = {}
                toc[file_path]["size_uncompressed"] = size_uncompressed
                toc[file_path]["size_compressed"] = size_compressed
                toc[file_path]["byte_offset"] = current_byte_offset
                toc[file_path]["checksum"] = checksum

                # Advance byte offset
                current_byte_offset += size_compressed

        os.chdir(cwd)

    toc_data = bytearray(json.dumps(toc), encoding="utf-8")

    with open(package_path, "wb") as package_file:
        with open(package_path + "_tmp", "rb") as temp_package_file:
            toc_size = len(toc_data).to_bytes(4, byteorder="little")
            package_file.write(toc_size)
            package_file.write(toc_data)
            package_file.write(temp_package_file.read())

    # Remove temp. package file
    os.remove(package_path + "_tmp")

    print("Package creation finished!")
except:
    exit(-1)
