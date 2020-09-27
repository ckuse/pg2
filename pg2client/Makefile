CC		= gcc
CFLAGS	= -g -std=gnu99

SOURCES = client.c
OBJS = client.o 
TARGETS = myftp 

%.o:	%.c $(DEPS)
	@echo "Compiling $@"
	$(CC) $(CFLAGS) -c -o $@ $<

myftp: $(OBJS)
	@echo "Compiling client" 
	$(CC) -o $@ client.o $(CFLAGS)

all: $(TARGETS)

clean:
	@echo "cleaning... "
	rm -f $(OBJS) $(TARGETS)
