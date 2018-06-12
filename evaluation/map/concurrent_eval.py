#!/usr/bin/env python3
import subprocess
import numpy as np
import matplotlib.pyplot as plt

x = list(range(1,21))
rlt_file = open("concurrent_eval.rlt", 'w')
print(x)
y = [];
for i in x:
	print(i)
	out_bytes = subprocess.check_output(['../../build/bin/concurrent_eval', str(i)])
	out_text = out_bytes.decode('ascii')
	result = out_text.split('\n')[-2]
	result = result.split('\t')[1]
	result = float(result)
	result = 65536/result
	y.append(result)
	rlt_file.write(str(i)+'\t' + str(result)+'\n')

plt.figure()
plt.plot(x,y, "-")
for i in x:
	plt.annotate(str(y[i-1]),xy=(i,y[i-1]))

plt.xlabel('Max Number of Concurrent Works')
plt.ylabel('Performance / Flops')
plt.savefig("plot.pdf")
plt.show()
rlt_file.close()
