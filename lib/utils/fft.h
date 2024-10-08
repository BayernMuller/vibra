#ifndef FFT_H
#define FFT_H

#include <vector>
#include <cmath>
#include <fftw3.h>
#include <algorithm>

namespace fft
{
    using RealArray = std::vector<long double>;

    class FFT
    {
    public:
        template <typename Iterable>
        static bool RFFT(const Iterable& input, RealArray* real);
    };

    template <typename Iterable>
    bool FFT::RFFT(const Iterable& input, RealArray* real)
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
        real->resize(N/2 + 1);

        double real_val = 0.0;
        double imag_val = 0.0;
        const double min_val = 0.0000000001;
        const double scale_factor = 1.0 / (1 << 17);

        // do max((real^2 + imag^2) / (1 << 17), 0.0000000001)
        for (size_t i = 0; i < N/2 + 1; ++i)
        {
            real_val = out[i][0];
            imag_val = out[i][1];

            real_val = (real_val * real_val + imag_val * imag_val) * scale_factor;
            (*real)[i] = (real_val < min_val) ? min_val : real_val;
        }

        // Clean up
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);

        return true;
    }

} // namespace fft

#endif // FFT_H