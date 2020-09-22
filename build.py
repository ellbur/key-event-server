#!/usr/bin/env python3

from scooter import build_main, dirof
from scooter.gcc import build_gcc

here = dirof(__file__)

@build_main(here)
def _(b):
    sources = (
        here/'./main.cpp',
    )
    
    include = (
        '-I' + here,
    )
    
    opts = (
        '-O3',
        '-g',
        '-std=c++17',
        '-Wall',
        '-Wno-unused-variable',
        '-Wno-unused-result',
        '-Werror',
    )
    
    libs = (
    )
    
    cpp = 'g++'
    
    bin_folder = here/'bin'
    bin_folder.mkdirs()
    bin = bin_folder / 'key-event-server'
    
    build_gcc(b, sources, bin, opts + include, gcc=cpp, libs=libs)

