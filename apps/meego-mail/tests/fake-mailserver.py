#!/usr/bin/python3
"""A stand-in IMAP (port 1143) and SMTP (port 1025) server on 127.0.0.1,
without TLS, for trying meego-mail on a PC: an inbox of sample messages
(encoded words, quoted-printable, an attachment, HTML only), Sent and
Trash; what is sent over SMTP lands in Sent and is printed. Any user name
and password log in. The account: address test@example.org, IMAP and SMTP
127.0.0.1 with the security "None", ports 1143 and 1025.
"""
import asyncio
import base64
import email
import email.utils
import re
import time

SAMPLES = [
    b"""From: Anna Berger <anna@example.org>\r
To: Test <test@example.org>\r
Subject: Rehearsal on Thursday\r
Date: %(d1)s\r
Message-ID: <r1@example.org>\r
Content-Type: text/plain; charset=utf-8\r
\r
Hi,\r
\r
the rehearsal moves to 19:30 on Thursday. Please bring the Brahms scores.\r
\r
Anna\r
""",
    b"""From: =?UTF-8?Q?J=C3=BCrgen_M=C3=BCller?= <juergen@example.org>\r
To: test@example.org\r
Cc: anna@example.org\r
Subject: =?UTF-8?B?w5xiZXIgZGllIEZhaHJ0IG5hY2ggR3Jheg==?=\r
Date: %(d2)s\r
Message-ID: <r2@example.org>\r
MIME-Version: 1.0\r
Content-Type: multipart/alternative; boundary="b1"\r
\r
--b1\r
Content-Type: text/plain; charset=utf-8\r
Content-Transfer-Encoding: quoted-printable\r
\r
Gr=C3=BC=C3=9F dich,\r
\r
wir fahren am Samstag um 8 Uhr ab. Treffpunkt beim Bahnhof, Gleis 7.\r
\r
J=C3=BCrgen\r
--b1\r
Content-Type: text/html; charset=utf-8\r
\r
<p>Gr&uuml;&szlig; dich,</p>\r
--b1--\r
""",
    b"""From: Tickets <noreply@tickets.example.com>\r
To: test@example.org\r
Subject: Your tickets\r
Date: %(d3)s\r
Message-ID: <r3@example.org>\r
MIME-Version: 1.0\r
Content-Type: multipart/mixed; boundary="mix"\r
\r
--mix\r
Content-Type: text/plain; charset=us-ascii\r
\r
Your tickets for the concert are attached.\r
--mix\r
Content-Type: application/pdf; name="tickets.pdf"\r
Content-Disposition: attachment; filename="tickets.pdf"\r
Content-Transfer-Encoding: base64\r
\r
JVBERi0xLjQKJcfsj6IKMSAwIG9iago8PC9UeXBlL0NhdGFsb2c+PgplbmRvYmoK\r
--mix--\r
""",
    b"""From: Newsletter <news@example.net>\r
To: test@example.org\r
Subject: October news\r
Date: %(d4)s\r
Message-ID: <r4@example.org>\r
Content-Type: text/html; charset=iso-8859-1\r
\r
<html><body><h1>October</h1><p>Caf\xe9 concerts every <b>Friday</b>.</p></body></html>\r
""",
]


def rfc2822(ago):
    return email.utils.formatdate(time.time() - ago, localtime=True).encode()


class Store:
    def __init__(self):
        d = {b"d1": rfc2822(3600 * 2), b"d2": rfc2822(86400), b"d3": rfc2822(86400 * 3), b"d4": rfc2822(86400 * 9)}
        self.folders = {"INBOX": [], "Sent": [], "Trash": []}
        self.uid = 0
        for i, s in enumerate(SAMPLES):
            self.add("INBOX", s % d, seen=(i >= 2))

    def add(self, folder, raw, seen=False):
        self.uid += 1
        self.folders[folder].append({"uid": self.uid, "raw": raw, "flags": {"\\Seen"} if seen else set()})


STORE = Store()


def q(s):
    if s is None:
        return "NIL"
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def addrs(header):
    if not header:
        return "NIL"
    out = []
    for name, addr in email.utils.getaddresses([header]):
        user, _, host = addr.partition("@")
        out.append("(%s NIL %s %s)" % (q(name) if name else "NIL", q(user), q(host)))
    return "(" + "".join(out) + ")"


def envelope(raw):
    m = email.message_from_bytes(raw)
    h = lambda k: m.get(k)
    return "(%s %s %s %s %s %s %s NIL NIL %s)" % (
        q(h("Date")), q(h("Subject")), addrs(h("From")), addrs(h("From")), addrs(h("From")),
        addrs(h("To")), addrs(h("Cc")), q(h("Message-ID")))


