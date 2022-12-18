#include <iostream>
#include <fun.hpp>
#include <variant>


int main()
{
    auto type_name_visitor = fun::overload(
        [](int) { return "int"; },
        [](double) { return "double"; },
        [](auto &&) { return "unknown"; }
    );

    auto print_type_name = [&]<typename Variant>(Variant &&var) {
        std::cout << std::visit(type_name_visitor, std::forward<Variant>(var)) << '\n';
    };

    std::variant<int, double, std::string_view> var = 1;
    print_type_name(var);
    var = 3.14;
    print_type_name(var);
    var = "hello";
    print_type_name(var);
}