# Functional types in C++.
Implementation of wrapper objects for function pointers and functional programming utilities in C++.
The `function<Return, Params ...>` class template provides an API for working with functions.
The first template parameter is always required and represents the return type of the function.
The following template parameters are all optional and represent the types of the arguments taken by the function.
Any `function` object that doesn't return `void` has its `operator()` automatically marked as `[[nodiscard]]`.

The `function` class template holds static functions used as default initializers. 
Usage of the default constructor requires any non-void `Return` to be default constructible.

The header also implements a `is_callable` type trait which tests if any type has at least one `operator()` definition.
