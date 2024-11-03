#ifndef LIB_UTILS_FFT_H_
#define LIB_UTILS_FFT_H_

#include <cmath>
#include <algorithm>
#include <fftw3.h> // NOLINT [include_order]
#include <vector>

namespace fft
{

using RealArray = std::vector<long double>;

class FFT
{
public:
    template <typename Iterable> RealArray RFFT(const Iterable &input)
    {
        // If input size is 0, return false.
        if (input.empty())
        {
            throw std::invalid_argument("Input size cannot be 0");
        }

        std::size_t input_size = input.size();
        RealArray real_output(input_size / 2 + 1);

        double *in = fftw_alloc_real(input_size);
        fftw_complex *out = fftw_alloc_complex(input_size / 2 + 1);

        // Copy and convert the input data to double
        for (std::size_t i = 0; i < input_size; i++)
        {
            in[i] = static_cast<double>(input[i]);
        }

        fftw_plan plan = fftw_plan_dft_r2c_1d(input_size, in, out, FFTW_ESTIMATE);
        fftw_execute(plan);

        double real_val = 0.0;
        double imag_val = 0.0;
        const double min_val = 1e-10;
        const double scale_factor = 1.0 / (1 << 17);

        // do max((real^2 + imag^2) / (1 << 17), 0.0000000001)
        for (std::size_t i = 0; i < input_size / 2 + 1; ++i)
        {
            real_val = out[i][0];
            imag_val = out[i][1];

            real_val = (real_val * real_val + imag_val * imag_val) * scale_factor;
            real_output[i] = (real_val < min_val) ? min_val : real_val;
        }

        // Clean up
        fftw_destroy_plan(plan);
        fftw_free(in);
        fftw_free(out);

        return real_output;
    }

    virtual ~FFT()
    {
        fftw_cleanup();
    }
};
} // namespace fft

#endif // LIB_UTILS_FFT_H_
