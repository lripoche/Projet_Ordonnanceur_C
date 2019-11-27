FLAGS=-W -Wall -pedantic

all: main clean

main: MySemaphore.o projet.o
	gcc -o projet MySemaphore.o projet.o $(FLAGS) -lpthread

MySemaphore.o: MySemaphore.c
	gcc -o MySemaphore.o -c MySemaphore.c $(FLAGS)

projet.o: projet.c MySemaphore.h
	gcc -o projet.o -c projet.c $(FLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf projet