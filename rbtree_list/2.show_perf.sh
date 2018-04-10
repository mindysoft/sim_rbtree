#! /bin/bash

if [ $# -lt 2 ];
then
	echo "need more parameter"
elif [ $# -lt 3 ];
then
	sudo perf report -i perf_$1_$2_seq
elif [ $# -lt 4 ];
then
	sudo perf report -i perf_$1_$2_$3
fi
