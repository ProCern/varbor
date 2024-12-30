/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (varbor::Value(5).encode() != std::vector<std::byte>{std::byte(5)})
      throw std::runtime_error("tiny positive int");
    if (varbor::Value(24).encode() != (std::vector<std::byte>{std::byte(24), std::byte(24)}))
      throw std::runtime_error("1 byte positive int");
    if (
      varbor::Value(256).encode() !=
      (std::vector<std::byte>{std::byte(25), std::byte(1), std::byte(0)}))
      throw std::runtime_error("2 byte positive int");
    if (
      varbor::Value(65536).encode() !=
      (std::vector<
        std::byte>{std::byte(26), std::byte(0), std::byte(1), std::byte(0), std::byte(0)}))
      throw std::runtime_error("4 byte positive int");
    if (
      varbor::Value(4294967296).encode() !=
      (std::vector<std::byte>{
        std::byte(27),
        std::byte(0),
        std::byte(0),
        std::byte(0),
        std::byte(1),
        std::byte(0),
        std::byte(0),
        std::byte(0),
        std::byte(0)}))
      throw std::runtime_error("8 byte positive int");
  return 0;
}


