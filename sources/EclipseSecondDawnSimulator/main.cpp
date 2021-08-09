#include <iostream>

#include <esd_core/samplecore.hpp>

int main() {
  std::cout << esd::sample{}.getValue() << "\n";
  std::cout << "Hello, World!\n";
  std::cout << std::endl;
  return 0;
}
