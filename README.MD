# Functional types in C++.
Implementation of wrapper objects for function pointers in C++.
The `function<return_t, Params ...>` class template provides an API for working with functions.
The first template parameter is always required and represents the return type of the function.
The followind template parameters are all optional and represent the types of the arguments taken by the function.
Any function that doesn't return `void` has its `operator()` automatically marked as `[[nodiscard]]`.

The class template holds a `defaults` inner class that defines static functions used as default initializers. 
Usage of the default constructor requires any non-void `return_t` to be default constructible.

The program requires at least C++17 due to the use of some specific `type_traits` definitions.
