ring_buffer_test.out : ring_buffer.o ring_buffer_test.o
	g++ -o ring_buffer_test.out ring_buffer.o ring_buffer_test.o

wav_test.out : wav.o wav_test.o
	g++ -o wav_test.out wav.o wav_test.o

wav.o : ../audio/wav.cpp ../audio/wav.h
	g++ -c ../audio/wav.cpp

wav_test.o : wav_test.cpp

ring_buffer.o : ../algorithm/ring_buffer.cpp ../algorithm/ring_buffer.h
	g++ -c ../algorithm/ring_buffer.cpp

ring_buffer_test.o : ring_buffer_test.cpp

clean :
	rm -f *.o *.out