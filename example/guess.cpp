#include <iostream>
#include <fun.hpp>

int main()
{
    using namespace fun::literals;
    using fun::tag;

    auto guess = fun::curry(
        fun::overload(
            [](tag<1>, tag<2>, tag<3>, tag<4>) { return "you guessed 1234!"; },
            [](tag<1>, tag<3>, tag<3>, tag<7>) { return "you guessed 1337!"; },
            fun::nullsink<4>([] { return "incorrect guess!"; })
        )
    );
    std::cout << guess(1_t)(2_t)(3_t)(4_t) << '\n';
    std::cout << guess(1_t)(3_t)(3_t)(7_t) << '\n';
    std::cout << guess(2_t)(2_t)(3_t)(4_t) << '\n';
}

