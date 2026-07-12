#ifndef LIB_UTILS_FFT_H_
#define LIB_UTILS_FFT_H_

#include <array>
#include <cassert>
#include <cmath>

#include <kiss_fftr.h>  // NOLINT [include_order]
#include <algorithm>

namespace vibra {

namespace fft {

template <int INPUT_SIZE>
class FFT {
 public:
  constexpr static const int kOutputSize = INPUT_SIZE / 2 + 1;
  using FFTOutput = std::array<double, kOutputSize>;

 public:
  FFT() : fft_cfg_(kiss_fftr_alloc(INPUT_SIZE, 0, nullptr, nullptr)) {
    assert(fft_cfg_ != nullptr && "Failed to allocate kissfft config");
  }
  FFT(const FFT&) = delete;
  FFT& operator=(const FFT&) = delete;
  FFT(FFT&&) = delete;
  FFT& operator=(FFT&&) = delete;

  template <typename Iterable>
  FFTOutput RFFT(const Iterable& input) {
    assert(input.size() == INPUT_SIZE &&
           "Input size must be equal to the input size specified in the "
           "constructor");

    FFTOutput real_output;

    for (std::size_t i = 0; i < INPUT_SIZE; i++) {
      input_data_buffer_[i] = static_cast<kiss_fft_scalar>(input[i]);
    }
    kiss_fftr(fft_cfg_, input_data_buffer_.data(), output_data_buffer_.data());

    double real_val = 0.0;
    double imag_val = 0.0;
    const double kMinVal = 1e-10;
    const double kScaleFactor = 1.0 / (1 << 17);

    // do max((real^2 + imag^2) / (1 << 17), 0.0000000001)
    for (std::size_t i = 0; i < kOutputSize; ++i) {
      real_val = output_data_buffer_[i].r;
      imag_val = output_data_buffer_[i].i;

      real_val = (real_val * real_val + imag_val * imag_val) * kScaleFactor;
      real_output[i] = (real_val < kMinVal) ? kMinVal : real_val;
    }
    return real_output;
  }

  virtual ~FFT() { kiss_fftr_free(fft_cfg_); }

 private:
  kiss_fftr_cfg fft_cfg_;
  std::array<kiss_fft_scalar, INPUT_SIZE> input_data_buffer_;
  std::array<kiss_fft_cpx, kOutputSize> output_data_buffer_;
};
}  // namespace fft

}  // namespace vibra

#endif  // LIB_UTILS_FFT_H_
