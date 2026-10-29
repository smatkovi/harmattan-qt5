#!/usr/bin/python3
"""A stand-in for ModemManager on the session bus, for trying meego-phone
on a PC (MEEGO_PHONE_BUS=session): one modem with voice calls. A dialled
call rings for a second and is answered; --incoming N makes a call come
in after N seconds; --answer-after N lets the far end hang up N seconds
after a call became active.

    dbus-run-session -- sh -c 'tests/fake-modemmanager.py --incoming 5 &
        MEEGO_PHONE_BUS=session meego-phone'
"""
import argparse

import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib

MM = "org.freedesktop.ModemManager1"
MODEM = MM + ".Modem"
VOICE = MODEM + ".Voice"
GPP = MODEM + ".Modem3gpp"
CALL = MM + ".Call"
PROPS = "org.freedesktop.DBus.Properties"
OBJMGR = "org.freedesktop.DBus.ObjectManager"
MODEM_PATH = "/org/freedesktop/ModemManager1/Modem/0"

# MMCallState, MMCallDirection
DIALING, RINGING_OUT, RINGING_IN, ACTIVE, TERMINATED = 1, 2, 3, 4, 7
OUTGOING, INCOMING = 2, 1


class Call(dbus.service.Object):
    count = 0

    def __init__(self, bus, voice, number, direction, state):
        Call.count += 1
        self.path = "/org/freedesktop/ModemManager1/Call/%d" % Call.count
        super().__init__(bus, self.path)
        self.voice = voice
        self.props = {"Number": dbus.String(number), "Direction": dbus.Int32(direction),
                      "State": dbus.Int32(state)}

    def set_state(self, new):
        old = int(self.props["State"])
        self.props["State"] = dbus.Int32(new)
        print("call %s: %d -> %d" % (self.path, old, new), flush=True)
        self.StateChanged(old, new, 0)
        if new == ACTIVE and ARGS.hangup_after:
            GLib.timeout_add_seconds(ARGS.hangup_after, lambda: self.Hangup() or False)
        return False

    @dbus.service.signal(CALL, signature="iiu")
    def StateChanged(self, old, new, reason):
        pass

    @dbus.service.method(CALL)
    def Start(self):
        self.set_state(DIALING)
        GLib.timeout_add(500, self.set_state, RINGING_OUT)
        GLib.timeout_add(1500, self.set_state, ACTIVE)

    @dbus.service.method(CALL)
    def Accept(self):
        self.set_state(ACTIVE)

    @dbus.service.method(CALL)
    def Hangup(self):
        if int(self.props["State"]) != TERMINATED:
            self.set_state(TERMINATED)

    @dbus.service.method(CALL, in_signature="s")
    def SendDtmf(self, tones):
        print("tones:", tones, flush=True)

    @dbus.service.method(PROPS, in_signature="ss", out_signature="v")
    def Get(self, iface, name):
        return self.props[name]


class Modem(dbus.service.Object):
    def __init__(self, bus):
        super().__init__(bus, MODEM_PATH)
        self.bus = bus
        self.calls = []
        self.props = {
            MODEM: {"Manufacturer": dbus.String("Test"), "Model": dbus.String("Stand-in modem"),
                    "SignalQuality": dbus.Struct((dbus.UInt32(72), dbus.Boolean(True)), signature="ub")},
            VOICE: {"Calls": dbus.Array([], signature="o")},
            GPP: {"OperatorName": dbus.String("Test Net")},
        }

    def interfaces(self):
        p = dict(self.props)
        p[VOICE] = {"Calls": dbus.Array([c.path for c in self.calls], signature="o")}
        return p

    @dbus.service.method(VOICE, in_signature="a{sv}", out_signature="o")
    def CreateCall(self, props):
        call = Call(self.bus, self, str(props.get("number", "")), OUTGOING, 0)
        self.calls.append(call)
        print("dialled", props.get("number"), flush=True)
        self.CallAdded(call.path)
        return call.path

    @dbus.service.method(VOICE, in_signature="o")
    def DeleteCall(self, path):
        for c in list(self.calls):
            if c.path == path:
                self.calls.remove(c)
                c.remove_from_connection()

    @dbus.service.signal(VOICE, signature="o")
    def CallAdded(self, path):
        pass

    def incoming(self):
        call = Call(self.bus, self, "+43 660 1234567", INCOMING, RINGING_IN)
        self.calls.append(call)
        print("incoming", call.path, flush=True)
        self.CallAdded(call.path)
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
p.add_argument("--incoming", type=int, default=0, help="a call comes in after N seconds")
p.add_argument("--hangup-after", type=int, default=0, help="the far end hangs up N s into a call")
p.add_argument("--no-modem", action="store_true", help="ModemManager without a modem")
ARGS = p.parse_args()

DBusGMainLoop(set_as_default=True)
bus = dbus.SessionBus()
name = dbus.service.BusName(MM, bus)
modem = Modem(bus)
if ARGS.no_modem:
    modem.props.pop(VOICE)
manager = Manager(bus, modem)
if ARGS.incoming:
    GLib.timeout_add_seconds(ARGS.incoming, modem.incoming)
GLib.MainLoop().run()
