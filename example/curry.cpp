#include <iostream>
#include <fun.hpp>

int main()
{
    auto original = [](int x, int y, int z) { return x + y + z; };
    auto curried = fun::curry(original);
    auto uncurried = fun::uncurry(curried);
    std::cout << original(1, 2, 3) << '\n';
    std::cout << curried(1)(2)(3) << '\n';
    std::cout << uncurried(1, 2, 3) << '\n';
}


