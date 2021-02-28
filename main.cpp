#include <iostream>
#include <vector>
#include "function.h"
#include "function_utils.h"

long sum_them(int first, unsigned second, long third)
{
    return first + second + third;
}

template<typename Return, typename ... Params>
Return apply(function<Return, Params ...> fn, Params ... params)
{
    return fn(params ...);
}

int increment(int x)
{
    return x + 1;
}

class Test
{
};

int main()
{
    // OK, int has a default constructor, calling def_func will return 0.
    function<int> def_func;
    std::cout << "Type of def_func is:\t\t\t\t" << def_func.class_name << '\n';
    std::cout << "Type of def_func's pointer is:\t\t" << def_func.fptr_name << '\n';
    std::cout << "def_func() = " << def_func() << '\n' << '\n';

    // Creating a partial from a given function.
    function sum_func = sum_them;
    std::cout << "Type of sum_func is:\t\t\t\t" << sum_func.class_name << '\n';
    std::cout << "Type of sum_func's pointer is:\t\t" << sum_func.fptr_name << '\n';
    // partial_func will capture the first parameter as 10.
    auto partial_func = sum_func.partial(10);
    // Call the partial, providing the rest of the parameters.
    std::cout << "sum_func(10, 20, 30) = " << partial_func(20u, 30L) << '\n' << '\n';

    // We can also make functions that take other functions as parameters.
    function applier = apply<long, int, unsigned, long>;
    std::cout << "Type of applier is:\t\t\t\t\t" << applier.class_name << '\n';
    std::cout << "Type of applier's pointer is:\t\t" << applier.fptr_name << '\n';
    // Equivalent to sum_func(1, 2, 3)
    std::cout << "applier(sum_func, 1, 2, 3) = " << applier(sum_func, 1, 2u, 3L) << '\n' << '\n';

    // Constructing an action from a lambda. Unfortunately, template type deduction does not work here.
    // We have to specify all template types explicitly.
    action<std::string_view> print_message = [](std::string_view name)
    { std::cout << "Hello, " << name << "!\n"; };
    print_message("John");

    // Default constructing a comparator function.
    // The actual types of the function parameters are int and std::vector<int> const &.
    comparator<int const &, std::vector<int>> cmp;
    std::cout << "Type of comparator is: \t" << cmp.class_name << '\n' << '\n';

    // Combining multiple functions into one object.
    auto combined_func = combine(sum_func, print_message);
    std::cout << combined_func(1, 2, 3) << '\n';
    auto extended = combined_func.extend(increment);
    std::cout << extended(41) << '\n';
    extended("Mike");

    // Testing if types are callable.
    std::cout << "\nIs the type callable?\n";
    std::cout << "functor:\t" << is_callable_v<decltype(extended)> << '\n';
    std::cout << "functor &:\t" << is_callable_v<decltype(extended) &> << '\n';
    std::cout << "functor *:\t" << is_callable_v<decltype(extended) *> << '\n';
    std::cout << "function pointer:\t" << is_callable_v<decltype(sum_them)> << '\n';
    std::cout << "float:\t" << is_callable_v<float> << '\n';
    std::cout << "std::string &:\t" << is_callable_v<std::string &> << '\n';
    std::cout << "std::string const &:\t" << is_callable_v<std::string const &> << '\n';
    std::cout << "std::string &&:\t" << is_callable_v<std::string &&> << '\n';
    std::cout << "std::string *:\t" << is_callable_v<std::string *> << '\n';


}
