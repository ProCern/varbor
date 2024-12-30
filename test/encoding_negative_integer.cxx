/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (varbor::Value(-6).encode() != std::vector<std::byte>{std::byte(1 << 5) | std::byte(5)})
      throw std::runtime_error("tiny negative int");
    if (
      varbor::Value(-25).encode() !=
      (std::vector<std::byte>{std::byte(1 << 5) | std::byte(24), std::byte(24)}))
      throw std::runtime_error("1 byte negative int");
    if (
      varbor::Value(-257).encode() !=
      (std::vector<std::byte>{std::byte(1 << 5) | std::byte(25), std::byte(1), std::byte(0)}))
      throw std::runtime_error("2 byte negative int");
    if (
      varbor::Value(-65537).encode() !=
      (std::vector<std::byte>{
        std::byte(1 << 5) | std::byte(26),
        std::byte(0),
        std::byte(1),
        std::byte(0),
        std::byte(0)}))
      throw std::runtime_error("4 byte negative int");
    if (
      varbor::Value(-4294967297).encode() !=
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
      throw std::runtime_error("8 byte negative int");
  return 0;
}


