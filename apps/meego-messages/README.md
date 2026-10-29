# meego-messages

The Nokia N9's conversations (text messages), written anew in QML with the
MeeGo Harmattan Qt Quick components (`com.nokia.meego 2.0`) on
ModemManager, for the MeeGo session on Maemo Leste, Arch Linux ARM and
FreeBSD and for Hildon. Nokia's messaging application is closed source;
this app imitates its look and behaviour, it contains none of Nokia's code
or files (nor Nemo's: the code is new).

- **Conversations** (light, as the N9's): one per number however it is
  written, the latest first, with the last message (yours with "You:"),
  when, and the unread count; a long press deletes one. New message in the
  tool bar; "No modem" in the header without one.
- **Conversation**: the messages in the N9's bubbles (theirs blue on the
  right, yours white on the left, as the N9's pictures point), the time
  under each and "Sending…" or "Not sent"; the writing field and Send. A
  message that arrives while it is open shows at once and counts as read;
  elsewhere a banner shows it. A long press forwards or deletes a message.
- **New message**: the number, the text, Send, then its conversation.

Messages are kept in the app's own SQLite store
(`~/.local/share/meego/meego-messages/messages.sqlite`); what the modem
receives comes in through ModemManager's `Modem.Messaging` (`List` at the
start for what arrived meanwhile, `Added` while it runs, once each: the
number, time and text are its key; a message arriving in parts when it is
complete). Sending is `Create` and `Send`, the state follows the message's
`State`. The messages stay on the modem as well. `meego-messages 0123` or
`meego-messages sms:0123` opens that number's conversation.

## Trying it on a PC

`tests/fake-modemmanager.py` stands in for ModemManager's messages on a
session bus (`--stored`: one received before the start, `--sms N`: one
arrives after N seconds, `--fail`: sending fails):

    dbus-run-session -- sh -c 'tests/fake-modemmanager.py --stored --sms 5 &
        MEEGO_MESSAGES_BUS=session meego-messages'

## The look: the N9's theme

Through `image://theme/`: the conversation bubbles
(`meegotouch-messaging-conversation-bubble-*`), the avatar placeholder,
the tool bar's icons, the components' own. These are Nokia's (the
"blanco" theme of one's own N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick, D-Bus, SQL with its SQLite driver) and qt-components'
`com.nokia.meego` 2.0 at run time; ModemManager on the device:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
