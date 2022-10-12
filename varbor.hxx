/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <bitset>
#include <cmath>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <ranges>
#include <span>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace varbor {
/** Default error type.
 */
class Error : public std::runtime_error {
  public:
    template <class... Args>
        requires std::constructible_from<std::runtime_error, Args...>
    Error(Args &&...t) : runtime_error(std::forward<Args>(t)...) {
    }
};

/** End of input error.
 */
class EndOfInput : public Error {
  public:
    template <class... Args>
        requires std::constructible_from<Error, Args...>
    EndOfInput(Args &&...t) : Error(std::forward<Args>(t)...) {
    }
};

/** Illegal SpecialFloat.
 */
class IllegalSpecialFloat : public Error {
  public:
    template <class... Args>
        requires std::constructible_from<Error, Args...>
    IllegalSpecialFloat(Args &&...t) : Error(std::forward<Args>(t)...) {
    }
};

class InvalidType : public Error {
  public:
    template <class... Args>
        requires std::constructible_from<Error, Args...>
    InvalidType(Args &&...t) : Error(std::forward<Args>(t)...) {
    }
};

/** Tried to extract a count when the count wasn't normal
 */
class SpecialCountError : public Error {
  public:
    template <class... Args>
        requires std::constructible_from<Error, Args...>
    SpecialCountError(Args &&...t) : Error(std::forward<Args>(t)...) {
    }
};

/** The major type read from the header.
 */
enum class MajorType {
    PositiveInteger = 0,
    NegativeInteger = 1,
    ByteString = 2,
    Utf8String = 3,
    Array = 4,
    Map = 5,
    SemanticTag = 6,
    SpecialFloat = 7
};

template <typename T>
inline T from_be_bytes(std::array<std::byte, sizeof(T)> input) {
    T value;
    const auto v_pointer = reinterpret_cast<std::byte *>(&value);

    for (size_t i = 0; i < sizeof(value); ++i) {
        if constexpr (std::endian::native == std::endian::big) {
            v_pointer[i] = input[i];
        } else {
            v_pointer[i] = input[sizeof(value) - 1 - i];
        }
    }

    return value;
}

inline float read_float16(std::array<std::byte, 2> input) {
    const bool sign = (input[0] & std::byte(0b10000000)) != std::byte(0);
    const std::uint8_t exponent =
      static_cast<std::uint8_t>((input[0] & std::byte(0b01111100)) >> 2);
    const std::uint16_t fraction =
      (static_cast<std::uint16_t>(input[0] & std::byte(0b00000011)) << 8) |
      static_cast<std::uint16_t>(input[1]);
    if (exponent == 0) {
        // zero
        if (fraction == 0) {
            if (sign) {
                return -0.0f;
            } else {
                return 0.0f;
            }
        } else {
            // Subnormal.  There probably is a better way of doing this.
            const auto adjusted_fraction =
              static_cast<float>(fraction) / static_cast<float>(1 << 10);
            return (sign ? -1.0f : 1.0f) * adjusted_fraction * std::pow(2.0f, -14);
        }
    } else if (exponent == 0b11111) {
        // infinity
        if (fraction == 0) {
            return (sign ? -1.0f : 1.0f) * std::numeric_limits<float>::infinity();
        } else {
            // NaN
            return std::numeric_limits<float>::quiet_NaN();
        }
    } else {
        const std::int8_t normalized_exponent = static_cast<std::int8_t>(exponent) - 15;
        const auto biased_exponent =
          std::byte(static_cast<std::int16_t>(normalized_exponent) + 127);

        std::array<std::byte, 4> bytes = {std::byte(0), std::byte(0), std::byte(0), std::byte(0)};

        if (sign) {
            bytes[0] = std::byte(0b10000000);
        }
        bytes[0] |= biased_exponent >> 1;

        // Left bit is right bit from exponent.
        bytes[1] = biased_exponent << 7;

        // Most significant 7 bits from 10-bit fraction
        bytes[1] |= std::byte(fraction >> 3);

        // Least significant 3 bits of 10-bit fraction
        bytes[2] = std::byte(fraction << 5);

        return from_be_bytes<float>(bytes);
    }
}

