# Functional utilities for C++.

This is a small C++ functional programming playground. It includes
a few utility classes and wrapper objects for function pointers.

## Function wrappers
The `function<Return, Args ...>` class template provides an API for working with functions.
The first template parameter is always required and represents the return type of the function.
The following template parameters are all optional and represent the types of the arguments taken by the function.
Any `function` object that doesn't return `void` has its `operator()` automatically marked as `[[nodiscard]]`.
The `default_function` template allows default-constructing functions
that return default-constructible types. 

## Overloading
The library provides a utility function called `overload` that
takes a variable number of callable objects (including C-style function pointers)
and generates a functor which behaves as an overload set for the given callables.

```cpp
auto type_name = fun::overload(
        [](int) { return "int"; },
        [](double) { return "double"; },
);
std::cout << type_name(3435) << '\n'; // "int"
std::cout << type_name(3.14) << '\n'; // "double"
```
## Currying
Currying is the technique of converting a function that takes multiple arguments into a sequence of functions that each takes a single argument.
Using the `curry` function, a callable object can be adapted to
receive its arguments one-by-one.
```cpp
auto curried = fun::curry([](int x, int y, int z) { return x + y + z; });
std::cout << curried(1)(2)(3); // 6
```