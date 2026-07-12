#ifndef LIB_UTILS_BASE64_H_
#define LIB_UTILS_BASE64_H_

#include <array>
#include <string>

namespace vibra {

namespace base64 {
constexpr const char* kBase64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
inline std::string Encode(const char* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  std::array<unsigned char, 3> char_array_3 = {};
  std::array<unsigned char, 4> char_array_4 = {};

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] =
          ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] =
          ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (i = 0; (i < 4); i++) {
        ret += kBase64Chars[char_array_4[i]];
      }
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) {
      char_array_3[j] = '\0';
    }

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] =
        ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] =
        ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++) {
      ret += kBase64Chars[char_array_4[j]];
    }

    while ((i++ < 3)) {
      ret += '=';
    }
  }

  return ret;
}
}  // namespace base64

}  // namespace vibra

#endif  // LIB_UTILS_BASE64_H_