/** The count read from the header.  If the count is 24-27, the extended count
 * field is delivered in one of the last four variants, otherwise, it is simply
 * the first variant.
 *
 * This can't be just a std::optional<std::uint64_t> because SpecialFloat needs
 * to be able to tell whether its contents were a 16, 32, or 64-bit floating
 * point number.
 *
 * From Wikipedia: Short counts of 25, 26 or 27 indicate a following extended
 * count field is to be interpreted as a (big-endian) 16-, 32- or 64-bit IEEE
 * floating point value. These are the same sizes as an extended count, but are
 * interpreted differently. In particular, for all other major types, a 2-byte
 * extended count of 0x1234 and a 4-byte extended count of 0x00001234 are
 * exactly equivalent. This is not the case for floating-point values.
 */
using Count = std::variant<std::uint8_t, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

struct Header {
    MajorType type;
    Count count;
    constexpr Header() noexcept = default;
    // Build explicitly
    constexpr Header(const MajorType type, const Count count) noexcept : type(type), count(count) {
    }

    // Build with indefinite count
    constexpr Header(const MajorType type) noexcept :
        Header(type, Count(std::in_place_index<0>, 31)) {
    }

    // Build with straightforward count
    constexpr Header(const MajorType type, const std::uint64_t simple_count) noexcept : type(type) {
        if (simple_count < 24) {
            count = Count{std::in_place_index<0>, simple_count};
        } else if (simple_count < 0x100ull) {
            count = Count{std::in_place_index<1>, simple_count};
        } else if (simple_count < 0x10000ull) {
            count = Count{std::in_place_index<2>, simple_count};
        } else if (simple_count < 0x100000000ull) {
            count = Count{std::in_place_index<3>, simple_count};
        } else {
            count = Count{std::in_place_index<4>, simple_count};
        }
    }

    constexpr auto operator<=>(const Header &other) const noexcept = default;

    /** Extract a full count from the variant,  throwing an error if the count
     * wouldn't be a legal count or would be ambiguous (like any value in the tiny
     * field of more than 23 other than 31).
     * Returns an empty optional if it was a tiny count of 31, indicating
     * indeterminant.
     */
    constexpr std::optional<std::uint64_t> get_count() const {
        if (count.index() == 0) {
            const auto tinycount = std::get<0>(count);
            if (tinycount < 24) {
                return tinycount;
            } else if (tinycount == 31) {
                return std::nullopt;
            } else {
                throw SpecialCountError("Tiny count would be ambiguous");
            }
        } else {
            return std::visit(
              [](const auto element) {
                  return static_cast<std::uint64_t>(element);
              },
              count);
        }
    }
};

/** Read a single byte, returning an error if input is empty.
 */
template <typename InputIt>
inline std::tuple<InputIt, std::byte> read(InputIt begin, const InputIt end) {
    if (begin == end) {
        throw EndOfInput("Reached end of input early");
    }
    const std::byte output = *begin;
    ++begin;
    return {begin, output};
}

template <typename InputIt>
std::tuple<InputIt, Header> read_header(InputIt begin, const InputIt end) {
    std::byte byte;
    std::tie(begin, byte) = read(begin, end);
    const auto type = static_cast<MajorType>(byte >> 5);
    const auto tinycount = static_cast<std::uint8_t>(byte & std::byte(0b00011111));

    switch (tinycount) {
    case 24: {
        std::tie(begin, byte) = read(begin, end);
        return {begin, Header{type, Count(std::in_place_index<1>, static_cast<uint8_t>(byte))}};
    }

    case 25: {
        std::array<std::byte, 2> count;
        for (size_t i = 0; i < sizeof(count); ++i) {
            std::tie(begin, count[i]) = read(begin, end);
        }
        return {begin, Header{type, Count(std::in_place_index<2>, from_be_bytes<uint16_t>(count))}};
    }

    case 26: {
        std::array<std::byte, 4> count;
        for (size_t i = 0; i < sizeof(count); ++i) {
            std::tie(begin, count[i]) = read(begin, end);
        }
        return {begin, Header{type, Count(std::in_place_index<3>, from_be_bytes<uint32_t>(count))}};
    }

    case 27: {
        std::array<std::byte, 8> count;
        for (size_t i = 0; i < sizeof(count); ++i) {
            std::tie(begin, count[i]) = read(begin, end);
        }
        return {begin, Header{type, Count(std::in_place_index<4>, from_be_bytes<uint64_t>(count))}};
    }

    default: {
        return {begin, Header{type, Count(std::in_place_index<0>, tinycount)}};
    }
    }
}

