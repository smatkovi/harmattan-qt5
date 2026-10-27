#!/bin/bash

if [ ! "$1" ]; then
	echo "Usage:"
	echo "$0 on | off | view"
	exit 1
fi

case "$1" in
	view)
		gconftool-2 --get "/Meego/System/DeveloperMode"
		gconftool-2 --get "/MeeGo/System/UsbMode"
		;;
	on)
		gconftool-2 --type bool --set "/Meego/System/DeveloperMode" true
		;;
	off)
		gconftool-2 --type bool --set "/Meego/System/DeveloperMode" false
		;;
	*)
		echo "Unknown command line option."
		exit 1
esac

