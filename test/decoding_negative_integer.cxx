/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      -6 !=
      static_cast<int64_t>(std::get<varbor::Negative>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(1 << 5) | std::byte(5)}).value())))
        throw std::runtime_error("tiny negative int");
    if (
      -25 !=
      static_cast<int64_t>(std::get<varbor::Negative>(
        varbor::Value::decode(
          std::vector<std::byte>{std::byte(1 << 5) | std::byte(24), std::byte(24)})
          .value())))
        throw std::runtime_error("1 byte negative int");
    if (
      -257 !=
      static_cast<int64_t>(std::get<varbor::Negative>(
        varbor::Value::decode(
          std::vector<std::byte>{std::byte(1 << 5) | std::byte(25), std::byte(1), std::byte(0)})
          .value())))
        throw std::runtime_error("2 byte negative int");
    if (
      -65537 !=
      static_cast<int64_t>(
        std::get<varbor::Negative>(varbor::Value::decode(std::vector<std::byte>{
                                                           std::byte(1 << 5) | std::byte(26),
                                                           std::byte(0),
                                                           std::byte(1),
                                                           std::byte(0),
                                                           std::byte(0)})
                                     .value())))
        throw std::runtime_error("4 byte negative int");
    if (
      -4294967297 !=
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
        throw std::runtime_error("8 byte negative int");
    return 0;
}
