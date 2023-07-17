CC = g++                

all: bmatch       

bmatch: iccad_A.cpp        
	$(CC) -o bmatch iccad_A.cpp

clean:                
	rm -f bmatch
