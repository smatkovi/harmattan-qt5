# meego-mail

The Nokia N9's mail, written anew in QML with the MeeGo Harmattan Qt Quick
components (`com.nokia.meego 2.0`) on an IMAP and SMTP client of its own,
for the MeeGo session on Maemo Leste, Arch Linux ARM and FreeBSD and for
Hildon. Nokia's mail (Fenix) is closed source; this app imitates its look
and behaviour, it contains none of Nokia's code or files (nor Nemo's: the
code is new).

- **Folder** (light, as the N9's): the folder and the account in the
  header (a tap changes the folder: the server's folders with the N9's
  icons for inbox, sent, drafts, trash and spam), the latest 60 messages,
  the newest first, the unread ones bold on the N9's unread background;
  new message, get mail, the menu (folders, account).
- **Message**: the subject, from, to, cc and the date in the reader's
  header, the attachments' names, the text (the plain part, else the HTML
  part as plain text); reply (with the quote), reply to all, forward, mark
  as unread, delete. Opened messages are seen on the server too.
- **New message**: to, cc, subject, the text; send. Replies keep the
  thread (`In-Reply-To`, `References`).
- **Account** (a sheet): name, address, password; the servers filled in
  from the address for the known providers (Gmail, Outlook, GMX, web.de,
  Posteo, mailbox.org, Yahoo, iCloud), else `imap.` and `smtp.` of its
  domain; ports, SSL, STARTTLS or none, the user name.

IMAP (RFC 3501, a connection per job: LIST, SELECT, UID SEARCH, UID FETCH
of the envelopes and of a whole message, UID STORE, EXPUNGE) and SMTP
(EHLO, STARTTLS, AUTH PLAIN or LOGIN, one message per connection) are
written here on Qt's SSL sockets; MIME too: encoded words, quoted-printable
and base64, charsets, multipart/alternative and mixed, attachments'
names (RFC 2231 too). The account is in `~/.config/meego/meego-mail.conf`,
the password with it, readable by its user alone; the folders' messages
and the opened ones are cached in `~/.cache/meego/meego-mail` for reading
offline. Not yet: HTML shown as HTML, opening attachments, a copy of sent
messages in the server's Sent folder, more than one account.
`meego-mail mailto:x@y` starts a message to x@y.

## Trying it on a PC

`tests/fake-mailserver.py` is a stand-in IMAP (127.0.0.1:1143) and SMTP
(127.0.0.1:1025) server without TLS, with sample messages; set up the
account test@example.org with the servers 127.0.0.1, the ports 1143 and
1025, security "None". Only that path was tried: TLS and STARTTLS against
a real server are untested.

## The look: the N9's theme

Through `image://theme/`: the reader's header, the unread list
background, the attachment row, the folders' and the tool bar's icons, the
components' own. These are Nokia's (the "blanco" theme of one's own N9 or
N950) and are not in this repository.

## Building

Qt 5 (Quick, Network with OpenSSL) and qt-components' `com.nokia.meego`
2.0 at run time:

    qmake PREFIX=/usr && make && make install

`--fullscreen` starts it without the window's title bar.

## Licence

BSD 3-Clause, see `LICENSE`.
