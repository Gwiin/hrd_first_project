carSerial : main.o serial.o
	cc -o carSerial main.o serial.o

main.o: main.c
	cc -c main.c

serial.o: serial.c serial.h
	cc -c serial.c

clean:
	rm carSerial main.o serial.o