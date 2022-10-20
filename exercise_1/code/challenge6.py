import os

print("Creating pipes..")
r1, w1 = os.pipe()
r2, w2 = os.pipe()

print("Duplicating file descriptors..")
os.dup2(r1, 33)
os.dup2(w1, 34)

os.dup2(r2, 53)
os.dup2(w2, 54)

print("Executing riddle..")
pid = os.fork()
if pid == 0:
	os.execve("./riddle", ["./riddle"], os.environ.copy())
	exit()
waitpid(pid, 0)

print("\nClosing file descriptors..")

for fd in [r1, w1, r2, w2, 33, 34, 53, 54]:
	os.close(fd)

print("Done!")
