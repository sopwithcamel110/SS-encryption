CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
EXEC = keygen encrypt decrypt
CLEAN = keygen.o encrypt.o decrypt.o ss.pub ss.priv
OBJS = ss.o numtheory.o randstate.o
LN = -lgmp

all: $(EXEC)

keygen : $(OBJS) keygen.o
	$(CC) -o keygen keygen.o $(OBJS) $(LN)

encrypt: $(OBJS) encrypt.o
	$(CC) -o encrypt encrypt.o $(OBJS) $(LN)

decrypt: $(OBJS) decrypt.o
	$(CC) -o decrypt decrypt.o $(OBJS) $(LN)

$.o : %.c
	$(CC) $(CFLAGS) -c $<

clean: 
	rm -f $(EXEC) $(CLEAN) $(OBJS)

format:
	clang-format -i -style=file *.[ch]
