# meego-contacts

The Nokia N9's contacts, written anew in QML with the MeeGo Harmattan Qt
Quick components (`com.nokia.meego 2.0`) on vCard files, for the MeeGo
session on Maemo Leste, Arch Linux ARM and FreeBSD and for Hildon. Nokia's
contacts are closed source; this app imitates their look and behaviour,
it contains none of Nokia's code or files (nor Nemo's: the code is new).

- **Contacts** (light, as the N9's): by name under their letters, with the
  photo or the N9's placeholder and the organisation, the N9's letter
  scroller at the side; search (in names, organisations, numbers, mail
  addresses, addresses) from the tool bar; new contact; import.
- **Card**: the photo and the name on the N9's card, then each number
  with its kind (a tap calls through meego-phone, the message icon writes
  through meego-messages), the mail addresses (meego-mail), the address,
  the birthday, the note; change or delete.
- **New or changed contact** (a sheet): first and last name, company, up
  to three numbers (mobile, home, work, phone), two mail addresses, the
  address, the birthday (the date picker), a note.

Each contact is a vCard 3.0 file of its own in
`~/.local/share/meego/meego-contacts/contacts` (other programs can read
and write them). Import reads every `.vcf` in the documents and downloads
folders: vCard 2.1 (quoted-printable, charsets), 3.0 and 4.0, with their
photos; a card without a UID gets one from what it says, so the same file
imported twice gives each contact once. Without meego-phone, -messages or
-mail installed the `tel:`, `sms:` and `mailto:` links go to the desktop's
handlers.

## The look: the N9's theme

Through `image://theme/`: the contact card's background, the avatar
placeholders, the tool bar's and the message icons, the components' own
(the letter scroller too). These are Nokia's (the "blanco" theme of one's
own N9 or N950) and are not in this repository.

## Building

Qt 5 (Quick) and qt-components' `com.nokia.meego` 2.0 at run time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
