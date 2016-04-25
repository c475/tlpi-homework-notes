import subprocess
import os

for i in range(1, 65):
	if i < 10:
		i = '0' + str(i)
	else:
		i = str(i)

	try:
		os.mkdir('chapter-' + i + '/bin')
	except:
		pass

	f = open('chapter-' + i + '/Makefile', 'w')
	f.write("CC=cc\n\nCFLAGS=-Wall -g\n\nOUTPUT=\"bin/$(OUT)\"\nDIRECT=$(D)\nFILE=$(FILE)\n\nSOURCES=$(wildcard $(DIRECT)/$(FILE).c ../lib/*.c)\nHEADERS=$(wildcard $(DIRECT)/$(FILE).h ../lib/*.h)\n\n\ntarget:\n\t$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)\n")
	f.close()
