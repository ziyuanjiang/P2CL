#/bin/bash
w=16
h=16
for i in `seq 8 26`;
do
	x=$((2**$i))
	sed "s/rw/$w/g" reshape.cl.tmp > reshape.cl.tmp1
	sed "s/rh/$h/g" reshape.cl.tmp1 > reshape.cl
	echo $x >>reshape_hand_rlt
	../build/bin/reshape_hand $x | awk 'END {print $5}' >> reshape_hand_rlt

	if test `expr $i % 2` == 0
	then
		w=$(($w*2))
	else
		h=$(($h*2))
	fi
done 
