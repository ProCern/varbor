/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    varbor::Map map;
    map.value.insert(std::make_pair(
      std::make_unique<varbor::Value>(u8"1337"),
      std::make_unique<varbor::Value>(u8"6969")));

    if (
      varbor::Value(std::move(map)).encode() !=

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
      }))
        throw std::runtime_error("Fail");

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

    if (
      varbor::Value(std::move(top)).encode() !=

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
      }))
        throw std::runtime_error("Fail");
    return 0;
}

