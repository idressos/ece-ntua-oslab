import socket

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.bind(("127.0.0.1", 49842))
	s.listen()

	conn, addr = s.accept()

	with conn:
		print(f"{addr} connected")

		while True:
			data = conn.recv(1024)
			if not data:
				print("Connection closed")
				break

			q = data.decode("UTF-8")
			expr = q[12:len(q) - 2]

			try:
				conn.sendall(str.encode(str(eval(expr))))
				print("Expression evaluated! Result sent through socket.")
			except SyntaxError:
				print("Could not evaluate expression: " + expr)
