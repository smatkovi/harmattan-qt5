include(../common_top.pri)

TEMPLATE=subdirs

install_files.path=/usr/share/meegofeedbackd-tests/ft_memusage
install_files.files+=restart.sh runtest.sh tc_meegofeedbackd.rb tests.rb

install_xml.path=/usr/share/meegofeedbackd-tests
install_xml.files+=tests.xml

INSTALLS += install_files \
            install_xml \

include(../common_bot.pri)
