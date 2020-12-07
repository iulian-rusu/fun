# Functional types in C++.
Implementation of wrapper objects for function pointers in C++.
The `func_t<return_t, param_t ...>` class template privides an API for working with functions.
The first template parameter represents the return type of the function and is required.
The followind template parameters are all optional and represent the types of arguments taken by the function.
Any function that doesn't return `void` has its `operator()` automatically marked as `[[nodiscard]]`.

The class also holds a `func_t<return_t, param_t ...>::defaults` inner class that defines static functions used as default initializers. 
The use of the default constructor with non-void `return_t` requires it to be default constructible.

The program requires at least C++17 due to the use of `std::is_void_v` and similar variable templates.
