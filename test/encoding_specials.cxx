/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      varbor::Value(varbor::Boolean(false)).encode() !=
      std::vector<std::byte>{std::byte(7 << 5) | std::byte(20)}) {
        throw std::runtime_error("boolean false");
    }
    if (
      varbor::Value(varbor::Boolean(true)).encode() !=
      std::vector<std::byte>{std::byte(7 << 5) | std::byte(21)}) {
        throw std::runtime_error("boolean true");
    }
    if (
      varbor::Value(varbor::Null{}).encode() !=
      std::vector<std::byte>{std::byte(7 << 5) | std::byte(22)}) {
        throw std::runtime_error("null");
    }
    if (
      varbor::Value(nullptr).encode() !=
      std::vector<std::byte>{std::byte(7 << 5) | std::byte(22)}) {
        throw std::runtime_error("implicit null");
    }
    if (
      varbor::Value(varbor::Undefined{}).encode() !=
      std::vector<std::byte>{std::byte(7 << 5) | std::byte(23)}) {
        throw std::runtime_error("undefined");
    }
    if (varbor::Value{}.encode() != std::vector<std::byte>{std::byte(7 << 5) | std::byte(23)}) {
        throw std::runtime_error("implicit undefined");
    }
    if (
      varbor::Value(varbor::Break{}).encode() !=
      std::vector<std::byte>{std::byte(7 << 5) | std::byte(31)}) {
        throw std::runtime_error("break");
    }
    return 0;
}