template <typename T>
inline std::array<std::byte, sizeof(T)> to_be_bytes(const T &value) {
    std::array<std::byte, sizeof(T)> output;
    const auto v_pointer = reinterpret_cast<const std::byte *>(&value);

    for (size_t i = 0; i < sizeof(value); ++i) {
        if constexpr (std::endian::native == std::endian::big) {
            output[i] = v_pointer[i];
        } else {
            output[i] = v_pointer[sizeof(value) - 1 - i];
        }
    }

    return output;
}

/** Write the header.
 */
template <typename OutputIt>
OutputIt write_header(OutputIt output, const Header header) {
    const auto [type, count] = header;

    const auto type_byte = std::byte(static_cast<std::uint8_t>(type) << 5);

    if (count.index() == 0) {
        *output = (type_byte | std::byte(std::get<0>(count)));
        return ++output;
    } else {
        *output = (type_byte | std::byte(23 + count.index()));
        ++output;

        return std::visit(
          [&](const auto &count) {
              return std::ranges::copy(to_be_bytes(count), output).out;
          },
          count);
    }
}

/** Convert from float to float16 only if it can be done losslessly.
 */
constexpr std::optional<std::array<std::byte, 2>> lossless_float16(const float value) {
    switch (std::fpclassify(value)) {
    case FP_ZERO: {
        std::array<std::byte, 2> output = {std::byte(0), std::byte(0)};
        if (std::signbit(value)) {
            output[0] = std::byte(0b10000000);
        }
        return output;
    }
    case FP_INFINITE: {
        std::array<std::byte, 2> output = {std::byte(0b01111100), std::byte(0)};
        if (std::signbit(value)) {
            // Positive or negative infinity
            output[0] |= std::byte(0b10000000);
        }

        return output;
    }
    case FP_NAN: {
        return std::array<std::byte, 2>{{std::byte(0b01111110), std::byte(0)}};
    }
    }
    const auto bytes = to_be_bytes(value);
    const bool sign = (bytes[0] & std::byte(0b10000000)) != std::byte(0);
    const std::uint8_t exponent = static_cast<std::uint8_t>(
      ((bytes[0] & std::byte(0b01111111)) << 1) | ((bytes[1] & std::byte(0b10000000)) >> 7));

    const std::int8_t normalized_exponent = static_cast<std::int16_t>(exponent) - 127;
    const std::uint32_t fraction =
      (static_cast<std::uint32_t>(bytes[1] & std::byte(0b01111111)) << 16) |
      (static_cast<std::uint32_t>(bytes[2]) << 8) | static_cast<std::uint32_t>(bytes[3]);

    if (
      normalized_exponent >= -14 && normalized_exponent <= 15 &&
      ((fraction & 0b00000000001111111111111) == 0)) {
        // float16 5-bit biased exponent.
        const std::uint8_t biased_exponent = normalized_exponent + 15;

        std::array<std::byte, 2> output;
        if (sign) {
            output[0] = std::byte(0b10000000);
        } else {
            output[0] = std::byte(0);
        }
        output[0] |= std::byte(biased_exponent << 2);
        // Left two significant bits of 23-bit integer in fraction
        output[0] |= std::byte(fraction >> 21);
        // Bits 21 through 13 in fraction.  cut off last 13 bits, which remain
        // unused.
        output[1] = std::byte(fraction >> 13);
        return output;
    }

    return std::nullopt;
}

class Value;

/** unique_ptr wrapper that forces comparisons to happen on the values.
 */
class ValuePointer {
  private:
    std::unique_ptr<Value> value;

  public:
    template <class... Args>
    constexpr ValuePointer(Args &&...t) : value(std::forward<Args>(t)...) {
    }

    constexpr operator const std::unique_ptr<Value> &() const noexcept {
        return value;
    }

    constexpr operator std::unique_ptr<Value> &() noexcept {
        return value;
    }

    inline const Value &operator*() const noexcept {
        return *value;
    }

    inline Value &operator*() noexcept {
        return *value;
    }

    inline const Value *operator->() const noexcept {
        return value.get();
    }

    inline Value *operator->() noexcept {
        return value.get();
    }

    inline bool operator==(const ValuePointer &other) const noexcept;
    inline std::strong_ordering operator<=>(const ValuePointer &other) const noexcept;
};

// All the individual types need to be wrapped, because they all need to support
// an encode function, and all need to be ordered by their encoded
// representation.

struct Positive {
    // The raw count value.
    std::uint64_t value = 0;

    constexpr Positive() noexcept = default;

    constexpr Positive(const std::uint64_t value) noexcept : value(value) {
    }

    constexpr operator std::int64_t() const noexcept {
        return static_cast<int64_t>(value);
    }

    constexpr operator std::uint64_t() const noexcept {
        return value;
    }

    constexpr bool is_valid_int64() const noexcept {
        return value < 9223372036854775808ul;
    }

    constexpr bool operator==(const Positive &other) const noexcept = default;
    constexpr auto operator<=>(const Positive &other) const noexcept = default;

    template <typename OutputIt>
    constexpr OutputIt encode(const OutputIt iterator) const {
        return write_header(iterator, Header(MajorType::PositiveInteger, value));
    }
};

/** A struct for negative numbers, needed because cbor's negative numbers can
 * exceed C++ int64_t limits.
 */
struct Negative {
    // The raw count value.
    std::uint64_t count = 0;

    constexpr Negative() noexcept = default;

    constexpr Negative(std::uint64_t count) : count(count) {
    }

    constexpr operator std::int64_t() const noexcept {
        return -1 - static_cast<int64_t>(count);
    }

    constexpr bool is_valid_int64() const noexcept {
        return count < 9223372036854775807ul;
    }

    constexpr bool operator==(const Negative &other) const noexcept = default;
    constexpr auto operator<=>(const Negative &other) const noexcept = default;

    template <typename OutputIt>
    constexpr OutputIt encode(const OutputIt iterator) const {
        return write_header(iterator, Header(MajorType::NegativeInteger, count));
    }
};

/** String wrapper with CBOR ordering rules.
 */
struct ByteString {
    std::vector<std::byte> value;
    template <class... Args>
    inline ByteString(Args &&...t) : value(std::forward<Args>(t)...) {
    }

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt output) const {
        const auto iterator = write_header(output, Header(MajorType::ByteString, value.size()));
        return std::copy(value.begin(), value.end(), iterator);
    }

    constexpr bool operator==(const ByteString &other) const noexcept = default;

    constexpr std::strong_ordering operator<=>(const ByteString &other) const noexcept {
        const auto size_compare = value.size() <=> other.value.size();
        if (std::is_lt(size_compare)) {
            return std::strong_ordering::less;
        } else if (std::is_gt(size_compare)) {
            return std::strong_ordering::greater;
        } else {
            return value <=> other.value;
        }
    }

    constexpr operator const std::vector<std::byte> &() const noexcept {
        return value;
    }

    constexpr operator std::vector<std::byte> &() noexcept {
        return value;
    }
};

/** String wrapper with CBOR ordering rules.
 */
struct Utf8String {
    std::u8string value;
    template <class... Args>
    inline Utf8String(Args &&...t) : value(std::forward<Args>(t)...) {
    }

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt output) const {
        const auto iterator = write_header(output, Header(MajorType::Utf8String, value.size()));
        return std::transform(value.begin(), value.end(), iterator, [](const auto c) {
            return std::byte(c);
        });
    }

    constexpr bool operator==(const Utf8String &other) const noexcept = default;

    constexpr std::strong_ordering operator<=>(const Utf8String &other) const noexcept {
        const auto size_compare = value.size() <=> other.value.size();
        if (std::is_lt(size_compare)) {
            return std::strong_ordering::less;
        } else if (std::is_gt(size_compare)) {
            return std::strong_ordering::greater;
        } else {
            return value <=> other.value;
        }
    }

    constexpr operator const std::u8string &() const noexcept {
        return value;
    }

    constexpr operator std::u8string &() noexcept {
        return value;
    }
};

