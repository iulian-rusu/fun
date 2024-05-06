#ifndef FUN_MEMBER_POINTER_HPP
#define FUN_MEMBER_POINTER_HPP

#include <concepts>
#include <type_traits>
#include <fun/overload.hpp>

namespace fun
{
    template<typename Ptr>
    concept member_pointer = std::is_member_pointer_v<Ptr>;

    template<typename T, typename Ptr>
    concept member_pointer_accessor = member_pointer<Ptr> && requires (Ptr ptr) {
        T::ptr = ptr;
    };

    /**
     * An explicit instantion of this template enables access to a (possibly private) member pointer
     * through a mutable static member of a given type.
     *
     * @tparam Ptr      The member pointer to access
     * @tparam Accessor The type through which the pointer will be accessed
     */
    template<member_pointer auto Ptr, member_pointer_accessor<decltype(Ptr)> Accessor>
    struct enable_access
    {
        static inline auto const ptr = Accessor::ptr = Ptr;
    };

    template<typename V, typename... Ptrs>
    concept visitor = (std::invocable<V, Ptrs> && ...);

    /**
     * Explicit instantions of this template's inner metafunctions allow performing actions on
     * a sequence of (possibly private) member pointers.
     */
    template<member_pointer auto... Ptrs>
    struct member_pointers
    {
        /**
         * Invokes a callable, passing all member pointers in the list to it as parameters.
         *
         * @tparam Consumer Callable that accepts the entire sequence of member pointers
         */
        template<std::invocable<decltype(Ptrs) ...> auto Consumer>
        struct access
        {
            [[maybe_unused]] static inline auto const _ = (Consumer(Ptrs ...), std::type_identity<void>{});
        };

        /**
         * Invokes the callable once on every member pointer in the sequence.
         * If the sequence is heterogeneous, the visitor should support multiple call signatures
         * in order to accept all pointer types.
         *
         * @tparam Visitor  Callable that accepts any member pointer in the sequence
         */
        template<visitor<decltype(Ptrs) ...> auto Visitor>
        struct visit
        {
            [[maybe_unused]] static inline auto const _ = (Visitor(Ptrs), ..., std::type_identity<void>{});
        };

        /**
         * Matches each member pointer in the sequence with exactly one of the supploed callables.
         * The matching is done according to function overloading rules.
         *
         * @tparam Matchers Callables whose overload set accepts any member pointer in the sequence
         */
        template<auto... Matchers>
        requires visitor<decltype(overload(Matchers ...)), decltype(Ptrs) ...>
        struct match
        {
            static constexpr auto overloaded = overload(Matchers ...);
            [[maybe_unused]] static inline auto const _ = (overloaded(Ptrs), ..., std::type_identity<void>{});
        };
    };
}
#endif //FUN_MEMBER_POINTER_HPP