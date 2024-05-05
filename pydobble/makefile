CC = gcc
CFLAGS = -I. -g -Wall
LDFLAGS = 

BIN_NAME = cdobble
SO_NAME = cdobble.so

PY_VERSION = $(shell python3 --version | grep -Po "\d+\.\d+")

$(SO_NAME): dobblegen.c poly.o
	$(CC) $(CFLAGS) -o $(SO_NAME) -I/usr/include/python$(PY_VERSION) -fPIC -shared -DPY_MOD=1 $(LDFLAGS) -lpython$(PY_VERSION) dobblegen.c frac.o poly.o

$(BIN_NAME): dobblegen.c poly.o
	$(CC) $(CFLAGS) -o $(BIN_NAME) $(LDFLAGS) dobblegen.c frac.o poly.o

frac.o: frac.c frac.h
	$(CC) -c $(CFLAGS) frac.c

poly.o: poly.c poly.h frac.o
	$(CC) -c $(CFLAGS) poly.c

clean:
	rm *.o
	rm $(BIN_NAME)
	rm $(SO_NAME)