/** String wrapper with CBOR ordering rules.
 */
struct Array {
    std::vector<ValuePointer> value;

    template <class... Args>
    inline Array(Args &&...t) : value(std::forward<Args>(t)...) {
    }

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt output) const;

    constexpr bool operator==(const Array &other) const noexcept = default;

    constexpr std::strong_ordering operator<=>(const Array &other) const noexcept {
        const auto size_compare = value.size() <=> other.value.size();
        if (std::is_lt(size_compare)) {
            return std::strong_ordering::less;
        } else if (std::is_gt(size_compare)) {
            return std::strong_ordering::greater;
        } else {
            return value <=> other.value;
        }
    }

    constexpr operator const std::vector<ValuePointer> &() const noexcept {
        return value;
    }

    constexpr operator std::vector<ValuePointer> &() noexcept {
        return value;
    }
};

struct Map {
    std::map<ValuePointer, ValuePointer> value;

    template <class... Args>
    inline Map(Args &&...t) : value(std::forward<Args>(t)...) {
    }

    template <typename OutputIt>
    inline OutputIt encode(OutputIt output) const;

    inline bool operator==(const Map &other) const noexcept = default;

    inline std::strong_ordering operator<=>(const Map &other) const noexcept {
        const auto size_compare = value.size() <=> other.value.size();
        if (std::is_lt(size_compare)) {
            return std::strong_ordering::less;
        } else if (std::is_gt(size_compare)) {
            return std::strong_ordering::greater;
        } else {
            return value <=> other.value;
        }
    }

    constexpr operator const std::map<ValuePointer, ValuePointer> &() const noexcept {
        return value;
    }

    constexpr operator std::map<ValuePointer, ValuePointer> &() noexcept {
        return value;
    }
};

struct SemanticTag {
    std::uint64_t id = -1;
    ValuePointer value;

    template <class... Args>
    constexpr SemanticTag(std::uint64_t id, Args &&...t) : id(id), value(std::forward<Args>(t)...) {
    }

    inline bool operator==(const SemanticTag &other) const noexcept = default;
    inline auto operator<=>(const SemanticTag &other) const noexcept = default;

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt output) const;
};

struct Boolean {
    bool value = false;

    constexpr Boolean() noexcept = default;
    constexpr Boolean(const bool value) noexcept : value(value) {
    }

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt iterator) const {
        return write_header(iterator, Header{MajorType::SpecialFloat, value ? 21u : 20u});
    }

    constexpr bool operator==(const Boolean &other) const noexcept {
        return value == other.value || (std::isnan(value) && std::isnan(other.value));
    }

    constexpr std::strong_ordering operator<=>(const Boolean &other) const noexcept {
        std::array<std::byte, 5> first;
        std::array<std::byte, 5> second;
        std::ranges::fill(first, std::byte(0));
        std::ranges::fill(second, std::byte(0));
        encode(first.begin());
        other.encode(second.begin());
        return first <=> second;
    }

    constexpr operator bool() const noexcept {
        return value;
    }
};

struct Null {
    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt iterator) const {
        return write_header(iterator, Header{MajorType::SpecialFloat, 22});
    }

    constexpr bool operator==(const Null &) const noexcept {
        return true;
    }

    constexpr std::strong_ordering operator<=>(const Null &) const noexcept {
        return std::strong_ordering::equal;
    }
};

struct Undefined {
    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt iterator) const {
        return write_header(iterator, Header{MajorType::SpecialFloat, 23});
    }

    constexpr bool operator==(const Undefined &) const noexcept {
        return true;
    }

    constexpr std::strong_ordering operator<=>(const Undefined &) const noexcept {
        return std::strong_ordering::equal;
    }
};

/** Simple floating point wrapper that gives cbor-compatible floating point
 * semantics.
 */
struct Float {
    double value = 0.0;

    constexpr Float() noexcept = default;

    constexpr Float(const double value) noexcept : value(value) {
    }

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt iterator) const {
        static_assert(sizeof(float) == 4, "floats must be 4 bytes");
        static_assert(sizeof(double) == 8, "doubles must be 8 bytes");
        static_assert(
          std::endian::native == std::endian::big || std::endian::native == std::endian::little,
          "mixed endian architectures can not be supported yet");

        const float f = value;

        if (std::isnan(value) || static_cast<double>(f) == value) {
            const auto float16 = lossless_float16(f);
            if (float16) {
                return write_header(
                  iterator,
                  Header{
                    MajorType::SpecialFloat,
                    Count(std::in_place_index<2>, from_be_bytes<std::uint16_t>(*float16))});
            } else {
                return write_header(
                  iterator,
                  Header{
                    MajorType::SpecialFloat,
                    Count(std::in_place_index<3>, from_be_bytes<std::uint32_t>(to_be_bytes(f)))});
            }
        } else {
            return write_header(
              iterator,
              Header{
                MajorType::SpecialFloat,
                Count(std::in_place_index<4>, from_be_bytes<std::uint64_t>(to_be_bytes(value)))});
        }
    }

    constexpr bool operator==(const Float &other) const noexcept {
        return value == other.value || (std::isnan(value) && std::isnan(other.value));
    }

    constexpr std::strong_ordering operator<=>(const Float &other) const noexcept {
        std::array<std::byte, 5> first;
        std::array<std::byte, 5> second;
        std::ranges::fill(first, std::byte(0));
        std::ranges::fill(second, std::byte(0));
        encode(first.begin());
        other.encode(second.begin());
        return first <=> second;
    }

    constexpr operator double() const noexcept {
        return value;
    }
};

/** Special break value.
 */
struct Break {
    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt iterator) const {
        return write_header(iterator, Header{MajorType::SpecialFloat});
    }

    constexpr bool operator==(const Break &) const noexcept {
        return true;
    }

    constexpr std::strong_ordering operator<=>(const Break &) const noexcept {
        return std::strong_ordering::equal;
    }
};

using Variant = std::variant<
  Positive,
  Negative,
  ByteString,
  Utf8String,
  Array,
  Map,
  SemanticTag,
  Boolean,
  Null,
  Undefined,
  Float,
  Break>;

class Value {
  private:
    Variant value_;

  public:
    Value() noexcept : value_(Undefined{}) {
    }

    template <typename... Args>
    Value(Args &&...args) : value_(std::forward<Args>(args)...) {
    }

    Value(const std::uint64_t value) noexcept : value_(Positive{value}) {
    }

    Value(const std::uint8_t value) noexcept : Value(static_cast<uint64_t>(value)) {
    }

    Value(const std::uint16_t value) noexcept : Value(static_cast<uint64_t>(value)) {
    }

    Value(const std::uint32_t value) noexcept : Value(static_cast<uint64_t>(value)) {
    }

