all: opencv_test.pgr

CC=g++
CFLAGS=-Wall -O3
LDFLAGS= -fopenmp

CFLAGS += $(shell pkg-config --cflags opencv4)
LDFLAGS += $(shell pkg-config --libs opencv4)

opencv_test.pgr: obj/opencv_simple.o obj/gaussianNoise.o obj/gauss_seidel.o obj/stopwatch.o
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

obj/%.o: %.cpp
	@mkdir -p ./obj
	$(CC) $(CFLAGS) $< -c -o $@

proper :
	rm -f obj/*.o

clean:
	rm -rf obj/*.o *.pgr
