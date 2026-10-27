include(../config.pri)

CONFIG += ordered

TEMPLATE = subdirs

SUBDIRS = \
          ft_mimsettingsapplet \

#          ut_mimsettingsconf \ # FIXME: Needs to be change to not rely on MImApplication and MImpPluginManager

outputFiles(runtests.sh, gen-tests-xml.sh)

target.commands += $$OUT_PWD/gen-tests-xml.sh > $$OUT_PWD/tests.xml
target.path = $$MALIIT_TEST_DATADIR
target.files += qtestlib2junitxml.xsl $$OUT_PWD/runtests.sh $$OUT_PWD/tests.xml
target.depends += $$OUT_PWD/gen-tests-xml.sh
target.CONFIG += no_check_exist
INSTALLS += target

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive
