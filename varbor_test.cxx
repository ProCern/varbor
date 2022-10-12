/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include "varbor.hxx"
#include <gtest/gtest.h>
#include <limits>

TEST(Equality, Array) {
    varbor::Array first;
    first.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    first.value.push_back(std::make_unique<varbor::Value>(u8"6969"));

    varbor::Array second;
    second.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    second.value.push_back(std::make_unique<varbor::Value>(u8"6969"));

    EXPECT_EQ(varbor::Value(std::move(first)), varbor::Value(std::move(second)));
}

TEST(Encoding, Specials) {
    EXPECT_EQ(
      varbor::Value(varbor::Boolean(false)).encode(),
      (std::vector<std::byte>{std::byte(7 << 5) | std::byte(20)}))
      << "boolean false";
    EXPECT_EQ(
      varbor::Value(varbor::Boolean(true)).encode(),
      (std::vector<std::byte>{std::byte(7 << 5) | std::byte(21)}))
      << "boolean true";
    EXPECT_EQ(
      varbor::Value(varbor::Null{}).encode(),
      (std::vector<std::byte>{std::byte(7 << 5) | std::byte(22)}))
      << "null";
    EXPECT_EQ(
      varbor::Value(nullptr).encode(),
      (std::vector<std::byte>{std::byte(7 << 5) | std::byte(22)}))
      << "implicit null";
    EXPECT_EQ(
      varbor::Value(varbor::Undefined{}).encode(),
      (std::vector<std::byte>{std::byte(7 << 5) | std::byte(23)}))
      << "undefined";
    EXPECT_EQ(varbor::Value{}.encode(), (std::vector<std::byte>{std::byte(7 << 5) | std::byte(23)}))
      << "implicit undefined";
    EXPECT_EQ(
      varbor::Value(varbor::Break{}).encode(),
      (std::vector<std::byte>{std::byte(7 << 5) | std::byte(31)}))
      << "break";
}

TEST(Encoding, Floats) {
    EXPECT_EQ(
      varbor::Value(0.15625f).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b00110001),
        std::byte(0b00000000)}))
      << "16 bit float";
    EXPECT_EQ(
      varbor::Value(0.15625).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b00110001),
        std::byte(0b00000000)}))
      << "16 bit float from double";
    EXPECT_EQ(
      varbor::Value(1.0f / 3.0f).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(26),
        std::byte(0b00111110),
        std::byte(0b10101010),
        std::byte(0b10101010),
        std::byte(0b10101011)}))
      << "32 bit float";
    EXPECT_EQ(
      varbor::Value(static_cast<double>(1.0f / 3.0f)).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(26),
        std::byte(0b00111110),
        std::byte(0b10101010),
        std::byte(0b10101010),
        std::byte(0b10101011)}))
      << "32 bit float from double";
    EXPECT_EQ(
      varbor::Value(1.0 / 3.0).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(27),
        std::byte(0b00111111),
        std::byte(0b11010101),
        std::byte(0b01010101),
        std::byte(0b01010101),
        std::byte(0b01010101),
        std::byte(0b01010101),
        std::byte(0b01010101),
        std::byte(0b01010101)}))
      << "64 bit float";

    EXPECT_EQ(
      varbor::Value(0.0).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b00000000),
        std::byte(0b00000000)}))
      << "16 bit zero";

    EXPECT_EQ(
      varbor::Value(-0.0).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b10000000),
        std::byte(0b00000000)}))
      << "16 bit negative zero";

    EXPECT_EQ(
      varbor::Value(std::numeric_limits<double>::infinity()).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b01111100),
        std::byte(0b00000000)}))
      << "16 bit inifinity";

    EXPECT_EQ(
      varbor::Value(-std::numeric_limits<double>::infinity()).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b11111100),
        std::byte(0b00000000)}))
      << "16 bit negative inifinity";

    EXPECT_EQ(
      varbor::Value(std::numeric_limits<double>::quiet_NaN()).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b01111110),
        std::byte(0b00000000)}))
      << "16 bit quiet nan";

    EXPECT_EQ(
      varbor::Value(std::numeric_limits<double>::signaling_NaN()).encode(),
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b01111110),
        std::byte(0b00000000)}))
      << "16 bit signaling nan";
}

TEST(Encoding, PositiveInteger) {
    EXPECT_EQ(varbor::Value(5).encode(), std::vector<std::byte>{std::byte(5)})
      << "tiny positive int";
    EXPECT_EQ(varbor::Value(24).encode(), (std::vector<std::byte>{std::byte(24), std::byte(24)}))
      << "1 byte positive int";
    EXPECT_EQ(
      varbor::Value(256).encode(),
      (std::vector<std::byte>{std::byte(25), std::byte(1), std::byte(0)}))
      << "2 byte positive int";
    EXPECT_EQ(
      varbor::Value(65536).encode(),
      (std::vector<
        std::byte>{std::byte(26), std::byte(0), std::byte(1), std::byte(0), std::byte(0)}))
      << "4 byte positive int";
    EXPECT_EQ(
      varbor::Value(4294967296).encode(),
      (std::vector<std::byte>{
        std::byte(27),
        std::byte(0),
        std::byte(0),
        std::byte(0),
        std::byte(1),
        std::byte(0),
        std::byte(0),
        std::byte(0),
        std::byte(0)}))
      << "8 byte positive int";
}

TEST(Encoding, NegativeInteger) {
    EXPECT_EQ(varbor::Value(-6).encode(), std::vector<std::byte>{std::byte(1 << 5) | std::byte(5)})
      << "tiny negative int";
    EXPECT_EQ(
      varbor::Value(-25).encode(),
      (std::vector<std::byte>{std::byte(1 << 5) | std::byte(24), std::byte(24)}))
      << "1 byte negative int";
    EXPECT_EQ(
      varbor::Value(-257).encode(),
      (std::vector<std::byte>{std::byte(1 << 5) | std::byte(25), std::byte(1), std::byte(0)}))
      << "2 byte negative int";
    EXPECT_EQ(
      varbor::Value(-65537).encode(),
      (std::vector<std::byte>{
        std::byte(1 << 5) | std::byte(26),
        std::byte(0),
        std::byte(1),
        std::byte(0),
        std::byte(0)}))
      << "4 byte negative int";
    EXPECT_EQ(
      varbor::Value(-4294967297).encode(),
      (std::vector<std::byte>{
        std::byte(1 << 5) | std::byte(27),
        std::byte(0),
        std::byte(0),
        std::byte(0),
        std::byte(1),
        std::byte(0),
        std::byte(0),
        std::byte(0),
        std::byte(0)}))
      << "8 byte negative int";
}

TEST(Encoding, ByteString) {
    EXPECT_EQ(
      (varbor::Value(std::vector<std::byte>{std::byte(1), std::byte(3), std::byte(3), std::byte(7)})
         .encode()),
      (std::vector<std::byte>{
        std::byte(2 << 5) | std::byte(4),
        std::byte(1),
        std::byte(3),
        std::byte(3),
        std::byte(7)}));
}

TEST(Encoding, String) {
    EXPECT_EQ(
      (varbor::Value(std::u8string_view(u8"1337")).encode()),
      (std::vector<std::byte>{
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7')}));
    EXPECT_EQ(
      (varbor::Value(u8"1337").encode()),
      (std::vector<std::byte>{
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7')}));
}

TEST(Decoding, ByteString) {
    EXPECT_EQ(
      (std::vector<std::byte>{std::byte(1), std::byte(3), std::byte(3), std::byte(7)}),
      std::get<varbor::ByteString>(varbor::Value::decode(std::vector<std::byte>{
                                                           std::byte(2 << 5) | std::byte(4),
                                                           std::byte(1),
                                                           std::byte(3),
                                                           std::byte(3),
                                                           std::byte(7)})
                                     .value()));
}

TEST(Decoding, String) {
    EXPECT_EQ(
      std::u8string(u8"1337"),
      std::get<varbor::Utf8String>(varbor::Value::decode(std::vector<std::byte>{
                                                           std::byte(3 << 5) | std::byte(4),
                                                           std::byte('1'),
                                                           std::byte('3'),
                                                           std::byte('3'),
                                                           std::byte('7')})
                                     .value()));
}

TEST(Encoding, Array) {
    varbor::Array array;
    array.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    array.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    EXPECT_EQ(
      varbor::Value(std::move(array)).encode(),

      (std::vector<std::byte>{
        // Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
      }));

    varbor::Array top;
    varbor::Array first;
    varbor::Array second;
    first.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    second.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    top.value.push_back(std::make_unique<varbor::Value>(std::move(first)));
    top.value.push_back(std::make_unique<varbor::Value>(std::move(second)));
    EXPECT_EQ(
      varbor::Value(std::move(top)).encode(),

      (std::vector<std::byte>{
        // Header
        std::byte(4 << 5) | std::byte(2),
        // Header
        std::byte(4 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // Header
        std::byte(4 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
      }));
}

TEST(Encoding, Map) {
    varbor::Map map;
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"1337"),
      std::make_unique<varbor::Value>(u8"6969")));

    EXPECT_EQ(
      varbor::Value(std::move(map)).encode(),

      (std::vector<std::byte>{
        // Header
        std::byte(5 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
      }));

    varbor::Map top;
    varbor::Map first;
    varbor::Map second;
    first.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"1337"),
      std::make_unique<varbor::Value>(u8"6969")));
    second.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"foo"),
      std::make_unique<varbor::Value>(u8"bar")));
    top.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(std::move(first)),
      std::make_unique<varbor::Value>(std::move(second))));

    EXPECT_EQ(
      varbor::Value(std::move(top)).encode(),

      (std::vector<std::byte>{
        // Header
        std::byte(5 << 5) | std::byte(1),
        // Header
        std::byte(5 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
        // Header
        std::byte(5 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('f'),
        std::byte('o'),
        std::byte('o'),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('b'),
        std::byte('a'),
        std::byte('r'),
      }));
}

TEST(Encoding, MapArrayMixedRecursive) {
    varbor::Array array;
    varbor::Map map;
    varbor::Array key;
    varbor::Array value;
    key.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    key.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"foo"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"bar"));
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(std::move(key)),
      std::make_unique<varbor::Value>(std::move(value))));
    array.value.push_back(std::make_unique<varbor::Value>(std::move(map)));
    EXPECT_EQ(
      varbor::Value(std::move(array)).encode(),

      (std::vector<std::byte>{
        // Array Header
        std::byte(4 << 5) | std::byte(1),
        // Map Header
        std::byte(5 << 5) | std::byte(1),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('f'),
        std::byte('o'),
        std::byte('o'),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('b'),
        std::byte('a'),
        std::byte('r'),
      }));
}

TEST(Encoding, SemanticTag) {
    varbor::Array array;
    varbor::Map map;
    varbor::Array key;
    varbor::Array value;
    key.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    key.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"foo"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"bar"));
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(std::move(key)),
      std::make_unique<varbor::Value>(std::move(value))));
    array.value.push_back(std::make_unique<varbor::Value>(std::move(map)));
    varbor::SemanticTag tag(55799, std::make_unique<varbor::Value>(std::move(array)));
    EXPECT_EQ(
      varbor::Value(std::move(tag)).encode(),

      (std::vector<std::byte>{
        // Simple CBOR data follows prefix
        std::byte(0xd9),
        std::byte(0xd9),
        std::byte(0xf7),

        // Array Header
        std::byte(4 << 5) | std::byte(1),
        // Map Header
        std::byte(5 << 5) | std::byte(1),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('f'),
        std::byte('o'),
        std::byte('o'),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('b'),
        std::byte('a'),
        std::byte('r'),
      }));
}

TEST(Decoding, SemanticTag) {
    varbor::Array array;
    varbor::Map map;
    varbor::Array key;
    varbor::Array value;
    key.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    key.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"foo"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"bar"));
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(std::move(key)),
      std::make_unique<varbor::Value>(std::move(value))));
    array.value.push_back(std::make_unique<varbor::Value>(std::move(map)));
    varbor::SemanticTag tag(55799, std::make_unique<varbor::Value>(std::move(array)));
    EXPECT_EQ(
      varbor::Value(std::move(tag)),

      varbor::Value::decode(std::vector<std::byte>{
        // Simple CBOR data follows prefix
        std::byte(0xd9),
        std::byte(0xd9),
        std::byte(0xf7),

        // Array Header
        std::byte(4 << 5) | std::byte(1),
        // Map Header
        std::byte(5 << 5) | std::byte(1),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('f'),
        std::byte('o'),
        std::byte('o'),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('b'),
        std::byte('a'),
        std::byte('r'),
      }));
}

TEST(Decoding, Specials) {
    EXPECT_EQ(
      false,
      std::get<varbor::Boolean>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(7 << 5) | std::byte(20)}).value())
        .value)
      << "boolean false";
    EXPECT_EQ(
      true,
      std::get<varbor::Boolean>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(7 << 5) | std::byte(21)}).value())
        .value)
      << "boolean true";
    EXPECT_EQ(
      varbor::Null{},
      std::get<varbor::Null>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(7 << 5) | std::byte(22)}).value()))
      << "null";
}

TEST(Decoding, Floats) {
    EXPECT_EQ(
      0.15625,
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b00110001),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
      << "16 bit float to double";
    EXPECT_EQ(
      static_cast<double>(1.0f / 3.0f),
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(26),
                                                      std::byte(0b00111110),
                                                      std::byte(0b10101010),
                                                      std::byte(0b10101010),
                                                      std::byte(0b10101011)})
                                .value())
        .value)
      << "32 bit float";
    EXPECT_EQ(
      1.0 / 3.0,
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(27),
                                                      std::byte(0b00111111),
                                                      std::byte(0b11010101),
                                                      std::byte(0b01010101),
                                                      std::byte(0b01010101),
                                                      std::byte(0b01010101),
                                                      std::byte(0b01010101),
                                                      std::byte(0b01010101),
                                                      std::byte(0b01010101)})
                                .value())
        .value)
      << "64 bit float";
    EXPECT_EQ(
      0.0,
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b00000000),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
      << "16 bit zero";
    EXPECT_EQ(
      -0.0,
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b10000000),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
      << "16 bit negative zero";
    EXPECT_EQ(
      std::numeric_limits<double>::infinity(),
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b01111100),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
      << "16 bit inifinity";
    EXPECT_EQ(
      -std::numeric_limits<double>::infinity(),
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b11111100),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
      << "16 bit negative inifinity";
    EXPECT_TRUE(
      std::isnan(std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                                 std::byte(7 << 5) | std::byte(25),
                                                                 std::byte(0b01111110),
                                                                 std::byte(0b00000000)})
                                           .value())
                   .value))
      << "16 bit nan";
}

TEST(Decoding, PositiveInteger) {
    EXPECT_EQ(
      5,
      std::get<varbor::Positive>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(5)}).value())
        .value)
      << "positive int";
    EXPECT_EQ(
      24,
      std::get<varbor::Positive>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(24), std::byte(24)}).value())
        .value)
      << "1 byte positive int";
    EXPECT_EQ(
      256,
      std::get<varbor::Positive>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(25), std::byte(1), std::byte(0)})
          .value())
        .value)
      << "2 byte positive int";
    EXPECT_EQ(
      65536,
      std::get<varbor::Positive>(varbor::Value::decode(std::vector<std::byte>{
                                                         std::byte(26),
                                                         std::byte(0),
                                                         std::byte(1),
                                                         std::byte(0),
                                                         std::byte(0)})
                                   .value())
        .value)
      << "4 byte positive int";
    EXPECT_EQ(
      4294967296,
      std::get<varbor::Positive>(varbor::Value::decode(std::vector<std::byte>{
                                                         std::byte(27),
                                                         std::byte(0),
                                                         std::byte(0),
                                                         std::byte(0),
                                                         std::byte(1),
                                                         std::byte(0),
                                                         std::byte(0),
                                                         std::byte(0),
                                                         std::byte(0)})
                                   .value())
        .value)
      << "8 byte positive int";
}