async def imap(reader, writer):
    selected = None
    def w(s):
        writer.write(s if isinstance(s, bytes) else s.encode())
    w("* OK stand-in IMAP ready\r\n")
    while True:
        line = await reader.readline()
        if not line:
            break
        line = line.decode(errors="replace").rstrip("\r\n")
        tag, _, rest = line.partition(" ")
        cmd, _, args = rest.partition(" ")
        cmd = cmd.upper()
        if cmd == "UID":
            sub, _, args = args.partition(" ")
            cmd = "UID " + sub.upper()
        print("IMAP", tag, cmd, args[:60], flush=True)
        if cmd == "CAPABILITY":
            w("* CAPABILITY IMAP4rev1\r\n%s OK\r\n" % tag)
        elif cmd == "LOGIN":
            w("%s OK logged in\r\n" % tag)
        elif cmd == "LIST":
            for f in STORE.folders:
                w('* LIST (\\HasNoChildren) "/" %s\r\n' % q(f))
            w("%s OK\r\n" % tag)
        elif cmd == "SELECT":
            selected = args.strip('"')
            n = len(STORE.folders.get(selected, []))
            w("* %d EXISTS\r\n%s OK [READ-WRITE] selected\r\n" % (n, tag))
        elif cmd == "UID SEARCH":
            w("* SEARCH %s\r\n%s OK\r\n" % (" ".join(str(m["uid"]) for m in STORE.folders[selected]), tag))
        elif cmd == "UID FETCH":
            uidset, _, what = args.partition(" ")
            uids = set(int(u) for u in uidset.split(","))
            for i, m in enumerate(STORE.folders[selected], 1):
                if m["uid"] not in uids:
                    continue
                if "BODY.PEEK[]" in what:
                    w("* %d FETCH (UID %d BODY[] {%d}\r\n" % (i, m["uid"], len(m["raw"])))
                    w(m["raw"])
                    w(")\r\n")
                else:
                    flags = " ".join(sorted(m["flags"]))
                    date = time.strftime("%d-%b-%Y %H:%M:%S +0000", time.gmtime())
                    w('* %d FETCH (UID %d FLAGS (%s) INTERNALDATE "%s" RFC822.SIZE %d ENVELOPE %s)\r\n'
                      % (i, m["uid"], flags, date, len(m["raw"]), envelope(m["raw"])))
            w("%s OK\r\n" % tag)
        elif cmd == "UID STORE":
            uid, op, flags = args.split(" ", 2)
            names = set(re.findall(r"\\\w+", flags))
            for m in STORE.folders[selected]:
                if m["uid"] == int(uid):
                    m["flags"] = m["flags"] | names if op.startswith("+") else m["flags"] - names
            w("%s OK\r\n" % tag)
        elif cmd == "EXPUNGE":
            keep = []
            for m in STORE.folders[selected]:
                if "\\Deleted" in m["flags"]:
                    if selected != "Trash":
                        m["flags"].discard("\\Deleted")
                        STORE.folders["Trash"].append(m)
                else:
                    keep.append(m)
            STORE.folders[selected] = keep
            w("%s OK\r\n" % tag)
        elif cmd == "LOGOUT":
            w("* BYE\r\n%s OK\r\n" % tag)
            await writer.drain()
            break
        else:
            w("%s BAD unknown\r\n" % tag)
        await writer.drain()
    writer.close()


async def smtp(reader, writer):
    def w(s):
        writer.write((s + "\r\n").encode())
    w("220 stand-in SMTP")
    rcpt = []
    while True:
        line = await reader.readline()
        if not line:
            break
        cmd = line.decode(errors="replace").strip()
        print("SMTP", cmd[:60], flush=True)
        up = cmd.upper()
        if up.startswith("EHLO"):
            w("250-stand-in\r\n250 AUTH PLAIN LOGIN")
        elif up.startswith("AUTH"):
            w("235 ok")
        elif up.startswith("MAIL FROM"):
            rcpt = []
            w("250 ok")
        elif up.startswith("RCPT TO"):
            rcpt.append(cmd)
            w("250 ok")
        elif up == "DATA":
            w("354 go")
            data = b""
            while True:
                l = await reader.readline()
                if l in (b".\r\n", b".\n", b""):
                    break
                data += l[1:] if l.startswith(b"..") else l
            STORE.add("Sent", data, seen=True)
            print("SENT to", rcpt, "\n" + data.decode(errors="replace"), flush=True)
            w("250 queued")
        elif up == "QUIT":
            w("221 bye")
            await writer.drain()
            break
        else:
            w("250 ok")
        await writer.drain()
    writer.close()


async def main():
    a = await asyncio.start_server(imap, "127.0.0.1", 1143)
    b = await asyncio.start_server(smtp, "127.0.0.1", 1025)
    print("IMAP 127.0.0.1:1143, SMTP 127.0.0.1:1025", flush=True)
    await asyncio.gather(a.serve_forever(), b.serve_forever())

asyncio.run(main())
