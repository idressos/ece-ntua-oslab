import os

print("Creating file descriptor..")
os.dup2(1, 99);

print("Executing riddle..")
pid = os.fork()
if pid == 0:
	os.execve("./riddle", ["./riddle"], os.environ.copy())
	exit()
os.waitpid(pid, 0)

print("\nClosing file descriptor..")
os.close(99);

print("Done!")
