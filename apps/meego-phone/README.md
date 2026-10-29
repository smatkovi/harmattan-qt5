# meego-phone

The Nokia N9's phone, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`) on ModemManager, for the MeeGo
session on Maemo Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's
phone and call screen are closed source; this app imitates their look and
behaviour, it contains none of Nokia's code or files (nor Nemo's: the code
is new).

- **Dialler** (dark): the number in the N9's field with the delete key (a
  long press clears it), the keypad with the letters (a long press on 0
  gives "+"), the green call key (on an empty number it brings back the
  last one); beside it (landscape) the latest calls, a tap puts a number
  in. The tool bar opens the history (with the count of the calls missed
  since) and the menu (the modem and its network, clear the history).
- **Call history** (light, as the N9's): received, dialled and missed
  calls with the N9's icons, when and how long; a tap calls back, a long
  press puts the number in the dialler or deletes the call.
- **Call screen**: the caller's picture place, the number, "Calling…",
  "Incoming call", the call's time, "Call ended"; for a call coming in
  Reject and Answer; in a call Mute, Loudspeaker, Keypad (the tones go to
  the network as DTMF) and End call. It comes up for every call, also one
  that comes in while the app is in the background, and goes after the
  call.

Calls go through ModemManager's D-Bus interface (the first modem with
`Modem.Voice`; `CreateCall` and `Start`, `Accept`, `Hangup`, `SendDtmf`;
the calls' `StateChanged`). Without ModemManager or a modem the dialler
says so and does not call. The history is the app's own
(`~/.local/share/meego/meego-phone/calls.json`). Mute and the loudspeaker
go through PulseAudio (`pactl`: the default source's mute, the default
sink's ports named speaker and earpiece/handset); without such ports the
loudspeaker button is off. `meego-phone 0123` or `meego-phone tel:0123`
starts with a number in the dialler.

## Trying it on a PC

`tests/fake-modemmanager.py` stands in for ModemManager on a session bus
(one modem; a dialled call is answered after a second; `--incoming N` makes
a call come in after N seconds, `--hangup-after N` ends a call N seconds
after it was answered):

    dbus-run-session -- sh -c 'tests/fake-modemmanager.py --incoming 5 &
        MEEGO_PHONE_BUS=session MEEGO_PHONE_NO_PACTL=1 meego-phone'

`MEEGO_PHONE_NO_PACTL` leaves the PC's sound alone.

## The look: the N9's theme

Through `image://theme/`: the call screen's backgrounds and buttons
(`meegotouch-callui-*`), the telephony icons (`icon-m-telephony-*`), the
avatar placeholder, the components' own. These are Nokia's (the "blanco"
theme of one's own N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick, D-Bus) and qt-components' `com.nokia.meego` 2.0 at run time;
ModemManager and PulseAudio's `pactl` on the device:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
