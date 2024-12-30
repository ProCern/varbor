/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
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
    if (
      varbor::Value(std::move(tag)) !=

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
      }))
        throw std::runtime_error("Fail");
    return 0;
}

