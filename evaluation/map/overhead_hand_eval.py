#!/usr/bin/env python
import subprocess

avg = 0
for j in range(100, 10001,100):
	resultf = open('avg', 'a')
	print(j)
	kernelf = open('map.ccode', 'w')
	kernelf.write('void f0(\n')
	kernelf.write('	float g_in,\n')
	kernelf.write('	int index,\n')
	kernelf.write('	float* g_out\n')
	kernelf.write(')\n')
	kernelf.write('{\n')
	kernelf.write('	float out = g_in;\n')
	kernelf.write('	for(int i = 0; i < '+str(j)+'; ++i)\n')
	kernelf.write('		out+= 1;\n')
	kernelf.write('	*g_out = out;\n')
	kernelf.write('}\n')
	kernelf.close()
	out_bytes = subprocess.check_output(['../../build/bin/map_hand'])
	out_text = out_bytes.decode('ascii')
	result = out_text.split('\n')[-2]
	result = result.split('\t')[1]
	avg = float(result)
	resultf.write(str(j) +'\t'+str(avg)+'\n')
	resultf.close()
