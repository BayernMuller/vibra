#include "fft.h"
#include <cmath>

bool FFT::RFFT(const FFT::RealArray& input, FFT::RealArray& real, FFT::RealArray& imag)
{
    // If input size is not power of 2, It doens't matter. Just ignore the last element.
    // If input size is 0, return false.

    // check input size
    if (input.size() == 0)
    {
        return false;
    }

    // prepare output
    real.resize(input.size() / 2 + 1);
    imag.resize(input.size() / 2 + 1);

    double degree = 0.0;
    // do FFT of first half
    for (std::size_t i = 0; i < input.size() / 2 + 1; ++i)
    {
        for (std::size_t j = 0; j < input.size(); ++j)
        {  
            degree = 2.0 * M_PI * i * j / input.size();
            real[i] += +input[j] * std::cos(degree);
            imag[i] += -input[j] * std::sin(degree);
        }
    }


    return true;
}
