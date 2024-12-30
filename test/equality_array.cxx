/* Copyright © 2022 Taylor C. Richberger
 * This code is released under the license described in the LICENSE file
 */

#include <varbor.hxx>

int main() {
    varbor::Array first;
    first.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    first.value.push_back(std::make_unique<varbor::Value>(u8"6969"));

    varbor::Array second;
    second.value.push_back(std::make_unique<varbor::Value>(u8"1337"));
    second.value.push_back(std::make_unique<varbor::Value>(u8"6969"));

    if (varbor::Value(std::move(first)) != varbor::Value(std::move(second))) {
      throw std::runtime_error("Arrays not equal");
    }
    return 0;
}
