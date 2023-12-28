#ifndef FFT_H
#define FFT_H
#include <vector>
#include <cmath>
#include <fftw3.h>

class FFT
{
public:
    using RealArray = std::vector<long double>;

    template <typename Iterable>
    static bool RFFT(const Iterable& input, FFT::RealArray& real, FFT::RealArray& imag);
};

template <typename Iterable>
bool FFT::RFFT(const Iterable& input, RealArray& real, RealArray& imag)
{
    // If input size is 0, return false.
    if (input.size() == 0)
    {
        return false;
    }

    size_t N = input.size();
    fftw_complex *out;
    fftw_plan p;

    // Allocate memory for the input and output arrays
    double* in = fftw_alloc_real(N);
    out = fftw_alloc_complex(N/2 + 1);

    // Copy and convert the input data to double
    std::transform(input.begin(), input.end(), in, [](const typename Iterable::value_type& val) {
        return static_cast<double>(val);
    });

    // Create the plan
    p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

    // Execute the plan
    fftw_execute(p);

    // Assign results directly
    real.resize(N/2 + 1);
    imag.resize(N/2 + 1);
    for (size_t i = 0; i < N/2 + 1; ++i)
    {
        real[i] = out[i][0]; // Real part
        imag[i] = out[i][1]; // Imaginary part
    }

    // Clean up
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    return true;
}


#endif // FFT_H