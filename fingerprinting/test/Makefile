all: wav_test.out ring_buffer_test.out fft_test.out

ring_buffer_test.out : ring_buffer.o ring_buffer_test.o
	g++ -o ring_buffer_test.out ring_buffer.o ring_buffer_test.o

wav_test.out : wav.o wav_test.o
	g++ -o wav_test.out wav.o wav_test.o

fft_test.out : fft.o fft_test.o
	g++ -o fft_test.out fft.o fft_test.o

wav.o : ../audio/wav.cpp ../audio/wav.h
	g++ -c ../audio/wav.cpp

wav_test.o : wav_test.cpp

ring_buffer.o : ../utils/ring_buffer.cpp ../utils/ring_buffer.h
	g++ -c ../utils/ring_buffer.cpp

ring_buffer_test.o : ring_buffer_test.cpp

fft_test.o : fft_test.cpp
	g++ -c fft_test.cpp -std=c++14

fft.o : ../utils/fft.cpp ../utils/fft.h
	g++ -c ../utils/fft.cpp

clean :
	rm -f *.o *.out