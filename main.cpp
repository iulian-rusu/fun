#include <iostream>
#include <fun.hpp>

int main()
{
    using fun::tag;
    using fun::tag_type;

    auto guess = fun::curry(
        fun::overload(
            [](tag_type<1>, tag_type<2>, tag_type<3>, tag_type<4>) { return "you guessed 1234!"; },
            [](tag_type<1>, tag_type<3>, tag_type<3>, tag_type<7>) { return "you guessed 1337!"; },
            fun::sink<4>([]() { return "incorrect guess!"; })
        )
    );
    std::cout << guess(tag<1>)(tag<2>)(tag<3>)(tag<4>) << '\n';
    std::cout << guess(tag<1>)(tag<3>)(tag<3>)(tag<7>) << '\n';
    std::cout << guess(tag<2>)(tag<2>)(tag<3>)(tag<4>) << '\n'; // This will call the 'sink'

    auto original = [](int x, int y, int z) { return x + y + z; };
    auto curried = fun::curry(original);
    auto uncurried = fun::uncurry(curried);
    std::cout << original(1, 2, 3) << '\n';  // 6
    std::cout << curried(1)(2)(3) << '\n';   // 6
    std::cout << uncurried(1, 2, 3) << '\n'; // 6
}

