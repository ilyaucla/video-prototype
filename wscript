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

    ropt.add_option('--with-examples', action='store_true', default=False, dest='with_examples',
                    help='''build examples''')

def configure(conf):
    conf.load("compiler_c compiler_cxx gnu_dirs boost default-compiler-flags")

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)
		
    conf.check_cfg(package='gstreamer-1.0', args=['--cflags', '--libs'], 
         uselib_store='GSTREAMER', mandatory=True) 

    conf.env.LIB_PTHREAD = 'pthread'

    if conf.options.with_tests:
        conf.env['WITH_TESTS'] = True

    if conf.options.with_examples:
        conf.env['WITH_EXAMPLES'] = True

    USED_BOOST_LIBS = ['system', 'iostreams', 'filesystem', 'random']
#   ['system', 'filesystem', 'date_time', 'iostreams',
#                      'regex', 'program_options', 'chrono', 'random']
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

#    bld(target="ndn-next-ndnvideo",
#        name="ndn-repo-objects",
#        features=["cxx", "cxxprogram"],
#        source=bld.path.ant_glob(['src/**/*.cpp']),
#        use='NDN_CXX BOOST GSTREAMER',
#        includes="src",
#        export_includes="src",
#        )
  
#    bld(target="producer_e",
#        features=["cxx", "cxxprogram"],
#        source= "src/producer_e.cpp src/producer-callback.cpp",
#        use='BOOST NDN_CXX PTHREAD',
#        )
#
#    bld(target="file_pro",
#        features=["cxx", "cxxprogram"],
#        source= "src/file_pro.cpp src/producer-callback.cpp",
#        use='BOOST NDN_CXX',
#        )

    bld(target="producer",
        features=["cxx", "cxxprogram"],
        source= "src/producer.cpp src/video-generator.cpp src/producer-callback.cpp",
        use='GSTREAMER BOOST NDN_CXX PTHREAD',
        )

#    bld(target="consumer_e",
#        features=["cxx", "cxxprogram"],
#        source= "src/consumer_e.cpp src/consumer-callback.cpp src/video-player.cpp",
#        use='GSTREAMER BOOST NDN_CXX PTHREAD',
#        )
      
    bld(target="consumer",
        features=["cxx", "cxxprogram"],
        source= "src/consumer.cpp src/video-player.cpp src/consumer-callback.cpp",
        use='GSTREAMER BOOST NDN_CXX PTHREAD',
        )
      
#    bld(target="file_con",
#        features=["cxx", "cxxprogram"],
#        source= "src/file_con.cpp src/video-player.cpp src/consumer-callback.cpp",
#        use='PTHREAD GSTREAMER BOOST NDN_CXX',
#        )
#
#    bld(target="test",
#        features=["cxx", "cxxprogram"],
#        source= "src/test.cpp",
#        args = ['-pthread'],
#        )

    # Tests
    if bld.env['WITH_TESTS']:
	  bld.recurse('tests')

    # Examples
    if bld.env['WITH_EXAMPLES']:
	  bld.recurse('examples')

    bld.install_files('${SYSCONFDIR}/ndn', 'next-ndnvideo.conf.sample')
