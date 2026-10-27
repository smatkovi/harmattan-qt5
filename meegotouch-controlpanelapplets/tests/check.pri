QMAKE_STRIP = echo

CONFIG += link_prl
QMAKE_EXTRA_TARGETS += check
check.depends += $$TARGET
check.commands += ./$$TARGET

