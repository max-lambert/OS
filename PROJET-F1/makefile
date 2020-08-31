CC=gcc -Wall -Werror -std=gnu99

all: rng.o file.o car.o display.o main.o
	$(CC) -o main main.o rng.o car.o file.o display.o

main.o: main.c
	$(CC) -c main.c

display.o: display.c
	$(CC) -c display.c

file.o: file.c
	$(CC) -c file.c

rng.o: rng.c
	$(CC) -c rng.c

car.o: car.c
	$(CC) -c car.c

tex: rapportOS.tex
	pdflatex rapportOS.tex

clean:
	rm -f *.o manageTime voiture utilitaire main *.gch rapportOS.out rapportOS.aux rapportOS.log rapportOS.toc *.txt main
