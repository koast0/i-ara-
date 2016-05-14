import subprocess

for i in range(1, 11):
	size = i*10;
	for j in [0.05, 0.15, 0.25, 0.35]:
		f = open("tests/data"+str(i)+"_"+ str(j)+'.txt', 'w')
		way = "tests/data"+str(i)+"_"+ str(j)+'.txt'
		res = "tests/random"+str(i)+"_"+ str(j)+'.txt'
		f.write(str(size)+ ' ' + str(size)+' '+str(j))
		f.close();
		p = subprocess.Popen("./rp.o <"+way+" >"+res,  shell=True)
