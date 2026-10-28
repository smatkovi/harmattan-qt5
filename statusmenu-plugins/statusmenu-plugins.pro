# Plugins for the status indicator menu (system-ui's drop-down), written
# anew for the Qt 5 port: the N9's own were not released. Volume
# (PulseAudio), internet connection (NetworkManager), Bluetooth (BlueZ).
TEMPLATE = subdirs
SUBDIRS = volume internetconnection bluetooth
