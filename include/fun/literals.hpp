#ifndef FUN_LITERALS_HPP
#define FUN_LITERALS_HPP

#include <fun/utility.hpp>

namespace fun
{
    namespace detail
    {
        // Shamelessly stolen from Boost.Hana:
        // https://github.com/boostorg/hana/blob/master/include/boost/hana/bool.hpp

        constexpr int to_int(char c)
        {
            return [=] {
                if (c >= 'A' && c <= 'F')
                    return static_cast<int>(c) - static_cast<int>('A') + 10;
                else if (c >= 'a' && c <= 'f')
                    return static_cast<int>(c) - static_cast<int>('a') + 10;
                else
                    return static_cast<int>(c) - static_cast<int>('0');
            }();
        }

        template<std::size_t N>
        constexpr std::intmax_t parse(char const (&arr)[N])
        {
            std::intmax_t base = 10;
            std::size_t offset = 0;
            if (N > 2)
            {
                bool starts_with_zero = arr[0] == '0';
                bool is_hex = starts_with_zero && arr[1] == 'x';
                bool is_binary = starts_with_zero && arr[1] == 'b';

                if (is_hex)
                {
                    //0xDEADBEEF (hexadecimal)
                    base = 16;
                    offset = 2;
                }
                else if (is_binary)
                {
                    //0b101011101 (binary)
                    base = 2;
                    offset = 2;
                }
                else if (starts_with_zero)
                {
                    //012345 (octal)
                    base = 8;
                    offset = 1;
                }
            }

            std::intmax_t number = 0;
            std::intmax_t multiplier = 1;
            for (std::size_t i = 0; i < N - offset; ++i)
            {
                char c = arr[N - 1 - i];
                if (c != '\'')
                {
                    number += to_int(c) * multiplier;
                    multiplier *= base;
                }
            }
            return number;
        }
    }

    namespace literals
    {
        template<char... Chars>
        constexpr auto operator ""_t() noexcept
        {
            return tag<detail::parse({Chars ...})>{};
        }
    }
}
#endif //FUN_LITERALS_HPP