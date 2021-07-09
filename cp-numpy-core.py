
import numpy
from distutils.dir_util import copy_tree
from os.path import join
from os.path import isdir

if __name__ == "__main__":

    search_dir = join(numpy.__path__[0], "core", "include", "numpy")
    numpy_core_dir = search_dir if isdir(search_dir) else None

    if numpy_core_dir is None:
        raise FileExistsError("ERROR! " + search_dir + " not found on " + numpy.__path__[0] + "!")

    dest_dir = join("include", "numpy")
    copy_tree(numpy_core_dir, dest_dir)
    print("SUCCESS! Numpy core found at " + numpy_core_dir + " and copied to " + dest_dir + "!")