TEST(Decoding, NegativeInteger) {
    EXPECT_EQ(
      -6,
      static_cast<int64_t>(std::get<varbor::Negative>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(1 << 5) | std::byte(5)}).value())))
      << "tiny negative int";
    EXPECT_EQ(
      -25,
      static_cast<int64_t>(std::get<varbor::Negative>(
        varbor::Value::decode(
          std::vector<std::byte>{std::byte(1 << 5) | std::byte(24), std::byte(24)})
          .value())))
      << "1 byte negative int";
    EXPECT_EQ(
      -257,
      static_cast<int64_t>(std::get<varbor::Negative>(
        varbor::Value::decode(
          std::vector<std::byte>{std::byte(1 << 5) | std::byte(25), std::byte(1), std::byte(0)})
          .value())))
      << "2 byte negative int";
    EXPECT_EQ(
      -65537,
      static_cast<int64_t>(
        std::get<varbor::Negative>(varbor::Value::decode(std::vector<std::byte>{
                                                           std::byte(1 << 5) | std::byte(26),
                                                           std::byte(0),
                                                           std::byte(1),
                                                           std::byte(0),
                                                           std::byte(0)})
                                     .value())))
      << "4 byte negative int";
    EXPECT_EQ(
      -4294967297,
      static_cast<int64_t>(
        std::get<varbor::Negative>(varbor::Value::decode(std::vector<std::byte>{
                                                           std::byte(1 << 5) | std::byte(27),
                                                           std::byte(0),
                                                           std::byte(0),
                                                           std::byte(0),
                                                           std::byte(1),
                                                           std::byte(0),
                                                           std::byte(0),
                                                           std::byte(0),
                                                           std::byte(0)})
                                     .value())))
      << "8 byte negative int";
}

TEST(Decoding, Array) {
    varbor::Array array;
    array.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    array.value.push_back(std::make_unique<varbor::Value>(u8"6969"));

    EXPECT_EQ(
      varbor::Value(std::move(array)),

      varbor::Value::decode(std::vector<std::byte>{
        // Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
      }));

    varbor::Array top;
    varbor::Array first;
    varbor::Array second;
    first.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    second.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    top.value.push_back(std::make_unique<varbor::Value>(std::move(first)));
    top.value.push_back(std::make_unique<varbor::Value>(std::move(second)));

    EXPECT_EQ(
      varbor::Value(std::move(top)),

      varbor::Value::decode(std::vector<std::byte>{
        // Header
        std::byte(4 << 5) | std::byte(2),
        // Header
        std::byte(4 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // Header
        std::byte(4 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
      }));
}

TEST(Decoding, Map) {
    varbor::Map map;
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"1337"),
      std::make_unique<varbor::Value>(u8"6969")));

    EXPECT_EQ(
      varbor::Value(std::move(map)),

      (varbor::Value::decode(std::vector<std::byte>{
        // Header
        std::byte(5 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
      })));

    varbor::Map top;
    varbor::Map first;
    varbor::Map second;
    first.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"1337"),
      std::make_unique<varbor::Value>(u8"6969")));
    second.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"foo"),
      std::make_unique<varbor::Value>(u8"bar")));
    top.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(std::move(first)),
      std::make_unique<varbor::Value>(std::move(second))));

    EXPECT_EQ(
      varbor::Value(std::move(top)),

      varbor::Value::decode(std::vector<std::byte>{
        // Header
        std::byte(5 << 5) | std::byte(1),
        // Header
        std::byte(5 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
        // Header
        std::byte(5 << 5) | std::byte(1),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('f'),
        std::byte('o'),
        std::byte('o'),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('b'),
        std::byte('a'),
        std::byte('r'),
      }));
}

TEST(Decoding, MapArrayMixedRecursive) {
    varbor::Array array;
    varbor::Map map;
    varbor::Array key;
    varbor::Array value;
    key.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    key.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"foo"));
    value.value.push_back(std::make_unique<varbor::Value>(u8"bar"));
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(std::move(key)),
      std::make_unique<varbor::Value>(std::move(value))));
    array.value.push_back(std::make_unique<varbor::Value>(std::move(map)));
    EXPECT_EQ(
      varbor::Value(std::move(array)),

      varbor::Value::decode(std::vector<std::byte>{
        // Array Header
        std::byte(4 << 5) | std::byte(1),
        // Map Header
        std::byte(5 << 5) | std::byte(1),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('1'),
        std::byte('3'),
        std::byte('3'),
        std::byte('7'),
        // String
        std::byte(3 << 5) | std::byte(4),
        std::byte('6'),
        std::byte('9'),
        std::byte('6'),
        std::byte('9'),
        // Array Header
        std::byte(4 << 5) | std::byte(2),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('f'),
        std::byte('o'),
        std::byte('o'),
        // String
        std::byte(3 << 5) | std::byte(3),
        std::byte('b'),
        std::byte('a'),
        std::byte('r'),
      }));
}
