# Functional types in C++.
Implementation of wrapper objects for Function pointers in C++.
The `Function<Return, Params ...>` class template provides an API for working with functions.
The first template parameter is always required and represents the return type of the Function.
The followind template parameters are all optional and represent the types of the arguments taken by the Function.
Any Function that doesn't return `void` has its `operator()` automatically marked as `[[nodiscard]]`.

The class template holds a `Defaults` inner class that defines static functions used as default initializers. 
Usage of the default constructor requires any non-void `Return` to be default constructible.

The program requires at least C++17 due to the use of some specific `type_traits` definitions.
