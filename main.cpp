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
    friend std::ostream &operator<<(std::ostream &os, const NoDefault&)
    {
        return os;
    }
};

int main()
{
    // won't compile -> NoDefault is not default constructible
    //func_t<NoDefault> f1;

    // OK -> int has a default constructor, calling f2 will return 0
    func_t<int> f2;
    std::cout << "Type of f2 is " << f2.name() << '\n';
    std::cout << "f2() = " << f2() << '\n' << '\n';

    // creating a partial from a given function
    func_t sum_func = sum_them;
    std::cout << "Type of sum_func is " << sum_func.name() << '\n';
    // partial_func will capture the first parameter to be 10
    auto partial_func = sum_func.partial(10);
    // call the partial, providing the rest of the parameters
    std::cout << "sum_func.partial(10)(20, 30) = " <<  partial_func(20u, 30L) << '\n' << '\n';

    // we can also make functions that take other functions as parameters
    func_t applier = apply<long, int, unsigned, long>;
    std::cout << "Type of applier is " << applier.name() << '\n';
    // equivalent to sum_func(1, 2, 3)
    std::cout << "applier(sum_func, 1, 2, 3) = " << applier(sum_func, 1, 2u, 3L) << '\n' << '\n';
}
