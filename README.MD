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
receive its arguments one-by-one.  This operation can be undone with the
`uncurry` function.

```cpp
auto original = [](int x, int y, int z) { return x + y + z; };
auto curried = fun::curry(original);
auto uncurried = fun::uncurry(curried);
std::cout << original(1, 2, 3) << '\n';  // 6
std::cout << curried(1)(2)(3) << '\n';   // 6
std::cout << uncurried(1, 2, 3) << '\n'; // 6
```

## Null sinks

The `nullsink<N>` function template can be used to create a callable object which
consumes exactly `N` arguments of any type and discards them. 
Optionally, it can also invoke a given callback function (which takes no arguments).
Its `operator()` requires an implicit conversion, giving it lower overload resolution priority
than exact signature matches. 

In the example below, `nullsink<4>` can consume any 4 argument types, but will only
be called in the last line because the other two overloads are better matches.

```cpp
using namespace fun::literals;
using fun::tag;

auto guess = fun::curry(
    fun::overload(
        [](tag<1>, tag<2>, tag<3>, tag<4>) { return "you guessed 1234!"; },
        [](tag<1>, tag<3>, tag<3>, tag<7>) { return "you guessed 1337!"; },
        fun::nullsink<4>([] { return "incorrect guess!"; })
    )
);
std::cout << guess(1_t)(2_t)(3_t)(4_t) << '\n';
std::cout << guess(1_t)(3_t)(3_t)(7_t) << '\n';
std::cout << guess(2_t)(2_t)(3_t)(4_t) << '\n'; // This will call the "null sink"
```

**Note:** 
Some library features might not work with `mutable` (🤢) lambda expressions.
