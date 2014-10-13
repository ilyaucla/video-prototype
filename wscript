# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-
VERSION = '0.1'
APPNAME = 'next-ndnvideo'

from waflib import Build, Logs, Utils, Task, TaskGen, Configure

def options(opt):
    opt.load('compiler_c compiler_cxx gnu_dirs')
    opt.load('boost default-compiler-flags doxygen', tooldir=['.waf-tools'])

    ropt = opt.add_option_group('ndn-next-ndnvideo Options')

    ropt.add_option('--with-tests', action='store_true', default=False, dest='with_tests',
                    help='''build unit tests''')

def configure(conf):
    conf.load("compiler_c compiler_cxx gnu_dirs boost default-compiler-flags")

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

    if conf.options.with_tests:
        conf.env['WITH_TESTS'] = True

    USED_BOOST_LIBS = ['system', 'iostreams', 'filesystem', 'random']
    if conf.env['WITH_TESTS']:
        USED_BOOST_LIBS += ['unit_test_framework']
    conf.check_boost(lib=USED_BOOST_LIBS, mandatory=True)

    try:
        conf.load("doxygen")
    except:
        pass

    conf.define('DEFAULT_CONFIG_FILE', '%s/ndn/next-ndnvideo.conf' % conf.env['SYSCONFDIR'])

    conf.write_config_header('src/config.hpp')

def build(bld):
    bld(target="ndn-next-ndnvideo",
        name="ndn-repo-objects",
        features=["cxx", "cxxprogram"],
        source=bld.path.ant_glob(['src/**/*.cpp']),
        use='NDN_CXX BOOST',
        includes="src",
        export_includes="src",
        )

    
    # Tests
    bld.recurse('tests')

    bld.install_files('${SYSCONFDIR}/ndn', 'next-ndnvideo.conf.sample')
