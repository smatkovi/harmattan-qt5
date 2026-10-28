require 'test/unit/testsuite'
require 'test/unit/ui/console/testrunner'
require '/usr/share/meegofeedbackd-tests/ft_memusage/tc_meegofeedbackd.rb'

class TS_Haptics
    def self.suite
        suite = Test::Unit::TestSuite.new(name = "TS_Haptics")
        suite << TC_Meegofeedbackd.suite
        return suite
    end
end

Test::Unit::UI::Console::TestRunner.run(TS_Haptics)
