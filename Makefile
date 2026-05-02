CC = gcc
CFLAGS = -nostdlib -ffreestanding -fno-stack-protector -static
TARGET = minls
SRCS = start.S minls.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
