# Fun utilities for C++.
This is a C++20 library with random utilities and fun tricks that make use of some obscure C++ techniques.

## Overloading
The library provides a function called `fun::overload` that takes a variable number of callable objects 
(including function pointers) and generates a functor which behaves as an overload set for the given callables.

```cpp
auto type_name = fun::overload(
    [](int) { return "int"; },
    [](double) { return "double"; },
    [](auto) { return "unknown"; }
);
std::cout << type_name(3435) << '\n'; // "int"
std::cout << type_name(3.14) << '\n'; // "double"
std::cout << type_name("ab") << '\n'; // "unknown"
```

## Currying
Currying is the technique of converting a function that takes multiple arguments into a sequence of functions
that each takes a single argument. Using the `fun::curry` function, a callable object can be adapted to receive
its arguments one-by-one. This operation can be undone with the `fun::uncurry` function.

```cpp
auto original = [](int x, int y, int z) { return x + y + z; };
auto curried = fun::curry(original);
auto uncurried = fun::uncurry(curried);
std::cout << original(1, 2, 3) << '\n';  // 6
std::cout << curried(1)(2)(3) << '\n';   // 6
std::cout << uncurried(1, 2, 3) << '\n'; // 6
```

## Member pointers

As we know, encapsulation in C++ is just an illusion. After all, private class members are just a `reinterpret_cast`
away from being accessible. However, this library can do it without undefined behavior by (ab)using member pointers.

Consider the following class definitions:
```cpp
class vault
{
    std::string_view _password = "secret";
};

class earth
{
    double _gravity = 9.807;
    std::size_t _radius = 6371000;
};
```

The `fun::enable_access` template provides a way to enable public access to any class member.
In the example below, the private member `_password` becomes accessible through `password_accessor::ptr`.
This is possible because explicit template instantiations do not follow usual access restriction rules, 
providing a legal way to obtain a pointer to a private member.
```cpp
struct password_accessor
{
    static inline std::string_view vault::*ptr = nullptr;
};

// Explicit instantiation: password is effectively public now
template struct fun::enable_access<&vault::_password, password_accessor>;

int main()
{
    vault v{};
    std::cout << "The password is " << v.*password_accessor::ptr << '\n';
}
```

A more generic interface is offered by `fun::member_pointers`. This template offers inner metafunctions which can 
inject the selected memeber pointers into callable objects. Injection can be done in multiple ways:
* Using `access`, which will pass all pointers at once to a supplied callable.
* Using `visit`, which will invoke a given visitor for each pointer type.
* Using `match`, which takes a list of callables and injects each pointer type into its matching signature.

```cpp
template struct fun::member_pointers<&vault::_password, &earth::_gravity, &earth::_radius>::match<
    [](double earth::*ptr) { // Will be called with &earth::_gravity
        earth e{};
        std::cout << "Gravity is " << e.*ptr << '\n';
        e.*ptr = 99.999;
        std::cout << "Gravity modified to " << e.*ptr << '\n';
    },
    [](std::string_view vault::*ptr) { // Will be called with &vault::_password
        vault v{};
        std::cout << "The password is " << v.*ptr << '\n';
    },
    [](auto other) { // Will be called when no other signature works
        std::cout << "Unknown type: " << typeid(other).name() << '\n';
    }
>;
```