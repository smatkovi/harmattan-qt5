include(../shared.pri)
TEMPLATE = subdirs

addSubDirs(mydriverstub_words)
addSubDirs(mydriverstub2_words)
addSubDirs(mydriverstub_hwr)
addSubDirs(mydriverstub2_hwr)
addSubDirs(ut_meegoimengine_hwr,mydriverstub_hwr mydriverstub2_hwr)
addSubDirs(ut_meegoimengine_words,mydriverstub_words mydriverstub2_words)


testxml.target=tests.xml
testxml.commands= $$system(touch tests.xml)
testxml.commands= ./gen-tests-xml.sh > tests.xml
testxml.path=/usr/share/libmeegoimengine-tests/
testxml.files+= runtests.sh \
                qtestlib2junitxml.xsl \
		gen-tests-xml.sh \
		tests.xml

testxml.config += no_check_exist

QMAKE_EXTRA_TARGET += testxml

INSTALLS += testxml

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.CONFIG = recursive

QMAKE_CLEAN += *.xml *~
QMAKE_DISTCLEAN += $$QMAKE_CLEAN
