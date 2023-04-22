wav_test.out : wav.o wav_test.o
	g++ -o wav_test.out wav.o wav_test.o

wav.o : ../audio/wav.cpp ../audio/wav.h
	g++ -c ../audio/wav.cpp

wav_test.o : wav_test.cpp

clean :
	rm -f *.o *.out