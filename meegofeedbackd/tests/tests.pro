TEMPLATE = subdirs

ARCH = $$system(uname -m)

SUBDIRS = \
          ft_configuration \
          ft_reconnection \
          ft_theming \
          ft_memusage \
          ut_mfcmdline \
          ut_mfsignal \
          ut_mfmanager \
          ut_mffeedback

QMAKE_STRIP = echo
include(shell.pri)
include(runtests.pri)

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml

QMAKE_CLEAN += **/*.log.xml ./coverage.log.xml **/*.log
