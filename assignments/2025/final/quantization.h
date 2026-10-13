#ifndef CNN_QUANTIZATION_H
#define CNN_QUANTIZATION_H
#include <cmath>

// E4M3 normal numbers, exponent bias 7; no subnormal support.
inline unsigned char encode_fp8(float value) {
    if (value == 0 || std::fabs(value) < 0.015625f) return 0;
    int sign = value < 0 ? 128 : 0, exponent;
    float fraction = std::frexp(std::fabs(value), &exponent) * 2;
    int field = exponent - 1 + 7;
    int mantissa = static_cast<int>(std::floor((fraction - 1) * 8 + 0.5f));
    if (mantissa == 8) { mantissa = 0; ++field; }
    if (field >= 15) return static_cast<unsigned char>(sign | 119);
    return static_cast<unsigned char>(sign | (field << 3) | mantissa);
}
inline float decode_fp8(unsigned char bits) {
    int exponent = (bits >> 3) & 15, mantissa = bits & 7;
    if (exponent == 0) return 0;
    float value = std::ldexp(1.0f + mantissa / 8.0f, exponent - 7);
    return bits & 128 ? -value : value;
}
inline float quantize(float value) { return decode_fp8(encode_fp8(value)); }
#endif
