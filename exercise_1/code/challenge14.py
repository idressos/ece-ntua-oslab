import os
import fcntl

if os.geteuid() != 0:
	print("Program requires root privileges")
	exit()

file = "/proc/sys/kernel/ns_last_pid"

try:
	fd = os.open(file, os.O_RDWR | os.O_CREAT)

	fcntl.flock(fd, fcntl.LOCK_EX)
	print("Acquired exclusive lock on file: " + file)

	print("Manipulating riddle execution PID..")
	os.write(fd, str.encode("32766"))

	print("Executing riddle..")
	pid = os.fork()
	if pid == 0:
		os.execve("./riddle", ["./riddle"], os.environ.copy())
		exit()
	os.waitpid(pid, 0)

	print("\nReleasing exclusive lock on file: " + file)
	fcntl.flock(fd, fcntl.LOCK_UN)

	os.close(fd)

	print("Done!")
except (OSError, IOError):
	print("An unexpected error occured")
