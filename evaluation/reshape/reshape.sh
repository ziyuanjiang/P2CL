#/bin/bash
w=16
h=16
for i in `seq 8 26`;
do
	x=$((2**$i))
	sed "s/replace/$x/g" reshape.xml.tmp > reshape.xml.tmp1
	sed "s/rw/$w/g" reshape.xml.tmp1 > reshape.xml.tmp2
	sed "s/rh/$h/g" reshape.xml.tmp2 > reshape.xml
	sed "s/replace/$x/g" reshape.cpp.tmp > reshape.cpp
	make -C ../build
	echo $x >>reshape_rlt
	../build/bin/reshape | awk 'END {print $3}' >> reshape_rlt

	if test `expr $i % 2` == 0
	then
		w=$(($w*2))
	else
		h=$(($h*2))
	fi
done 
