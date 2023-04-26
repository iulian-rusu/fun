#include <iostream>
#include <string_view>
#include <fun.hpp>
#include <variant>

int main()
{
    auto type_name = []<typename Variant>(Variant &&var) -> std::string_view {
        return fun::match(
            var,
            [](int) { return "int"; },
            [](double) { return "double"; },
            [](auto &&) { return "unknown"; }
        );
    };

    using variant = std::variant<int, double, std::string_view>;
    variant var = 1;
    std::cout << type_name(var) << '\n';
    var = 3.14;
    std::cout << type_name(var) << '\n';
    var = "hello";
    std::cout << type_name(var) << '\n';
}