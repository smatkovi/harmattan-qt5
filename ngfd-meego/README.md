# ngfd for the MeeGo look

The N9 played its event sounds (messages, e-mail, alarms, ringtones,
system sounds) through ngfd, which Nokia did not release. Nemo Mobile's
ngfd, its open successor (LGPL 2.1), reads the same event definitions, so
the N9's own (`/usr/share/ngfd/events.d` and the sounds in
`/usr/share/sounds` of an N9 or N950 root file system, Nokia's) play as
they did. `ngfd.ini` here has the plugins that run without Harmattan's
profiled and ohm.

Built from github.com/sailfishos/ngfd at tag 1.4.13 (and libngf 0.29 for
system-ui, `DEFINES+=HAVE_LIBNGF`): with Harmattan's mce-dev, which lacks
a newer signal name,

    ./autogen.sh
    CPPFLAGS='-DMCE_LED_PATTERN_DEACTIVATED_SIG="led_pattern_deactivated_ind"' \
        ./configure --prefix=/usr --sysconfdir=/etc --disable-systemd
    make && make install

ngfd uses the system bus (`com.nokia.NonGraphicFeedback1.Backend`, its
policy in `/etc/dbus-1/system.d`); `NGF_CONF_PATH` and `NGF_PLUGIN_PATH`
point it at a staging directory. Tried on a PC: the N9's SMS and e-mail
events play their sounds.
