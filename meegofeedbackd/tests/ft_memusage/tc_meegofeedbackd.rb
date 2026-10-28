require 'nokogiri'
require 'test/unit'

class TC_Meegofeedbackd < Test::Unit::TestCase

  def setup
  end

  def teardown
  end

  def test_1_CheckMemLeak
    # analysing generated result report
    document = File.open("endurance-report.html", "r")
    doc = Nokogiri::HTML.parse(document)

    # find the memory usage summary table
    i = 0
    flag = doc.xpath("//table[last()]/caption[1]").text.to_s
    while flag != "Process private dirty and swap memory usages combined (according to SMAPS)"
        i+=1
        flag = doc.xpath("//table[last()-#{i}]/caption[1]").text.to_s
    end

    # find the row of meegofeedbackd's memory usage in table
    j = 1
    not_find = 0
    proc_name = doc.xpath("//table[last()-#{i}]/tr[#{j}]/td[1]").text.to_s[0..-7]
    while proc_name != "meegofeedbackd"
        if proc_name != "Total ch"
            j+=1
            proc_name = doc.xpath("//table[last()-#{i}]/tr[#{j}]/td[1]").text.to_s[0..-7]
        else
            not_find = 1
            break
        end
    end

    if not_find != 1
        mem_increase = doc.xpath("//table[last()-#{i}]/tr[#{j}]/td[2]").text.to_s[1..-4].to_i
    else
        proc_name = "meegofeedbackd"
        mem_increase = 0
    end

    puts proc_name
    puts mem_increase
    assert(mem_increase < 300, message="used memory of #{proc_name} increased #{mem_increase} KB, more than 300 KB")

    document.close
  end

end
