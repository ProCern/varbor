/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      false !=
      std::get<varbor::Boolean>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(7 << 5) | std::byte(20)}).value())
        .value)
        throw std::runtime_error("boolean false");
    if (
      true !=
      std::get<varbor::Boolean>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(7 << 5) | std::byte(21)}).value())
        .value)
        throw std::runtime_error("boolean true");
    if (
      varbor::Null{} !=
      std::get<varbor::Null>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(7 << 5) | std::byte(22)}).value()))
        throw std::runtime_error("null");
    return 0;
}
