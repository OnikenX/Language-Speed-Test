#include <iostream>
#include <vector>

std::vector<int> primenumbers(int max = 1000) {
  std::vector<int> primes;
  primes.push_back(1);
  primes.push_back(2);
  int div, actual = 3;
  bool notprime;
  while (actual < max) {
    notprime = false;
    for (div = 2; div < actual; ++div)
      if (!(actual % div)) {
        notprime = true;
        break;
      }
    if (!notprime)
      primes.push_back(actual);
    ++actual;
  }

  return primes;
}

int main() {
  // auto primes = primenumbers(5);
  // for (int d = 0; d < primes.size(); ++d)
  // std::cout << primes[d] << std::endl;
  try {
    std::cout << 2 / 0 << std::endl;
  } catch (...) {
    std::cout << "Ocurreu um erro." << std::endl;
  }
}
