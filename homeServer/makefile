CC=gcc
LINK=gcc
CFLAGS=-c -Wall -I.
OBJECT_FILES= gpioDIO.o Keypad.o homeServer.o

.SUFFIXES: .c .o

.c .o:
	$(CC) $^ -o $@ $<

appServer: $(OBJECT_FILES)
	$(LINK) $(OBJECT_FILES) -o $@ -lpthread

#Dependences
gpioDIO.o: gpioDIO.c gpioDIO.h gpioTypes.h
Keypad.o: Keypad.c gpioDIO.h
homeServer.o: Keypad.h homeServer.c gpioDIO.h config.h
