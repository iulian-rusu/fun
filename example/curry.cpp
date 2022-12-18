#include <iostream>
#include <fun.hpp>

int sum(int x, int y, int z) { return x + y + z; }

int main()
{
    auto curried = fun::curry(sum);
    auto uncurried = fun::uncurry(curried);
    std::cout << sum(1, 2, 3) << '\n';
    std::cout << curried(1)(2)(3) << '\n';
    std::cout << uncurried(1, 2, 3) << '\n';
}


