import os
import sys

if len(sys.argv) != 2:
	print("Usage: python3 challenge12.py <CHAR>")
	exit()

tempfiles = []

for(dirpath, dirnames, filenames) in os.walk("/tmp"):
	tempfiles = list(filter(lambda f: f.startswith("riddle-"), filenames))
	break

if len(tempfiles) == 1:
	file = "/tmp/" + tempfiles[0]

	try:
		fd = os.open(file, os.O_WRONLY)

		os.lseek(fd, 111, os.SEEK_SET)
		os.write(fd, str.encode(argv[1]))
		os.close(fd)

		print("Success!")
	except (OSError, IOError):
		print("Could not open file: " + file)

elif len(tempfiles) == 0:
	print("No temp riddle files found")
	exit()
else:
	print("Found multiple temp riddle files - please clean up the /tmp directory")
	exit()
