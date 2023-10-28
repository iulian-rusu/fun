#include <fun.hpp>

namespace fun::tests
{
    inline constexpr auto sum_10 = with_arity<10>([](std::integral auto... xs) { return (xs + ... + 0); });

    inline constexpr auto sum_10_curried = curry(sum_10);
    inline constexpr auto sum_10_uncurried = uncurry(sum_10_curried);
    inline constexpr auto expected = sum_10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    static_assert(sum_10_curried(1)(2)(3)(4)(5)(6)(7)(8)(9)(10) == expected);
    static_assert(sum_10_uncurried(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == expected);

    static_assert(std::is_invocable_v<decltype(sum_10_curried), int>);
    static_assert(std::is_invocable_v<decltype(sum_10_curried), int, int> == false);
}