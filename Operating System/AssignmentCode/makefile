CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
OBJ = queue.o cq.o threads.o
EXEC = cq

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

queue.o :queue.c queue.h
	$(CC) -c queue.c $(CFLAGS)

threads.o :threads.c threads.h queue.h cq.h
	$(CC) -c threads.c $(CFLAGS)

cq.o :cq.c cq.h queue.h threads.h
	$(CC) -c cq.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)
