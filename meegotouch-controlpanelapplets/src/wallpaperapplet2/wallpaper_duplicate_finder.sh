#!/bin/sh
#
# Author: David Kedves <kedazo@gmail.com>
# Copyright 2012 Nokia Oy
#

#
# INFO: This script is removing the wallpaper duplicates from
# the MyDocs partititon, as those are installed to the rootfs
# after the PR1.2 SSU update.
#

NEWDIR=/usr/share/backgrounds
OLDDIR=/home/user/MyDocs/.wallpapers
FLAGFILE=/var/clean-device/wallpaper_dups_removed

# We must run only once
if [ -f ${FLAGFILE} ]; then
    exit
fi

# For Mass-Storage mode
if [ ! -d ${OLDDIR} ]; then
    exit
fi

IFS='
'

file_size() {
    # getting file-size (with busybox) even with spaces in the filename
    cd `dirname "$1"`

    BASENAME=`basename "$1"`
    SIZE=`stat -t "${BASENAME}" | sed "s/${BASENAME}//" | cut -d' ' -f1`
    echo ${SIZE}

    cd ${HOME}
}

for img in `ls -1 ${NEWDIR}`; do
    OLDIMAGE=${OLDDIR}/`basename "${img}"`
    if [ ! -f ${OLDIMAGE} ]; then
        # Skip non-existant wallpapers
        continue
    fi
    NEWIMAGE=${NEWDIR}/`basename "${img}"`
    ORIGSIZE=`file_size ${NEWIMAGE}`
    SIZE=`file_size ${OLDIMAGE}`
    if [ ${ORIGSIZE} -eq ${SIZE} ]; then
        rm -f "${OLDIMAGE}"
    fi
done

touch ${FLAGFILE}

