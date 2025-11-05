CC65 = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-2.19\bin\cc65.exe
CA65 = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-2.19\bin\ca65.exe
LD65 = C:\Users\yovel\Desktop\VScode\CPU6502\cc65-2.19\bin\ld65.exe

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
	rm -f *.o *.s main.bin
