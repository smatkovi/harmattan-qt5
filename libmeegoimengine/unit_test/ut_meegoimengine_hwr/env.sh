# !/bin/bash
cd .. 
stubdirverpath=$PWD/mydriverstub/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$stubdirverpath
cd ..
meegoimenginepath=$PWD/hwr
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$meegoimenginepath
echo $LD_LIBRARY_PATH
cd unit_test_hwr 
cd ut_meegoimengine
