import glob
import subprocess
import resource
to_test = glob.glob("./tests/lab*.txt")
to_test += glob.glob("./tests/ran*.txt")
to_test.sort()
for i in to_test:
	res = i[:-4]+".out"
	p = subprocess.Popen(".:./i-ara.out "+i+" >"+res, stderr=subprocess.PIPE, shell=True)
