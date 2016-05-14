import subprocess
import resource


for i in range(1, 22):
	size = i*10;
	f = open("tests/data"+str(i)+'.txt', 'w')
	way = "tests/data"+str(i)+'.txt'
	res = "tests/lab"+str(i)+'.txt'
	f.write(str(size)+ ' ' + str(size))
	f.close;
	p = subprocess.Popen("./lab.o <"+way+" >"+res, shell=True)
	