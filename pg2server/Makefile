CC		= gcc
CFLAGS	= -g -std=gnu99

SOURCES = server.c
OBJS = server.o
TARGETS = myftp

%.o:	%.c $(DEPS)
	@echo "Compiling $@"
	$(CC) $(CFLAGS) -c -o $@ $<

myftp: $(OBJS)
	@echo "Compiling server"
	$(CC) -o $@ server.o $(CFLAGS)

all: $(TARGETS)

clean:
	@echo "cleaning... "
	rm -f $(OBJS) $(TARGETS)
