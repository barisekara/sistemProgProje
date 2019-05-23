CC=gcc
LINK=gcc
CFLAGS=-c -Wall -I.
OBJECT_FILES= connection.o homeClient.o

.SUFFIXES: .c .o

.c .o:
	$(CC) $^ -o $@ $<

appClient: $(OBJECT_FILES)
	$(LINK) $(OBJECT_FILES) -o $@

#Dependences
RemoteSystemAPI.o: connection.c connection.h
homeClient.o: connection.h homeClient.c config.h