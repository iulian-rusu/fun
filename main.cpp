#include <iostream>
#include <fun.hpp>

struct Foo {};

int main()
{
    auto type_name = fun::overload(
            [](int) { return "int"; },
            [](double) { return "double"; },
            [](const char *) { return "const char *"; },
            [](Foo) { return "Foo"; },
            [](auto &&) { return "unknown type"; } // default case
    );
    std::cout << type_name(0) << '\n';
    std::cout << type_name(3.14) << '\n';
    std::cout << type_name("abc") << '\n';
    std::cout << type_name(Foo{}) << '\n';
    std::cout << type_name(std::string_view{}) << '\n';

    auto original = [](int x, int y, int z) { return x + y + z; };
    auto curried = fun::curry(original);
    auto uncurried = fun::uncurry(curried);
    std::cout << original(1, 2, 3) << '\n';
    std::cout << curried(1)(2)(3) << '\n';
    std::cout << uncurried(1, 2, 3) << '\n';
}

