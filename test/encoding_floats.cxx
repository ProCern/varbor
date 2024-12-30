/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      varbor::Value(0.15625f).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b00110001),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit float");
    if (
      varbor::Value(0.15625).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b00110001),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit float from double");
    if (
      varbor::Value(1.0f / 3.0f).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(26),
        std::byte(0b00111110),
        std::byte(0b10101010),
        std::byte(0b10101010),
        std::byte(0b10101011)}))
      throw std::runtime_error("32 bit float");
    if (
      varbor::Value(static_cast<double>(1.0f / 3.0f)).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(26),
        std::byte(0b00111110),
        std::byte(0b10101010),
        std::byte(0b10101010),
        std::byte(0b10101011)}))
      throw std::runtime_error("32 bit float from double");
    if (
      varbor::Value(1.0 / 3.0).encode() !=
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
      throw std::runtime_error("64 bit float");

    if (
      varbor::Value(0.0).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b00000000),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit zero");

    if (
      varbor::Value(-0.0).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b10000000),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit negative zero");

    if (
      varbor::Value(std::numeric_limits<double>::infinity()).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b01111100),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit inifinity");

    if (
      varbor::Value(-std::numeric_limits<double>::infinity()).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b11111100),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit negative inifinity");

    if (
      varbor::Value(std::numeric_limits<double>::quiet_NaN()).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b01111110),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit quiet nan");

    if (
      varbor::Value(std::numeric_limits<double>::signaling_NaN()).encode() !=
      (std::vector<std::byte>{
        std::byte(7 << 5) | std::byte(25),
        std::byte(0b01111110),
        std::byte(0b00000000)}))
      throw std::runtime_error("16 bit signaling nan");
  return 0;
}

