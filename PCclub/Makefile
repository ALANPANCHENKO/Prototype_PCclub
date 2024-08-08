CC = gcc
CFLAGS = -Wall -I.

SRCS = type.c main.c client.c second_utility.c
OBJS = $(SRCS:.c=.o)

TARGET = PcClub

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
