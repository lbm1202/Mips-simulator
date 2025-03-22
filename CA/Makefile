main: calculator.o load.o fetch.o decode.o execute.o util.o cal_io.o init.o intergrity_check.o
	gcc -O2 -o main calculator.o load.o fetch.o decode.o execute.o util.o cal_io.o init.o intergrity_check.o

calculator.o: calculator.c
	gcc -O2 -c -o calculator.o calculator.c

load.o: load.c
	gcc -O2 -c -o load.o load.c

fetch.o: fetch.c
	gcc -O2 -c -o fetch.o fetch.c

decode.o: decode.c
	gcc -O2 -c -o decode.o decode.c

execute.o: execute.c
	gcc -O2 -c -o execute.o execute.c

util.o: util.c
	gcc -O2 -c -o util.o util.c

cal_io.o: cal_io.c
	gcc -O2 -c -o cal_io.o cal_io.c

init.o: init.c
	gcc -O2 -c -o init.o init.c

intergrity_check.o: intergrity_check.c
	gcc -O2 -c -o intergrity_check.o intergrity_check.c

clean:	
	rm *.o main
