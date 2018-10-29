CC = gcc
CFLAGS = -masm=intel -no-pie -fno-stack-protector
TARGET = main
OBJECTS = main.o mov.o add.o sub.o xor.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	rm *.o
