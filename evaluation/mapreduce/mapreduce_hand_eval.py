#!/usr/bin/env python
import subprocess

x = list(range(1,9))
print(x)
y = [];
resultf = open('avg', 'a')
for i in x:
	i = 2 ** i
	print(i)
	out_bytes = subprocess.check_output(['../../build/bin/mapreduce_hand', '131072', str(i)])
	out_text = out_bytes.decode('ascii')
	value = out_text.split('\n')[-2]
	value = value.split('\t')[1]
	resultf.write(str(i) + '\t' + str(value) + '\n')

resultf.close()
