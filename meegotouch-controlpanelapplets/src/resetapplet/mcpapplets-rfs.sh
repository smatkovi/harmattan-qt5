#! /bin/sh
set -e
# USB applet
gconftool-2 --recursive-unset /Meego/System/UsbMode
# Display settings applet
gconftool-2 --recursive-unset /system/osso/dsm/display
rm -f $HOME/.config/mcompositor/mcompositor.conf
# Battery settings applet
gconftool-2 --recursive-unset /system/osso/dsm/energymanagement
# Wallpaper settings applet
gconftool-2 --recursive-unset /desktop/meego/background
rm -rf $HOME/.wallpapers
# Soundsettings applet
gconftool-2 --recursive-unset /meegotouch/input_feedback
# Theme applet
gconftool-2 --recursive-unset /meegotouch/theme
# Easter eggs.
gconftool-2 --recursive-unset /apps/mcpeggs
