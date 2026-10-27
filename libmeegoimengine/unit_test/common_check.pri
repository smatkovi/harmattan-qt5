QMAKE_EXTRA_TARGETS += check
check.target = check
check.commands = @MYSTUBDRIVER_PATH=../lib/ LD_LIBRARY_PATH=../../src/:../lib/ ./$$TARGET
check.depends += $$TARGET

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.commands = LD_LIBRARY_PATH=../../src/:../lib/ valgrind --leak-check=full --trace-children=yes --num-callers=50 --xml=yes --log-file=$${TARGET}.memcheck.xml ./$$TARGET && \
	xsltproc --stringparam global.target $$TARGET ../memcheck.xsl $${TARGET}.memcheck.xml > $${TARGET}.memcheck.html
memcheck.depends += $$TARGET

QMAKE_CLEAN += *.log *.xml *~ *.html


INSTALLS += target \

