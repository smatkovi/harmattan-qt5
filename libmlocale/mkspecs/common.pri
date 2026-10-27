# Build configuration

QMAKE_TARGET_COMPANY = Nokia
QMAKE_TARGET_PRODUCT = DirectUI
QMAKE_TARGET_DESCRIPTION = DirectUI Framework
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2010 Nokia


# Features
# Parts to build. Options: libs tests benchmarks demos doc debian
#M_BUILD_PARTS = libs tests benchmarks demos doc debian

# Load configure script results
!win32: {
    include($${M_BUILD_TREE}/mkspecs/mlocaleconfig.pri)
}

# No -Werror: today's compilers, ICU and Qt 5's deprecations warn where
# Harmattan's GCC 4.4 and Qt 4.7 did not

# ICU 61 and later keep their classes in the icu namespace only
DEFINES += U_USING_ICU_NAMESPACE=1

mac {
    INCLUDEPATH += include
}

# Load global definitions
include($${M_BUILD_TREE}/mkspecs/features/mlocale_defines.prf)

# Defines for directories, for use in source code.
{
    # TRANSLATION_DIR determines the default translation path
    DEFINES += TRANSLATION_DIR=\\\"\"$$ML_TRANSLATION_DIR\"\\\"

    # extra data for libicu to be used be default
    DEFINES += ML_ICUEXTRADATA_DIR=\\\"\"$$ML_ICUEXTRADATA_DIR\"\\\"
}

unix {
   # drop unneeded dependencies
   !macx {
      QMAKE_LFLAGS += "-Wl,--as-needed"
   }
}

# Compiler configuration for all subprojects in libm

!win32-msvc*:QMAKE_CXXFLAGS *= -g

contains( CONFIG, coverage ) {
    QMAKE_CXXFLAGS *= -ftest-coverage -fprofile-arcs
    LIBS *= -lgcov

    QMAKE_CLEAN *= \
    *.gcda \
    *.gcno \
    *.gcov
}

QMAKE_LIBDIR += $${M_BUILD_TREE}/lib

include(shared.pri)
