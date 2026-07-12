#ifndef LIB_ALGORITHM_SIGNATURE_H_
#define LIB_ALGORITHM_SIGNATURE_H_

#include <array>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "algorithm/frequency.h"

// Prevent Structure Padding
#ifdef _MSC_VER
#pragma pack(push, 1)
#define PACKED_ATTRIBUTE
#else
#define PACKED_ATTRIBUTE __attribute__((packed))
#endif

namespace vibra {

struct RawSignatureHeader {
  uint32_t magic1;
  uint32_t crc32;
  uint32_t size_minus_header;
  uint32_t magic2;
  std::array<uint32_t, 3> void1;
  uint32_t shifted_sample_rate_id;
  std::array<uint32_t, 2> void2;
  uint32_t number_samples_plus_divided_sample_rate;
  uint32_t fixed_value;
} PACKED_ATTRIBUTE;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

struct SignatureData {
  std::uint32_t sample_rate;
  std::uint32_t num_samples;
};

class Signature {
 public:
  explicit Signature(const SignatureData& data);
  ~Signature() = default;
  void Reset(const SignatureData& data);

  inline void AddNumSamples(std::uint32_t num_samples) {
    num_samples_ += num_samples;
  }
  inline std::uint32_t sample_rate() const { return sample_rate_; }
  inline std::uint32_t num_samples() const { return num_samples_; }
  inline std::map<FrequencyBand, std::list<FrequencyPeak>>&
  frequency_band_to_peaks() {
    return frequency_band_to_peaks_;
  }
  std::uint32_t SumOfPeaksLength() const;
  std::string EncodeBase64() const;

 private:
  template <typename T>
  std::stringstream& writeLittleEndian(std::stringstream& stream, T value,
                                       std::size_t size = sizeof(T)) const {
    for (std::size_t i = 0; i < size; ++i) {
      stream << static_cast<char>(value >> (i << 3));
    }
    return stream;
  }

 private:
  std::uint32_t sample_rate_ = 0;
  std::uint32_t num_samples_ = 0;
  std::map<FrequencyBand, std::list<FrequencyPeak>> frequency_band_to_peaks_;
};

}  // namespace vibra

#endif  // LIB_ALGORITHM_SIGNATURE_H_
