#!/bin/sh

# restart call-ui
chmod +x /usr/share/meegofeedbackd-tests/ft_memusage/restart.sh
/usr/share/meegofeedbackd-tests/ft_memusage/restart.sh call-ui
echo "*** restarting finished ***"

# generate results report
parse-endurance-measurements call-ui/[0-9]*
echo "*** results report generated ***"

# run testcase to check memory usage of feedbackd
ruby /usr/share/meegofeedbackd-tests/ft_memusage/tests.rb
