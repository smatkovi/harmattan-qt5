#####################################################################
# DirectUI project file
#####################################################################

CONFIG += ordered
TEMPLATE = subdirs

!win32 {
    !exists($${OUT_PWD}/mkspecs/mlocaleconfig.pri) {
        error("Please run ./configure before proceeding")
    }
}

include(mkspecs/common.pri)
include(mkspecs/features/mlocale_coverage.prf)

isEqual( IN_PWD, $${OUT_PWD} ) {
    IS_OUT_OF_SOURCE = 0
} else {
    IS_OUT_OF_SOURCE = 1
}

# Not src/icu-extradata: Nokia's additions to ICU 4.4's data (collation,
# locale data) in its old source format, which today's genrb refuses;
# today's ICU carries its own
SUBDIRS += \
    src \
    benchmarks \
    tests \

# Docs are always explicitly built with "make doc"
include(doc/doc.pri)

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml
