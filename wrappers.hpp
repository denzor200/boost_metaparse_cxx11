// Copyright (c) 2025 Denis Mikhailov
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAPARSE_CXX11_WRAPPER_HPP_INCLUDED
#define BOOST_METAPARSE_CXX11_WRAPPER_HPP_INCLUDED

#include <boost/metaparse/foldl_reject_incomplete.hpp>
#include <boost/metaparse/foldl_reject_incomplete1.hpp>
#include <boost/metaparse/lit_c.hpp>
#include <boost/metaparse/transform.hpp>
#include <boost/metaparse/one_char_except_c.hpp>
#include <boost/metaparse/one_of.hpp>
#include <boost/metaparse/always_c.hpp>
#include <boost/metaparse/build_parser.hpp>
#include <boost/metaparse/middle_of.hpp>
#include <boost/metaparse/entire_input.hpp>
#include <boost/metaparse/string_value.hpp>

// #include <boost/callable_traits/args.hpp>

namespace boost { namespace metaparse { namespace cxx11 {

// ============================================================================
// Adapter for functional objects passed to metafunction
// ============================================================================

namespace detail {

template<typename FuncObj>
struct func_obj2_to_metafunction {
    // static_assert(std::tuple_size_v<
    //     boost::callable_traits::args_t<FuncObj>> == 2,
    //     "Function object must take exactly 2 arguments");

    // TODO: fail compilation if FuncObj not a value based

    template<typename A, typename B>
    struct apply {
        using type = apply<A, B>;
        static auto run() -> decltype(FuncObj{}(A::type::run(), B::type::run())) {
            return FuncObj{}(A::type::run(), B::type::run());
        }
    };
};

template<typename FuncObj>
struct func_obj0_to_metafunction {
    // static_assert(std::tuple_size_v<
    //     boost::callable_traits::args_t<FuncObj>> == 0,
    //     "Function object must take exactly 0 arguments");

    // TODO: fail compilation if FuncObj not a value based

    using type = func_obj0_to_metafunction<FuncObj>;
    static auto run() -> decltype(FuncObj{}()) {
        return FuncObj{}();
    }
};

template<typename FuncObj>
struct func_obj1_to_metafunction {
    // static_assert(std::tuple_size_v<
    //     boost::callable_traits::args_t<FuncObj>> == 1,
    //     "Function object must take exactly 1 argument");

    // TODO: fail compilation if FuncObj not a value based

    template<typename A>
    struct apply {
        using type = apply<A>;
        static auto run() -> decltype(FuncObj{}(
                // TODO: make it more generic
                std::integral_constant<char, A::type::value>{}
            )) {
            return FuncObj{}(
                // TODO: make it more generic
                std::integral_constant<char, A::type::value>{}
            );
        }
    };
};

} // namespace detail

// ============================================================================
// Value-based wrappers for metafunctions
// ============================================================================

template<typename Parser>
struct parser_wrapper {
    // Calling operator to apply input data to the parser
    template<typename Input>
    auto operator()(Input) const noexcept ->
        decltype( Parser::template apply<Input>::type::run())
    {
        return  Parser::template apply<Input>::type::run();
    }

    // Alternative syntax via `parse` method
    template<typename Input>
    auto parse(Input) const noexcept ->
        decltype( Parser::template apply<Input>::type::run())
    {
        return  Parser::template apply<Input>::type::run();
    }
};

template<typename Parser>
constexpr auto build_parser(Parser) -> parser_wrapper<boost::metaparse::build_parser<Parser>> {
    return {};
}

// ============================================================================
// Wrappers for combinations of parsers
// ============================================================================

template<typename Left, typename Middle, typename Right>
constexpr auto middle_of(Left, Middle, Right) -> 
    boost::metaparse::middle_of<Left, Middle, Right>
{
    return {};
}

template<typename... Parsers>
constexpr auto one_of(Parsers...) ->
    boost::metaparse::one_of<Parsers...>
{
    return {};
}

template<char C>
constexpr auto lit_c() ->
    boost::metaparse::lit_c<C>
{
    return {};
}

template<char C, typename Result>
constexpr auto always_c(Result) ->
    boost::metaparse::always_c<C, detail::func_obj0_to_metafunction<Result>>
{
    return {};
}

template<char... Except>
constexpr auto one_char_except_c() ->
    boost::metaparse::one_char_except_c<Except...>
{
    return {};
}

template<typename Parser, typename Transformer>
constexpr auto transform(Parser, Transformer) ->
    boost::metaparse::transform<Parser, detail::func_obj1_to_metafunction<Transformer>>
{
    return {};
}

// ============================================================================
// Wrappers for fold functions
// ============================================================================

namespace detail {
template<typename Parser, typename State, typename Func>
struct foldl_reject_incomplete_wrapper {
    using adapted_state_t = detail::func_obj0_to_metafunction<State>;
    using adapted_func_t = detail::func_obj2_to_metafunction<Func>;
    using type = boost::metaparse::foldl_reject_incomplete<
        Parser,
        adapted_state_t,
        adapted_func_t
    >;
};
}

template<typename Parser, typename State, typename Func>
constexpr auto foldl_reject_incomplete(Parser, State, Func) ->
    typename detail::foldl_reject_incomplete_wrapper<Parser, State, Func>::type
{
    return {};
}

namespace detail{
template<typename Parser, typename State, typename Func>
struct foldl_reject_incomplete1 {
    using adapted_state_t = detail::func_obj0_to_metafunction<State>;
    using adapted_func_t = detail::func_obj2_to_metafunction<Func>;
    using type = boost::metaparse::foldl_reject_incomplete1<
        Parser,
        adapted_state_t,
        adapted_func_t
    >;
};
}

template<typename Parser, typename State, typename Func>
constexpr auto foldl_reject_incomplete1(Parser, State, Func) ->
    typename detail::foldl_reject_incomplete1<Parser, State, Func>::type
{
    return {};
}

// ============================================================================
// Wrapper for `entire_input`
// ============================================================================

template<typename Parser>
constexpr auto entire_input(Parser) ->
    boost::metaparse::entire_input<Parser>
{
    return {};
}

}}} // namespace boost::metaparse::cxx11

#endif  // #ifndef BOOST_METAPARSE_CXX11_WRAPPER_HPP_INCLUDED
