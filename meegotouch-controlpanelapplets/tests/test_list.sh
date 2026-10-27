#! /bin/bash

#
# Filtering out the ut and ft dirs with the prefix given by $1
#except the disabled ones
#

for f in ut_$1*; do
	if [ $f == "ut_template" ]; then
		continue
	fi

	if [ ! -e $f/disabled -a ! -e $f/deleted ]; then
		echo $f
	fi
done

for u in ft_$1*; do
	if [ $u == "ft_template" ]; then
		continue
	fi

	if [ ! -e $u/disabled -a ! -e $u/deleted ]; then
		echo $u
	fi
done

