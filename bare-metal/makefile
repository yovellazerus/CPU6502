CC65 = cc65.exe
CA65 = ca65.exe
LD65 = ld65.exe

all: main.bin

main.s: main.c
	$(CC65) -Oirs -t none -Cl --add-source main.c

main.o: main.s
	$(CA65) main.s

crt0.o: crt0.s
	$(CA65) crt0.s

main.bin: linker.cfg crt0.o main.o
	$(LD65) -C linker.cfg -o main.bin crt0.o main.o

clean:
	rm -f *.o main.bin
