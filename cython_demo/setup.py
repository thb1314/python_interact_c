from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import glob
import os

pyx_filelist = glob.glob(os.path.join(os.path.realpath("./"), "**", "*.pyx"), recursive=True)

extension_name = "sum_py"
ext_modules = [
    Extension(extension_name,pyx_filelist)
]

setup(
    name = "Hello pyx",
    version="1.0",
    cmdclass = {'install': build_ext},
    ext_modules = ext_modules
)