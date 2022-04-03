#include <iostream>
#include <functional.hpp>

long sum_three_nums(int first, unsigned second, long third)
{
    return first + second + third;
}

int increment(int x)
{
    return x + 1;
}

int main(){
    // OK, int has a default constructor, calling def_func will return 0.
    function<int> default_func;
    std::cout << "default_func() = " << default_func() << '\n';

    // Creating a partial from a given function.
    function sum_func = sum_three_nums;
    auto partial_func = sum_func.bind_front(10);
    std::cout << "sum_func(10, 20, 30) = " << partial_func(20u, 30L) << '\n';

    // Overloading multiple functions into one object.
    auto overloaded_func = overload(sum_three_nums, [](std::string_view name) {
        std::cout << "Hello, " << name << '\n';
    });
    std::cout << overloaded_func(1, 2, 3) << '\n';
    auto extended = overloaded_func.extend(increment);
    std::cout << extended(41) << '\n';
    extended("World");

    // Adapting a function to be curried
    auto curried_sum = curry(sum_three_nums);
    std::cout << curried_sum(10)(20)(30) << '\n';
}
