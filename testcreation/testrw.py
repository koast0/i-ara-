import subprocess

for i in range(1, 11):
	size = i*10;
	for j in [0.03, 0.07, 0.11, 0.15]:
		f = open("tests/dataw"+str(i)+"_"+ str(j)+'.txt', 'w')
		way = "tests/dataw"+str(i)+"_"+ str(j)+'.txt'
		res = "tests/randomw"+str(i)+"_"+ str(j)+'.txt'
		f.write(str(size)+ ' ' + str(size)+' '+str(j))
		f.close();
		p = subprocess.Popen("./walls.out <"+way+" >"+res,  shell=True)
