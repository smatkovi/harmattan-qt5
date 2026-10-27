#!/bin/sh

BASE=/system/osso/connectivity

gconftool-2 -s -t int $BASE/general_value_int 1
gconftool-2 -s -t bool $BASE/general_value_bool 1
gconftool-2 -s -t string $BASE/general_value_string "test string"
gconftool-2 -s -t list --list-type int $BASE/general_value_int_list '[ 1, 2, 1000, 3 ]'
gconftool-2 -s -t float $BASE/general_value_float 1.1


gconftool-2 -s -t int $BASE/IAP/foobar/foobar_value_int 1
gconftool-2 -s -t bool $BASE/IAP/foobar/foobar_value_bool 1
gconftool-2 -s -t string $BASE/IAP/foobar/foobar_value_string "test string"
gconftool-2 -s -t list --list-type int $BASE/IAP/foobar/foobar_value_int_list "[ 65, 66, 67, 68 ]"
gconftool-2 -s -t float $BASE/IAP/foobar/foobar_value_float 1.1


gconftool-2 -s -t string $BASE/IAP/WANO/ipv4_type AUTO
gconftool-2 -s -t bool $BASE/IAP/WANO/temporary false
gconftool-2 -s -t string $BASE/IAP/WANO/name WANO
gconftool-2 -s -t string $BASE/IAP/type WLAN_INFRA
gconftool-2 -s -t list --list-type int $BASE/IAP/WANO/wlan_ssid "[87,65,78,79]"
gconftool-2 -s -t string $BASE/IAP/WANO/proxytype NONE
gconftool-2 -s -t string $BASE/IAP/WANO/wlan_security NONE
gconftool-2 -s -t bool $BASE/IAP/WANO/wlan_hidden false
gconftool-2 -s -t string $BASE/IAP/WANO/demo_key "this is a key for the demo"


gconftool-2 -s -t int $BASE/network_type/GPRS/gprs_rx_bytes 1024
gconftool-2 -s -t int $BASE/network_type/GPRS/gprs_tx_bytes 20003
gconftool-2 -s -t list --list-type string $BASE/network_type/GPRS/network_modules "[libicd_network_gprs.so]"
gconftool-2 -s -t bool $BASE/network_type/GPRS/gprs_roaming_disabled true
gconftool-2 -s -t int $BASE/network_type/GPRS/gprs_no_coverage_timeout 60000

