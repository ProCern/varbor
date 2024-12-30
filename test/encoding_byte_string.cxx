/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    if (
      (varbor::Value(std::vector<std::byte>{std::byte(1), std::byte(3), std::byte(3), std::byte(7)})
         .encode()) !=
      (std::vector<std::byte>{
        std::byte(2 << 5) | std::byte(4),
        std::byte(1),
        std::byte(3),
        std::byte(3),
        std::byte(7)}))
        throw std::runtime_error("Fail");
    return 0;
}



