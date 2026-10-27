include(../mconfig.pri)

VERSION = 0.1.0
TEMPLATE = lib
TARGET=meegoimengine
INCLUDEPATH = ./ ../includes
LIBS += -L./

include(../meegoimengine.prf)

LIBS += -L$$[QT_INSTALL_LIBS]
QT = core

# coverage flags are off per default, but can be enabled via qmake COV_OPTION=on
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

QMAKE_CLEAN += *.gcno *.gcda

SOURCES +=  mimenginefactory.cpp \
            mimenginewordsinterface.cpp \
            mimenginehwrinterface.cpp \


HEADERS +=  mimenginewordsinterface.h \
            MImEngineWordsInterface \
            mimenginehwrinterface.h \
            MImEngineHwrInterface \
            mimenginefactory.h \
            ../includes/mimenginetypes.h \

target.path += $$MEEGOIMENGINE_PREFIX/lib

headers.path += $$MEEGOIMENGINE_INSTALL_HEADERS
headers.files += $$HEADERS

install_headers.path = $$MEEGOIMENGINE_INSTALL_HEADERS
install_headers.files = include/*

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = meegoimengineinterface.prf

INSTALLS += target \
    headers \
    install_headers \
    install_pkgconfig \
    install_prf \

QMAKE_EXTRA_TARGETS += check
check.depends = lib$${TARGET}.so.$$VERSION
check.commands = $$system(true)

QMAKE_EXTRA_TARGETS += memcheck
memcheck.depends = lib$${TARGET}.so.$$VERSION
memcheck.commands = $$system(true)

MIMENGINE_PCFILE = MeegoImEngine.pc.in

PREFIX_PATH=/usr

pcfile.output = MeegoImEngine.pc
pcfile.input = MIMENGINE_PCFILE
pcfile.commands += sed -e \"s:PREFIX_PATH:$$PREFIX_PATH:g\" ${QMAKE_FILE_NAME} > ${QMAKE_FILE_OUT}
pcfile.CONFIG = target_predeps no_link no_check_exist
PRE_TARGETDEPS += $${pcfile.output}
QMAKE_EXTRA_COMPILERS += pcfile
QMAKE_EXTRA_TARGETS += pcfile

install_pkgconfig.path = /usr/lib/pkgconfig
install_pkgconfig.files = MeegoImEngine.pc
install_pkgconfig.config = no_check_exist
