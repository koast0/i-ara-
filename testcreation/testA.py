import glob
import subprocess
import resource
f = open('timeA.txt', 'w')
to_test = glob.glob("./tests/lab*.txt")
to_test += glob.glob("./tests/ran*.txt")
to_test.sort()
for i in to_test:
	p = subprocess.Popen("time python3 A.py <"+i,stderr=subprocess.PIPE, shell=True)
	stderr = str(p.communicate()[1])[2:8]
	f.write(i[8:-4]+' ' +stderr+'\n')
f.close()