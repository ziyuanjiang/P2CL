for i in $(grep -rl "comb"); 
	do vim -c "%s/comb/map/gc" -c "wq" "$i"; 
done
for i in $(grep -rl "Comb"); 
	do vim -c "%s/Comb/Map/gc" -c "wq" "$i"; 
done
