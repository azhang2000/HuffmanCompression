all: fileCompressor.c
	gcc fileCompressor.c -o fileCompressor -g
clean: 
	rm fileCompressor
