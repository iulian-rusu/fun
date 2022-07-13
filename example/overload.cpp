#include <iostream>
#include <fun.hpp>

int main()
{
    auto type_name = fun::overload(
            [](int) { return "int"; },
            [](double) { return "double"; },
            [](auto) { return "unknown"; }
    );
    std::cout << type_name(3435) << '\n';
    std::cout << type_name(3.14) << '\n';
    std::cout << type_name("ab") << '\n';
}