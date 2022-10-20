import os

for i in range(10):
	file = "bf0" + str(i)

	try:
		fd = os.open(file, os.O_RDWR | os.O_CREAT)

		os.lseek(fd, 1073741824, os.SEEK_SET)
		os.write(fd, str.encode("X"))

		os.close(fd)
		print(file + ": Success")
	except (OSError, IOError):
		print("Could not open file: " + file)

print("Done!")
