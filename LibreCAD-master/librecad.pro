TEMPLATE = subdirs
TARGET = librecad
CONFIG += ordered

SUBDIRS     = \
    libraries \
    librecad \
    plugins \
    tools
PRE_TARGETDEPS += ../../generated/lib/dxfrw.lib
PRE_TARGETDEPS += ../../generated/lib/dxflib.lib
PRE_TARGETDEPS += ../../generated/lib/jwwlib.lib
PRE_TARGETDEPS += ../../generated/lib/fparser.lib
exists( custom.pro ):include( custom.pro )
