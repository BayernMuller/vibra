#ifndef FFT_H
#define FFT_H
#include <vector>

class FFT
{
public:
    using RealArray = std::vector<long double>;
    static bool RFFT(const FFT::RealArray& input, FFT::RealArray& real, FFT::RealArray& imag);
};

#endif // FFT_H