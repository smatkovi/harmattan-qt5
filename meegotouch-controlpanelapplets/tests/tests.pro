TEMPLATE = subdirs
include(../shared.pri)
include(common.pri)
#
# The coverage target will not build the functional tests.
#

contains(BUILD_FEATURES,coverage) {
    #
    # Sometimes it is good to disable all but one unit tests.
    #
    LIST = $$system(./unit_tests.sh)
    for(testdir, LIST) {
        addSubDirs($${testdir})
    }
} else {
    LIST = $$system(ls -1d ut_* ft_*)
    for(testdir, LIST) {
        addSubDirs($${testdir})
    }
}

if (!isEmpty(BUILD_APPLET)){
   message ("***Will build tests only for the given applets****")
   SUBDIRS = 
   APPLETLIST = BUILD_APPLET
      for(dirs, BUILD_APPLET){
         message ("***Will build test for:" $$dirs)
         SUBDIRLIST =  $$system(./test_list.sh $$dirs)
         for(testdirs, SUBDIRLIST){
             addSubDirs($${testdirs})
             }
      }

}

contains(BUILD_FEATURES,nocheck) {
    # nocheck option disables the test building
    message("*** Disabling unit/functional test building ***")
    SUBDIRS =
}

check.target = check
check.depends =
check.commands =
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

QMAKE_STRIP = echo
QMAKE_CLEAN += \
    **/*.log.xml \
    **/*.log \
    tests.xml

contains(BUILD_FEATURES,coverage) {
  QMAKE_EXTRA_TARGETS += coverage
  coverage.target = coverage
  coverage.commands =
  coverage.CONFIG = recursive
}

support_files.commands += $$PWD/gen-tests-xml.sh $$TEST_PKG_NAME > $$OUT_PWD/tests.xml
support_files.target = support_files
support_files.files += $$OUT_PWD/tests.xml
support_files.path = /usr/share/$$TEST_PKG_NAME
support_files.CONFIG += no_check_exist

dummy_file.commands += touch $$OUT_PWD/keep_this_dir
dummy_file.target = dummy_file
dummy_file.files += $$OUT_PWD/keep_this_dir
dummy_file.path = /usr/lib/$$TEST_PKG_NAME

INSTALLS += \
    support_files \
    dummy_file

