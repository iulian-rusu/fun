#include <fun.hpp>

namespace fun::tests
{
    using no_args_callable = decltype(with_arity<0>([](auto &&...) {}));
    using unary_callable = decltype(with_arity<1>([](auto &&...) {}));
    using binary_callable = decltype(with_arity<2>([](auto &&...) {}));

    static_assert(std::is_invocable_v<no_args_callable>);
    static_assert(std::is_invocable_v<no_args_callable, tag<1>> == false);
    static_assert(std::is_invocable_v<no_args_callable, tag<1>, tag<2>> == false);

    static_assert(std::is_invocable_v<unary_callable> == false);
    static_assert(std::is_invocable_v<unary_callable, tag<1>>);
    static_assert(std::is_invocable_v<unary_callable, tag<1>, tag<2>> == false);

    static_assert(std::is_invocable_v<binary_callable> == false);
    static_assert(std::is_invocable_v<binary_callable, tag<1>> == false);
    static_assert(std::is_invocable_v<binary_callable, tag<1>, tag<2>>);
    static_assert(std::is_invocable_v<binary_callable, tag<1>, tag<2>, tag<3>> == false);
}