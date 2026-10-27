#!/bin/bash


STARTUP_TEMPLATE="<case name=\"init_testcase\" description=\"Remove possible stale configuration file\" insignificant=\"true\" requirement=\"\" timeout=\"120\">
        <step expected_result=\"0\">rm -f ~/.config/Nokia/MImEngineWordsInterface.conf</step>
      </case>
      "
UT_STARTUP="${STARTUP_TEMPLATE}"

UT_TESTCASES_WORDS=""
UT_TESTCASES_HWR=""

for TEST in `ls -d ut_*words`; do
      if [ -x $TEST/$TEST ]; then

TESTCASE_TEMPLATE="<case name=\"$TEST\" description=\"$TEST\" requirement=\"\" timeout=\"120\">
        <step expected_result=\"0\">LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/meego-imengines/drivers/words /usr/lib/libmeegoimengine-tests/words/$TEST/$TEST</step>
      </case>
      "

            UT_TESTCASES_WORDS="${UT_TESTCASES_WORDS}${TESTCASE_TEMPLATE}"
        fi
done


for TEST in `ls -d ut_*hwr`; do
      if [ -x $TEST/$TEST ]; then

TESTCASE_TEMPLATE="<case name=\"$TEST\" description=\"$TEST\" requirement=\"\" timeout=\"120\">
        <step expected_result=\"0\">LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/meego-imengines/drivers/hwr /usr/lib/libmeegoimengine-tests/hwr/$TEST/$TEST</step>
      </case>
      "

            UT_TESTCASES_HWR="${UT_TESTCASES_HWR}${TESTCASE_TEMPLATE}"
        fi
done


TESTSUITE_TEMPLATE="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
<testdefinition version=\"0.1\">
  <suite name=\"libmeegoimengine-tests\">
    <set name=\"unit_tests\" description=\"Unit Tests\">

      $UT_STARTUP
      $UT_TESTCASES_WORDS
      $UT_TESTCASES_HWR

      <environments>
        <scratchbox>false</scratchbox>
        <hardware>true</hardware>    
      </environments> 

    </set>
  </suite>
</testdefinition>"

echo "$TESTSUITE_TEMPLATE"

