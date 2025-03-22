CC=gcc
OBJS= ./src/alu.o ./src/acu.o ./src/cu.o ./src/memory.o ./src/register.o ./src/cpu.o
LIBS= ./src/cycle.a
CFLAGS=-Wall

main: $(LIBS) ./src/predictor.o
	$(CC) $(CFLAGS) -o main main.c ./src/predictor.o -L. $(LIBS)
	rm -f $(OBJS) ./src/cycle.a ./src/predictor.o

./src/predictor.o: ./src/predictor.c
	$(CC) $(CFLAGS) -c ./src/predictor.c -o ./src/predictor.o

$(LIBS): $(OBJS)
	$(AR) rv $(LIBS) $(OBJS)
		
./src/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
		
clean:
	rm -f $(OBJS) ./src/cycle.a main ./src/predictor.o