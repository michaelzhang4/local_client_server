COMPILE=gcc -g -o

compile_single_server:
	$(COMPILE) singleserver.exe singleserver.c

singleserver: compile_single_server
	./singleserver.exe

singleservergdb: compile_single_server
	gdb ./singleserver.exe

compile_single_client:
	$(COMPILE) singleclient.exe singleclient.c

singleclient: compile_single_client
	./singleclient.exe

singleclientgdb: compile_single_client
	gdb ./singleclient.exe

compile_multiserver:
	$(COMPILE) multiserver.exe multiserver.c

multiserver: compile_multiserver
	./multiserver.exe

multiservergdb: compile_multiserver
	gdb ./multiserver.exe

compile_multiclient:
	$(COMPILE) multiclient.exe multiclient.c

multiclient: compile_multiclient
	./multiclient.exe

multiclientgdb: compile_multiclient
	gdb ./multiclient.exe

clean:
	rm *.exe

