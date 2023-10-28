#include <iostream>
#include <string_view>
#include <fun.hpp>

class vault
{
    std::string_view _password = "secret";
};

class earth
{
    double _gravity = 9.807;
    std::size_t _radius = 6371000;
};

// Accessing private member pointers can be done by injecting them into supplied functions
template struct fun::member_pointers<&vault::_password, &earth::_gravity, &earth::_radius>::match<
    [](double earth::*ptr) {
        earth e{};
        std::cout << "Gravity is " << e.*ptr << '\n';
        e.*ptr = 99.999;
        std::cout << "Gravity modified to " << e.*ptr << '\n';
    },
    [](std::string_view vault::*ptr) {
        vault v{};
        std::cout << "The password is " << v.*ptr << '\n';
    },
    [](auto other) {
        std::cout << "Unknown type: " << typeid(other).name() << '\n';
    }
>;

struct password_accessor
{
    static inline std::string_view vault::*ptr = nullptr;
};

// We can also enable public access to _password through password_accessor
template struct fun::enable_access<&vault::_password, password_accessor>;

int main()
{
    vault v{};
    v.*password_accessor::ptr = "updated";
    std::cout << "The password is " << v.*password_accessor::ptr << '\n';
}


