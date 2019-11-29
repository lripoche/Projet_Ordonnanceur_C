FLAGS=-W -Wall -pedantic

all: main clean

main: semaphore.o main.o allocCPU.o randomProcessus.o readfile.o utils.o rrAlgorithm.o
	gcc -o program main.o semaphore.o readfile.o utils.o allocCPU.o rrAlgorithm.o randomProcessus.o $(FLAGS) -lpthread

semaphore.o: semaphore.c
	gcc -o semaphore.o -c semaphore.c $(FLAGS)

allocCPU.o: allocCPU.c header.h
	gcc -o allocCPU.o -c allocCPU.c $(FLAGS)

randomProcessus.o: randomProcessus.c header.h
	gcc -o randomProcessus.o -c randomProcessus.c $(FLAGS)

readfile.o: readfile.c header.h
	gcc -o readfile.o -c readfile.c $(FLAGS)

utils.o: utils.c header.h
	gcc -o utils.o -c utils.c $(FLAGS)

rrAlgorithm.o: rrAlgorithm.c header.h
	gcc -o rrAlgorithm.o -c rrAlgorithm.c $(FLAGS)

main.o: main.c semaphore.h header.h
	gcc -o main.o -c main.c $(FLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf projet
