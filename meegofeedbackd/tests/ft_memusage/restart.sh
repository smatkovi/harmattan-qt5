#!/bin/sh

app=$1
cnt=10

echo "*** testing for $app for $cnt cycles each 10 runs ***"
rm -fr $app
sync

save-incremental-endurance-stats $app
c=1
while [ $c -le $cnt ]
do
   echo "*** starting test cycle # $c ***"
   i=1
   while [ $i -le 10 ]
   do
     su - user -c /usr/bin/$app &
     sleep 10
     kill `pidof $app`
     i=$(($i + 1))
     sleep 3
   done
   c=$(($c + 1))
   save-incremental-endurance-stats $app
   echo > /var/log/syslog
   sync
done

echo "finished testing for $app"
