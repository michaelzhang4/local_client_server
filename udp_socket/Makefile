COMPILE=gcc -g -o

compile_server:
	$(COMPILE) server.exe server.c

server: compile_server
	./server.exe

servergdb: compile_server
	gdb server.exe

compile_client:
	$(COMPILE) client.exe client.c

client: compile_client
	./client.exe

clientgdb: compile_client
	gdb client.exe

clean:
	rm *.exe
	
