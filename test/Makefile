CC = gcc
CFLAGS = -no-pie -fno-stack-protector
TARGET = cal
OBJECTS = cal.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	rm *.o
