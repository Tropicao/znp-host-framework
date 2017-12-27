project('ZNP Host Library', 'c')

# Sources
src = ['framework/znp.c',
    'framework/rpc/rpc.c',
    'framework/rpc/queue.c',
    'framework/mt/mtParser.c',
    'framework/mt/Zdo/mtZdo.c',
    'framework/mt/Sys/mtSys.c',
    'framework/mt/Af/mtAf.c',
    'framework/mt/Sapi/mtSapi.c',
    'framework/mt/Util/mtUtil.c',
    'framework/platform/gnu/dbgPrint.c',
    'framework/platform/gnu/hostConsole.c',
    'framework/platform/gnu/rpcTransport.c']

# Includes
headers = ['framework/znp.h',
    'framework/platform/tirtos/sys/ioctl.h',
    'framework/platform/gnu/hostConsole.h',
    'framework/platform/gnu/dbgPrint.h',
    'framework/platform/gnu/rpcTransport.h',
    'framework/mt/Af/mtAf.h',
    'framework/mt/Sys/mtSys.h',
    'framework/mt/Zdo/mtZdo.h',
    'framework/mt/Sapi/mtSapi.h',
    'framework/mt/Util/mtUtil.h',
    'framework/mt/mtParser.h',
    'framework/rpc/queue.h',
    'framework/rpc/rpc.h']
znp_incdir = include_directories('framework')
gnu_incdir = include_directories('framework/platform/gnu')
rpc_incdir = include_directories('framework/rpc')
mt_incdir = include_directories('framework/mt')
af_incdir = include_directories('framework/mt/Af')
zdo_incdir = include_directories('framework/mt/Zdo')
sys_incdir = include_directories('framework/mt/Sys')
sapi_incdir = include_directories('framework/mt/Sapi')
util_incdir = include_directories('framework/mt/Util')
incdir = [gnu_incdir,
    rpc_incdir,
    mt_incdir,
    af_incdir,
    zdo_incdir,
    sys_incdir,
    sapi_incdir,
    util_incdir]

# Libraries
cc = meson.get_compiler('c')
rtdep = cc.find_library('rt')
pthreaddep = dependency('threads')
dep = [rtdep, pthreaddep]

# Build options
cflags=['-Wall', '-Wextra', '-Werror']

shared_library('znp',
    sources: src,
    c_args: cflags,
    include_directories: incdir,
    dependencies: dep,
    version: '1.0.0',
    soversion:'0',
    install: true)

install_headers(headers, subdir: 'libznp')
