/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      5 !=
      std::get<varbor::Positive>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(5)}).value())
        .value)
        throw std::runtime_error("positive int");
    if (
      24 !=
      std::get<varbor::Positive>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(24), std::byte(24)}).value())
        .value)
        throw std::runtime_error("1 byte positive int");
    if (
      256 !=
      std::get<varbor::Positive>(
        varbor::Value::decode(std::vector<std::byte>{std::byte(25), std::byte(1), std::byte(0)})
          .value())
        .value)
        throw std::runtime_error("2 byte positive int");
    if (
      65536 !=
      std::get<varbor::Positive>(varbor::Value::decode(std::vector<std::byte>{
                                                         std::byte(26),
                                                         std::byte(0),
                                                         std::byte(1),
                                                         std::byte(0),
                                                         std::byte(0)})
                                   .value())
        .value)
        throw std::runtime_error("4 byte positive int");
    if (
      4294967296 !=
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
        throw std::runtime_error("8 byte positive int");
    return 0;
}
