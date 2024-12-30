/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      0.15625 !=
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b00110001),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
        throw std::runtime_error("16 bit float to double");
    if (
      static_cast<double>(1.0f / 3.0f) !=
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(26),
                                                      std::byte(0b00111110),
                                                      std::byte(0b10101010),
                                                      std::byte(0b10101010),
                                                      std::byte(0b10101011)})
                                .value())
        .value)
        throw std::runtime_error("32 bit float");
    if (
      1.0 / 3.0 !=
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
        throw std::runtime_error("64 bit float");
    if (
      0.0 !=
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b00000000),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
        throw std::runtime_error("16 bit zero");
    if (
      -0.0 !=
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b10000000),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
        throw std::runtime_error("16 bit negative zero");
    if (
      std::numeric_limits<double>::infinity() !=
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b01111100),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
        throw std::runtime_error("16 bit inifinity");
    if (
      -std::numeric_limits<double>::infinity() !=
      std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                      std::byte(7 << 5) | std::byte(25),
                                                      std::byte(0b11111100),
                                                      std::byte(0b00000000)})
                                .value())
        .value)
        throw std::runtime_error("16 bit negative inifinity");
    if (!std::isnan(
          std::get<varbor::Float>(varbor::Value::decode(std::vector<std::byte>{
                                                          std::byte(7 << 5) | std::byte(25),
                                                          std::byte(0b01111110),
                                                          std::byte(0b00000000)})
                                    .value())))

        throw std::runtime_error("16 bit nan");
    return 0;
}
