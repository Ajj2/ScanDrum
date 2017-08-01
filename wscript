#!/usr/bin/env python

import sys, os 

def options(opt):
    opt.load('compiler_cxx')
    opt.load('compiler_c')

def configure(conf):
    conf.load('compiler_cxx')
    conf.load('compiler_c')
    #conf.check_cxx(lib="opencv2",
    #            errmsg="Please install the opencv library",
    #            mandatory=True)
    
    #platform specific
    if sys.platform == 'darwin': 
        conf.env.INCLUDES_OS = ['/opt/local/include',
                                '/opt/local/include/libxml2',
                                '/usr/local/include/openni']
        conf.env.LIBPATH_OS = ['/opt/local/lib']
        conf.env.LIB_OS = ['m', 'xml2',
                           'opencv_core', 'opencv_highgui', 
                           'opencv_imgproc',]
        conf.env.FRAMEWORK_OS = ['Cocoa','OpenGL', 'AGL', 'Carbon', 
                                 'Accelerate', 'IOKit','System', 'AppKit',
                                 'CoreFoundation']
        conf.env.DEFINES_OS  = ['OSX=1', 'POSIX=1']
    elif sys.platform == 'win32' or sys.platform == 'cygwin':
        conf.env.INCLUDES_OS = ['os/win/include/', 'C:\MinGW\include']
        conf.env.LIBPATH_OS = [os.path.join(os.getcwd(), 'os/win/lib/')]
        conf.env.LIB_OS = ['m', 'pthreadGC1',
                           'ws2_32', 'xml2', 'GLU', 'GL',
                           'OpenNI2', 'fltk', 'fltk_gl', 
                           'opencv_core', 'opencv_highgui', 
                           'opencv_calib3d', 'opencv_imgproc']
    else : #linux
        conf.env.INCLUDES_OS = ['/usr/include', '/usr/local/include',
                                '/usr/include/libxml2', 
                                '/usr/local/include/openni',
                                '/usr/include/openni2']
        conf.env.LIB_OS = ['X11', 'Xxf86vm', 'm',
                           'opencv_core', 'opencv_highgui', 
                           'opencv_imgproc','juce','stk','Leap','aubio']
        conf.env.LIBPATH_OS = ['/usr/local/lib/']
        conf.env.DEFINES_OS  = ['POSIX=1','GL42=1', 'LINUX=1']

    conf.env.INCLUDES=['/usr/include/juce/modules', 
                      'JuceLibraryCode', 
                      '/usr/include/Leap', 
                      'JuceLibraryCode/stk/include', 
                      'Source/Audio',
                      'Source/Cursor',
                      'Source/Leap',
                      'Source/Osc']
   
    #release specific
    #conf.env.CXXFLAGS = ['-O3', '-Wall'] 
    conf.env.CXXFLAGS = ['-O3', '-std=c++0x'] 
    conf.env.DEFINES  = ['DEBUG(x)=//x', 'DONT_SET_USING_JUCE_NAMESPACE=1']

    #debug specific
    conf.setenv('debug', env=conf.env.derive())
    conf.env.CXXFLAGS = ['-g', '-Wall', '-std=c++11']
    conf.env.DEFINES  = ['DEBUG(x)=std::cout<< x <<std::endl;', 
                         'DONT_SET_USING_JUCE_NAMESPACE=1']

def build(bld):
    installPath = '/usr/local/bin'
    macApp = False

    bld.program(
        source       = bld.path.ant_glob('Source/**/*.cpp'),
        use          = ['OS'],
        target       = 'scandrum'+bld.variant,
        vnum         = '0.0.1',
        install_path = installPath,
        mac_app      = macApp,
        mac_plist     = 'os/mac/Info.plist',
    )

def install(bld):
    bld.install('${PREFIX}/bin', 'revil')

from waflib.Build import BuildContext, CleanContext
class debug(BuildContext): 
    cmd = 'debug'
    variant = 'debug' 
