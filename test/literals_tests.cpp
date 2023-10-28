#include <fun.hpp>

namespace fun::tests
{
    using namespace literals;

    template<std::intmax_t ID>
    inline constexpr auto value_tag = fun::tag<ID>{};

    static_assert(1_t == value_tag<1>);
    static_assert(2_t == value_tag<2>);
    static_assert(3_t == value_tag<3>);
    static_assert(10_t == value_tag<10>);
    static_assert(11_t == value_tag<11>);
    static_assert(99_t == value_tag<99>);
    static_assert(12345_t == value_tag<12345>);
    static_assert(-12345_t == value_tag<-12345>);
    static_assert(0x12_t == value_tag<0x12>);
    static_assert(0345_t == value_tag<0345>);
    static_assert(0b101011101_t == value_tag<0b101011101>);
    static_assert(-0b101011101_t == value_tag<-0b101011101>);
}