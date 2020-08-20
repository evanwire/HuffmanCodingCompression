all: fileCompressor

fileCompressor: fileCompressor.o
	gcc -o fileCompressor fileCompressor.o

fileCompressor.o: fileCompressor.c
	gcc -c fileCompressor.c

clean:
	rm fileCompressor.o fileCompressor
