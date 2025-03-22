CC=gcc
OBJS= ./src/alu.o ./src/acu.o ./src/cu.o ./src/memory.o ./src/register.o ./src/cpu.o
LIBS= ./src/cycle.a
CFLAGS=-Wall

test: main
	./main
	make clean

main: $(LIBS)
	$(CC) $(CFLAGS) -o main main.c -L. $(LIBS)

$(LIBS): $(OBJS)
	$(AR) rv $(LIBS) $(OBJS)
		
./src/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
		
clean:
	rm -f $(OBJS) ./src/cycle.a main