#! /bin/bash

if [ $# -lt 2 ];
then
	echo "need more parameter"
elif [ $# -lt 3 ];
then
	sudo perf kmem record -o  perf_$1_$2_seq ./exefile $1 $2
	./2.show_perf.sh $1 $2
elif [ $# -lt 4 ];
then
	sudo perf kmem record -o  perf_$1_$2_$3 ./exefile $1 $2 $3
	./2.show_perf.sh $1 $2 $3
fi
