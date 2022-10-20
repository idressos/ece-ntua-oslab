import os

file = ".hello_there"

try:
	fd = os.open(file, os.O_RDWR)

	os.ftruncate(fd, 32768)
	os.close(fd)

	print("Successfully truncated file: " + file)
except (OSError, IOError):
	print("Could not open file: " + file)
