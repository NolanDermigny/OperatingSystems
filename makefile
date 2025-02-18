wish: main.o wish.o
	gcc -o wish main.o wish.o

main.o: wish.c
	gcc -c main.c

wish.o: wish.c
	gcc -c wish.c
