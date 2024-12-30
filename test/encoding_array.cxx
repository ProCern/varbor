/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    varbor::Array array;
    array.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    array.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    if (
      varbor::Value(std::move(array)).encode() !=

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
      }))
        throw std::runtime_error("Fail");

    varbor::Array top;
    varbor::Array first;
    varbor::Array second;
    first.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    second.value.push_back(std::make_unique<varbor::Value>(u8"6969"));
    top.value.push_back(std::make_unique<varbor::Value>(std::move(first)));
    top.value.push_back(std::make_unique<varbor::Value>(std::move(second)));
    if (
      varbor::Value(std::move(top)).encode() !=

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
      }))
        throw std::runtime_error("Fail");
    return 0;
}

