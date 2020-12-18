#include "func_types.h"
#include <iostream>

long sum_them(int first, unsigned second, long third)
{
    return first + second + third;
}

template<typename return_t, typename ... param_t>
return_t apply(func_t<return_t, param_t ...> fn, param_t ... params)
{
    return fn(params ...);
}

struct NoDefault
{
    NoDefault() = delete;
};

int increment(int x)
{
    return x + 1;
}

int main()
{
    // func_t<NoDefault> f1; /* won't compile, NoDefault is not default constructible */

    // OK, int has a default constructor, calling f2 will return 0
    func_t<int> def_func;
    std::cout << "Type of def_func is:\t\t\t\t" << def_func.class_type_name << '\n';
    std::cout << "Type of def_func's pointer is:\t\t" << def_func.ptr_type_name << '\n';
    std::cout << "def_func() = " << def_func() << '\n' << '\n';

    // creating a partial from a given function
    func_t sum_func = sum_them;
    std::cout << "Type of sum_func is:\t\t\t\t" << sum_func.class_type_name << '\n';
    std::cout << "Type of sum_func's pointer is:\t\t" << sum_func.ptr_type_name << '\n';
    // partial_func will capture the first parameter to be 10
    auto partial_func = sum_func.partial(10);
    // call the partial, providing the rest of the parameters
    std::cout << "sum_func.(10, 20, 30) = " <<  partial_func(20u, 30L) << '\n' << '\n';

    // we can also make functions that take other functions as parameters
    func_t applier = apply<long, int, unsigned, long>;
    std::cout << "Type of applier is:\t\t\t\t\t" << applier.class_type_name << '\n';
    std::cout << "Type of applier's pointer is:\t\t" << applier.ptr_type_name << '\n';
    // equivalent to sum_func(1, 2, 3)
    std::cout << "applier(sum_func, 1, 2, 3) = " << applier(sum_func, 1, 2u, 3L) << '\n' << '\n';
}
