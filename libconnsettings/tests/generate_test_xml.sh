#!/bin/sh

PACKAGE="libconnsettings0"

echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
<testdefinition version=\"0.1\">
  <suite name=\"$PACKAGE-test\" domain=\"connectivity\"> 
    <set name=\"connsettingstestset1\" description=\"Unit tests\">
      <pre_steps>
        <step>export DISPLAY=:0.0</step>
        <step>source /tmp/session_bus_address.user</step>
      </pre_steps>
"

for item in $@; do
    echo "
        <case name=\"$item\" description=\"Unit test $item\">
            <step>/usr/lib/$PACKAGE-test/$item</step>
        </case>"
done

echo "
      <environments>
        <scratchbox>false</scratchbox>
        <hardware>true</hardware>
      </environments>
    </set>
  </suite>
</testdefinition>
"
