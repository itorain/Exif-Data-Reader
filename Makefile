#define which compiler to use
CC=g++ -std=c++11
#define compile-time flags
FLAGS=-I. -c -O3
DEBUG=-I. -c -g
DEPS=ExifData.h
OBJS = main.o ExifData.o

all: exifRead

exifRead: $(OBJS)
	$(CC) $(OBJS) -o exifRead

ExifData.o: ExifData.cpp $(DEPS)
	$(CC) $(DEBUG) ExifData.cpp

main.o: main.cpp
	$(CC) $(DEBUG) main.cpp

clean:
	-rm -f *.o exifRead *.tar *~

tar:
	tar cf Challenge3.tar *.cpp *.h Makefile