    Value(const std::int64_t value) noexcept {
        if (value >= 0) {
            value_ = Variant{Positive{static_cast<std::uint64_t>(value)}};
        } else if (value == std::numeric_limits<std::int64_t>::min()) {
            value_ = Variant{
              Negative{static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())}};
        } else {
            value_ = Variant{Negative{static_cast<std::uint64_t>(std::abs(value)) - 1}};
        }
    }

    Value(const std::int8_t value) noexcept : Value(static_cast<int64_t>(value)) {
    }

    Value(const std::int16_t value) noexcept : Value(static_cast<int64_t>(value)) {
    }

    Value(const std::int32_t value) noexcept : Value(static_cast<int64_t>(value)) {
    }

    Value(std::vector<std::byte> value) noexcept : value_(ByteString(std::move(value))) {
    }

    Value(const std::span<const std::byte> value) noexcept :
        value_(ByteString(std::vector<std::byte>(value.begin(), value.end()))) {
    }

    Value(std::u8string value) noexcept : value_(Utf8String(std::move(value))) {
    }

    Value(const std::u8string_view value) noexcept : value_(Utf8String(std::u8string(value))) {
    }

    Value(const char8_t *value) noexcept : value_(Utf8String(std::u8string(value))) {
    }

    Value(std::vector<ValuePointer> value) noexcept : value_(Array(std::move(value))) {
    }

    Value(std::map<ValuePointer, ValuePointer> value) noexcept : value_(Map(std::move(value))) {
    }

    Value(const std::uint64_t id, ValuePointer value) noexcept :
        value_(SemanticTag(id, std::move(value))) {
    }

    Value(const bool value) noexcept : value_(Boolean(value)) {
    }

    Value(const std::nullptr_t) noexcept : value_(Null{}) {
    }

    Value(const float value) noexcept : value_(Float(value)) {
    }

    Value(const double value) noexcept : value_(Float(value)) {
    }

    inline Variant &value() noexcept {
        return value_;
    }

    inline const Variant &value() const noexcept {
        return value_;
    }

    template <typename OutputIt>
    constexpr OutputIt encode(OutputIt output) const {
        return std::visit(
          [output](const auto &value) {
              return value.encode(output);
          },
          value_);
    }

    constexpr std::vector<std::byte> encode() const {
        std::vector<std::byte> output;
        std::visit(
          [&output](const auto &value) {
              value.encode(std::back_inserter(output));
          },
          value_);
        return output;
    }

    template <typename InputIt>
    static inline std::tuple<InputIt, Value> decode(InputIt begin, const InputIt end) {
        Header header;
        std::tie(begin, header) = read_header(begin, end);
        switch (header.type) {
        case MajorType::PositiveInteger: {
            return {begin, Value(Positive(header.get_count().value()))};
        }
        case MajorType::NegativeInteger: {
            return {begin, Value(Negative(header.get_count().value()))};
        }
        case MajorType::ByteString: {
            std::vector<std::byte> string;
            const auto count = header.get_count();
            if (count) {
                string.reserve(*count);
                if constexpr (std::contiguous_iterator<InputIt>) {
                    const auto string_end = begin + *count;
                    if (string_end > end) {
                        throw EndOfInput("String reads past end of buffer");
                    }
                    std::copy(begin, string_end, std::back_inserter(string));
                    begin = string_end;
                } else {
                    for (uint64_t i = 0; i < *count; ++i) {
                        string.emplace_back();
                        std::tie(begin, string.back()) = read(begin, end);
                    }
                }
            } else {
                Value value(Undefined{});
                std::tie(begin, value) = Value::decode(begin, end);
                for (; value != Value(Break{});
                     std::tie(begin, value) = Value::decode(begin, end)) {
                    auto substring = std::get<ByteString>(std::move(value).value_).value;
                    string.reserve(substring.size());
                    std::copy(substring.begin(), substring.end(), std::back_inserter(string));
                }
            }
            return {begin, Value(ByteString(std::move(string)))};
        }
        case MajorType::Utf8String: {
            std::u8string string;
            const auto count = header.get_count();
            if (count) {
                string.reserve(*count);
                if constexpr (std::contiguous_iterator<InputIt>) {
                    const auto string_end = begin + *count;
                    if (string_end > end) {
                        throw EndOfInput("String reads past end of buffer");
                    }
                    std::transform(
                      begin,
                      string_end,
                      std::back_inserter(string),
                      [](const auto byte) {
                          return static_cast<char8_t>(byte);
                      });
                    begin = string_end;
                } else {
                    for (uint64_t i = 0; i < *count; ++i) {
                        std::byte byte;
                        std::tie(begin, byte) = read(begin, end);
                        string.push_back(static_cast<char8_t>(byte));
                    }
                }
            } else {
                Value value(Undefined{});
                std::tie(begin, value) = Value::decode(begin, end);
                for (; value != Value(Break{});
                     std::tie(begin, value) = Value::decode(begin, end)) {
                    auto substring = std::get<Utf8String>(std::move(value).value_).value;
                    string.reserve(substring.size());
                    std::copy(substring.begin(), substring.end(), std::back_inserter(string));
                }
            }
            return {begin, Value(Utf8String(std::move(string)))};
        }
        case MajorType::Array: {
            std::vector<ValuePointer> array;
            const auto count = header.get_count();
            if (count) {
                array.reserve(*count);
                Value value(Undefined{});
                for (uint64_t i = 0; i < *count; ++i) {
                    std::tie(begin, value) = Value::decode(begin, end);
                    array.push_back(std::make_unique<Value>(std::move(value)));
                }
            } else {
                Value value(Undefined{});
                std::tie(begin, value) = Value::decode(begin, end);
                for (; value != Value(Break{});
                     std::tie(begin, value) = Value::decode(begin, end)) {
                    array.push_back(std::make_unique<Value>(std::move(value)));
                }
            }
            return {begin, Value(Array(std::move(array)))};
        }
        case MajorType::Map: {
            std::map<ValuePointer, ValuePointer> map;
            const auto count = header.get_count();
            if (count) {
                Value key(Undefined{});
                Value value(Undefined{});
                for (uint64_t i = 0; i < *count; ++i) {
                    std::tie(begin, key) = Value::decode(begin, end);
                    std::tie(begin, value) = Value::decode(begin, end);
                    map.insert(std::make_pair(
                      std::make_unique<Value>(std::move(key)),
                      std::make_unique<Value>(std::move(value))));
                }
            } else {
                Value key(Undefined{});
                Value value(Undefined{});
                std::tie(begin, key) = Value::decode(begin, end);
                for (; key != Value(Break{}); std::tie(begin, key) = Value::decode(begin, end)) {
                    std::tie(begin, value) = Value::decode(begin, end);
                    map.insert(std::make_pair(
                      std::make_unique<Value>(std::move(key)),
                      std::make_unique<Value>(std::move(value))));
                }
            }
            return {begin, Value(Map(std::move(map)))};
        }
        case MajorType::SemanticTag: {
            const auto count = header.get_count().value();
            Value value(Undefined{});
            std::tie(begin, value) = Value::decode(begin, end);
            return {begin, Value(SemanticTag(count, std::make_unique<Value>(std::move(value))))};
        }
        case MajorType::SpecialFloat: {
            switch (header.count.index()) {
            case 0: {
                switch (std::get<0>(header.count)) {
                case 20:
                    return {begin, Value(Boolean(false))};
                case 21:
                    return {begin, Value(Boolean(true))};
                case 22:
                    return {begin, Value(Null{})};
                case 23:
                    return {begin, Value(Undefined{})};
                case 31:
                    return {begin, Value(Break{})};
                default:
                    throw IllegalSpecialFloat(
                      "Illegal special float tiny header count " +
                      std::to_string(std::get<0>(header.count)));
                }
            }
            case 1:
                throw IllegalSpecialFloat(
                  "Illegal special float single-byte header value " +
                  std::to_string(std::get<1>(header.count)));
            case 2:
                return {begin, Value(Float(read_float16(to_be_bytes(std::get<2>(header.count)))))};
            case 3:
                return {
                  begin,
                  Value(Float(from_be_bytes<float>(to_be_bytes(std::get<3>(header.count)))))};
            case 4:
                return {
                  begin,
                  Value(Float(from_be_bytes<double>(to_be_bytes(std::get<4>(header.count)))))};
            default:
                __builtin_unreachable();
            }
            break;
        }
        default: {
            throw std::runtime_error("Illegal major type");
        }
        }
    }

    static inline Value decode(const std::span<const std::byte> bytes) {
        return std::get<1>(decode(std::begin(bytes), std::end(bytes)));
    }

    static inline Value decode(const std::vector<std::byte> &bytes) {
        return decode(std::span<const std::byte>{bytes.data(), bytes.size()});
    }

    inline bool operator==(const Value &other) const noexcept = default;
    constexpr auto operator<=>(const Value &other) const noexcept = default;
};

inline bool ValuePointer::operator==(const ValuePointer &other) const noexcept {
    return *value == *other.value;
}
inline std::strong_ordering ValuePointer::operator<=>(const ValuePointer &other) const noexcept {
    return *value <=> *other.value;
}

template <typename OutputIt>
constexpr OutputIt Array::encode(OutputIt output) const {
    output = write_header(output, Header(MajorType::Array, value.size()));
    for (const auto &item : value) {
        output = item->encode(output);
    }
    return output;
}

template <typename OutputIt>
inline OutputIt Map::encode(OutputIt output) const {
    output = write_header(output, Header(MajorType::Map, value.size()));
    for (const auto &[key, val] : value) {
        output = key->encode(output);
        output = val->encode(output);
    }
    return output;
}
template <typename OutputIt>
constexpr OutputIt SemanticTag::encode(OutputIt output) const {
    output = write_header(output, Header(MajorType::SemanticTag, id));
    return value->encode(output);
}
} // namespace varbor
