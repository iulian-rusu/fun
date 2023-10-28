#include <fun.hpp>

namespace fun::tests
{
    using namespace literals;

    template<std::intmax_t A, std::intmax_t B>
    constexpr tag<B> map(tag<A>) noexcept { return {}; }

    inline constexpr auto unit = overload(
        map<1, 0>,
        map<2, 1>,
        map<3, 1>,
        map<4, 2>,
        map<5, 3>,
        map<6, 5>,
        map<7, 8>,
        map<8, 13>,
        map<9, 21>,
        [](){}
    );

    static_assert(unit(1_t) == 0_t);
    static_assert(unit(2_t) == 1_t);
    static_assert(unit(3_t) == 1_t);
    static_assert(unit(4_t) == 2_t);
    static_assert(unit(5_t) == 3_t);
    static_assert(unit(6_t) == 5_t);
    static_assert(unit(7_t) == 8_t);
    static_assert(unit(8_t) == 13_t);
    static_assert(unit(9_t) == 21_t);
    static_assert(std::is_void_v<decltype(unit())>);
    static_assert(std::is_invocable_v<decltype(unit), int> == false);
}