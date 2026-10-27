#!/bin/bash

if [ ! "$1" ]; then
	echo "Usage:"
	echo "$0 set | unset | view"
	exit 1
fi

if [ "$1" == "set" ]; then
    if [ ! "$2" ]; then 
        echo "The 'set' method needs an image file name."
        exit 1
    fi

    if [ ! -e "$2" ]; then
        echo "The file does not exists"
        exit 2
    fi
fi

case "$1" in
    view)
    	echo "view"
        gconftool-2 --recursive-list /desktop/meego/background
        ;;

    unset)
    	echo "Unset"
        gconftool-2 --recursive-unset /desktop/meego/background
        ;;

    set)
        echo "Setting to: " "$2"
        gconftool-2 --type string --set /desktop/meego/background/portrait/picture_filename "$2"
        gconftool-2 --type string --set /desktop/meego/background/landscape/picture_filename "$2"
        gconftool-2 --type string --set /desktop/meego/background/landscape/original_filename "$2"
        gconftool-2 --type string --set /desktop/meego/background/portrait/original_filename "$2"
        
        gconftool-2 --recursive-list /desktop/meego/background
        ;;

    *)	
        echo "Unknown command line option."
		exit 1
esac
