#! /usr/bin/env python
# encoding: utf-8

from waflib.Tools import waf_unit_test


top = '.'
out = 'build'

subdirs = [
    'tests'
    ]

submoduledirs = [
    'ecell4-bd',
    'ecell4-ode',
    'ecell4-gillespie',
    'ecell4-vis'
    ]

def options(opt):
    opt.load('compiler_cxx waf_unit_test')

def configure(conf):
    conf.load('compiler_cxx waf_unit_test')
    conf.check_cxx(lib='gsl')
    conf.check_cxx(lib='gslcblas')
    conf.check_cxx(lib='m')

    conf.check_cfg(package='cppunit', args='--cflags --libs', mandatory=True)
    if 'dl' not in conf.env.LIB_CPPUNIT:
        l = conf.check(lib='dl', uselib_store='CPPUNIT')

    conf.recurse(subdirs)
    configure_submodule(conf)

def build(bld):
    bld.shlib(
        source = ['Journal.cpp', 'Species.cpp', 'ParticleSpace.cpp',
                  'CompartmentSpace.cpp'],
        includes = ['.'],
        lib = ['gsl', 'gslcblas', 'm'],
        target = 'ecell4-core')

    bld.recurse(subdirs)

    bld.add_post_fun(waf_unit_test.summary)
    bld.options.all_tests = True
    build_submodule(bld)

def configure_submodule(conf):
    import os
    for subm in submoduledirs:
        if os.path.exists(os.path.join(os.getcwd(), subm, '.git')):
            print subm + " detected. build recurse."
            os.system("cd " + subm + "; ./waf configure")

def build_submodule(bld):
    import os
    for subm in submoduledirs:
        if os.path.exists(os.path.join(os.getcwd(), subm, '.git')):
            print subm + " detected. build recurse."
            os.system("cd " + subm + "; ./waf build")
