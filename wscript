#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import sys
import platform

APPNAME = 'essentia'
VERSION = '2.0-dev'

top = '.'
out = 'build'


def options(ctx):
    ctx.load('compiler_cxx compiler_c')
    ctx.recurse('src')

    ctx.add_option('--with-cpptests', action='store_true',
                   dest='WITH_CPPTESTS', default=False,
                   help='build the c++ tests')

    ctx.add_option('--mode', action='store',
                   dest='MODE', default="release",
                   help='debug or release')

    ctx.add_option('--arch', action='store',
                   dest='ARCH', default="x64",
                   help='i386, x64 or FAT')



def configure(ctx):
    print('→ configuring the project in ' + ctx.path.abspath())

    ctx.env.WITH_CPPTESTS = ctx.options.WITH_CPPTESTS

    # compiler flags
    ctx.env.CXXFLAGS = [ '-pipe', '-Wall' ]

    # define this to be stricter, but sometimes some libraries can give problems...
    #ctx.env.CXXFLAGS += [ '-Werror' ]

    if ctx.options.MODE == 'debug':
        print ('→ Building in debug mode')
        ctx.env.CXXFLAGS += [ '-g' ]

    elif ctx.options.MODE == 'release':
        print ('→ Building in release mode')
        ctx.env.CXXFLAGS += [ '-O2' ] # '-march=native' ] # '-msse3', '-mfpmath=sse' ]

    else:
        raise ValueError('mode should be either "debug" or "release"')


    # required if we want to use libessentia.a to be linked in the python bindings
    # (dynamic library, needs -fPIC)
    ctx.env.CXXFLAGS += [ '-fPIC' ]

    # global defines
    ctx.env.DEFINES = []

    if sys.platform == 'darwin':
        # force the use of clang as compiler, we don't want gcc anymore on mac
        ctx.env.CC = 'clang'
        ctx.env.CXX = 'clang++'


        ctx.env.DEFINES   += [ 'GTEST_HAS_TR1_TUPLE=0' ]
        ctx.env.CXXFLAGS = [ '-stdlib=libc++', '-std=c++11', '-Wno-gnu' ]
        ctx.env.LINKFLAGS = [ '-stdlib=libc++' ]
        # for defining static const variables in header
        ctx.env.CXXFLAGS += [ '-Wno-static-float-init' ]
        # add /usr/local/include as the brew formula for yaml doesn't have
        # the cflags properly set
        ctx.env.CXXFLAGS += [ '-I/usr/local/include' ]

        if ctx.options.ARCH == 'i386':
            ctx.env.CXXFLAGS += [ '-arch' , 'i386']
            ctx.env.LINKFLAGS += [ '-arch', 'i386']
            ctx.env.LDFLAGS = ['-arch', 'i386']
        if ctx.options.ARCH == 'FAT':
            ctx.env.CXXFLAGS += [ '-arch' , 'i386', '-arch', 'x86_64']
            ctx.env.LINKFLAGS += [ '-arch' , 'i386', '-arch', 'x86_64']
            ctx.env.LDFLAGS = [ '-arch' , 'i386', '-arch', 'x86_64']
            
    elif sys.platform == 'win32': 
        # compile libgcc and libstd statically when using MinGW
        ctx.env.CXXFLAGS = [ '-static-libgcc', '-static-libstdc++' ]
        
        # make pkgconfig find 3rdparty libraries in packaging/win32_3rdparty
        libs_3rdparty = ['yaml-0.1.5', 'fftw-3.3.3', 'libav-0.8.9', 'libsamplerate-0.1.8']
        libs_paths = [';packaging\win32_3rdparty\\' + lib + '\lib\pkgconfig' for lib in libs_3rdparty]
        os.environ["PKG_CONFIG_PATH"] = ';'.join(libs_paths)
        
        # TODO why this code does not work?
        # force the use of mingw gcc compiler instead of msvc
        #ctx.env.CC = 'gcc'
        #ctx.env.CXX = 'g++'

    ctx.load('compiler_cxx compiler_c')

    ctx.recurse('src')


def adjust(objs, path):
    return [ '%s/%s' % (path, obj) for obj in objs ]

def build(ctx):
    print('→ building from ' + ctx.path.abspath())
    ctx.recurse('src')

    if ctx.env.WITH_CPPTESTS:
        # missing -lpthread flag on Ubuntu
        if platform.dist()[0] == 'Ubuntu':
            ext_paths = ['/usr/lib/i386-linux-gnu', '/usr/lib/x86_64-linux-gnu']
            ctx.read_shlib('pthread', paths=ext_paths)
            ctx.env.USES += ' pthread'

        ctx.program(
            source   = ctx.path.ant_glob('test/src/basetest/*.cpp test/3rdparty/gtest-1.6.0/src/gtest-all.cc '),
            target   = 'basetest',
            includes = [ 'test/3rdparty/gtest-1.6.0/include',
                         'test/3rdparty/gtest-1.6.0' ] + adjust(ctx.env.INCLUDES, 'src'),
            install_path = None,
            use      = 'essentia ' + ctx.env.USES
            )

def run_tests(ctx):
    os.system(out + '/basetest')

def run_python_tests(ctx):
    os.system('python test/src/unittest/all_tests.py')

def ipython(ctx):
    os.system('ipython --pylab')

def doc(ctx):
    os.system('doc/build_sphinx_doc.sh')
