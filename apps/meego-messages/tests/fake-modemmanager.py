#!/usr/bin/python3
"""A stand-in for ModemManager's text messages on the session bus, for
trying meego-messages on a PC (MEEGO_MESSAGES_BUS=session): one modem with
Messaging; sent messages are "sent" after a second; --stored puts a
received message on the modem before the app starts; --sms N makes one
arrive after N seconds; --fail makes sending fail.

    dbus-run-session -- sh -c 'tests/fake-modemmanager.py --sms 5 &
        MEEGO_MESSAGES_BUS=session meego-messages'
"""
import argparse
import datetime

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

MM = "org.freedesktop.ModemManager1"
MODEM = MM + ".Modem"
MESSAGING = MODEM + ".Messaging"
SMS = MM + ".Sms"
PROPS = "org.freedesktop.DBus.Properties"
OBJMGR = "org.freedesktop.DBus.ObjectManager"
MODEM_PATH = "/org/freedesktop/ModemManager1/Modem/0"

# MMSmsState, MMSmsPduType
RECEIVED, SENDING, SENT = 3, 4, 5
DELIVER, SUBMIT = 1, 2


def now():
    return datetime.datetime.now().astimezone().isoformat(timespec="seconds")


class Sms(dbus.service.Object):
    count = 0

    def __init__(self, bus, number, text, state, pdu, stamp=None):
        Sms.count += 1
        self.path = "/org/freedesktop/ModemManager1/SMS/%d" % Sms.count
        super().__init__(bus, self.path)
        self.props = {"Number": dbus.String(number), "Text": dbus.String(text),
                      "State": dbus.UInt32(state), "PduType": dbus.UInt32(pdu),
                      "Timestamp": dbus.String(stamp or now())}

    def set_state(self, state):
        self.props["State"] = dbus.UInt32(state)
        self.PropertiesChanged(SMS, {"State": dbus.UInt32(state)}, [])
        return False

    @dbus.service.signal(PROPS, signature="sa{sv}as")
    def PropertiesChanged(self, iface, changed, invalidated):
        pass

    @dbus.service.method(SMS)
    def Send(self):
        if ARGS.fail:
            raise dbus.exceptions.DBusException("No network", name=MM + ".Error.Core.Failed")
        print("sending to", self.props["Number"], ":", self.props["Text"], flush=True)
        self.set_state(SENDING)
        GLib.timeout_add(1000, self.set_state, SENT)

    @dbus.service.method(PROPS, in_signature="ss", out_signature="v")
    def Get(self, iface, name):
        return self.props[name]


class Modem(dbus.service.Object):
    def __init__(self, bus):
        super().__init__(bus, MODEM_PATH)
        self.bus = bus
        self.messages = []

    def interfaces(self):
        return {MODEM: {"Manufacturer": dbus.String("Test"), "Model": dbus.String("Stand-in modem")},
                MESSAGING: {"Messages": dbus.Array([m.path for m in self.messages], signature="o")}}

    @dbus.service.method(MESSAGING, out_signature="ao")
    def List(self):
        return [m.path for m in self.messages]

    @dbus.service.method(MESSAGING, in_signature="a{sv}", out_signature="o")
    def Create(self, props):
        sms = Sms(self.bus, str(props.get("number", "")), str(props.get("text", "")), 0, SUBMIT)
        self.messages.append(sms)
        self.Added(sms.path, False)
        return sms.path

    @dbus.service.method(MESSAGING, in_signature="o")
    def Delete(self, path):
        for m in list(self.messages):
            if m.path == path:
                self.messages.remove(m)
                m.remove_from_connection()

    @dbus.service.signal(MESSAGING, signature="ob")
    def Added(self, path, received):
        pass

    def arrive(self, number, text, stamp=None, announce=True):
        sms = Sms(self.bus, number, text, RECEIVED, DELIVER, stamp)
        self.messages.append(sms)
        print("received from", number, flush=True)
        if announce:
            self.Added(sms.path, True)
        return False

    @dbus.service.method(PROPS, in_signature="ss", out_signature="v")
    def Get(self, iface, name):
        return self.interfaces()[iface][name]


class Manager(dbus.service.Object):
    def __init__(self, bus, modem):
        super().__init__(bus, "/org/freedesktop/ModemManager1")
        self.modem = modem

    @dbus.service.method(OBJMGR, out_signature="a{oa{sa{sv}}}")
    def GetManagedObjects(self):
        return {MODEM_PATH: self.modem.interfaces()}


p = argparse.ArgumentParser()
p.add_argument("--stored", action="store_true", help="a received message on the modem from the start")
p.add_argument("--sms", type=int, default=0, help="a message arrives after N seconds")
p.add_argument("--fail", action="store_true", help="sending fails")
ARGS = p.parse_args()

DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()
name = dbus.service.BusName(MM, bus)
modem = Modem(bus)
manager = Manager(bus, modem)
if ARGS.stored:
    modem.arrive("+43 660 1234567", "Are we still on for the rehearsal tonight?",
                 (datetime.datetime.now() - datetime.timedelta(hours=3)).astimezone().isoformat(timespec="seconds"),
                 announce=False)
if ARGS.sms:
    GLib.timeout_add_seconds(ARGS.sms, modem.arrive, "+43 660 1234567", "Great, see you at seven!")
GLib.MainLoop().run()
