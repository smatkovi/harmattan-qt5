include(../common_top.pri)

# Stupid qmake wants to link everything.
QMAKE_LINK = @: IGNORE THIS LINE
QMAKE_CLEAN -= ft_wallpaper01
QMAKE_DISTCLEAN -= ft_wallpaper01

testbg_files.files += testwallpaper.png
testbg_files.path = /usr/share/$$TEST_PKG_NAME

ftprepare_files.files += ft_wallpaper01
ftprepare_files.path = /usr/lib/$$TEST_PKG_NAME

INSTALLS +=  \
    ftprepare_files \
    testbg_files

