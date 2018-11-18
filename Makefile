CC = gcc
CFLAGS = -masm=intel -no-pie -fno-stack-protector
TARGET = main
OBJECTS = main.o parse.o nop.o push.o pop.o mov.o movsx.o movsxd.o movzx.o cdqe.o lea.o add.o sub.o xor.o jmp.o je.o jne.o jg.o jge.o ja.o jae.o jl.o jle.o jb.o jbe.o jo.o jno.o jz.o jnz.o js.o jns.o shl.o shr.o cmp.o test.o call.o div.o imul.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	rm *.o
