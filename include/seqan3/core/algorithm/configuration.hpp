// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides seqan3::detail::configuration and utility functions.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <tuple>

#include <meta/meta.hpp>

#include <seqan3/core/algorithm/concept.hpp>
#include <seqan3/core/algorithm/configuration_utility.hpp>
#include <seqan3/core/algorithm/pipeable_config_element.hpp>
#include <seqan3/core/type_list/traits.hpp>
#include <seqan3/core/tuple_utility.hpp>
#include <seqan3/core/type_list/type_list.hpp>
#include <seqan3/std/concepts>

namespace seqan3
{

//!\cond
// Forward declaration for friend declaration definitions below.
template <detail::config_element_specialisation ... configs_t>
class configuration;

template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> && lhs,
                         pipeable_config_element<rhs_derived_t, rhs_value_t> && rhs)
{
    return configuration{static_cast<lhs_derived_t &&>(lhs)}.push_back(static_cast<rhs_derived_t &&>(rhs));
}

template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> && lhs,
                         pipeable_config_element<rhs_derived_t, rhs_value_t> const & rhs)
{
    return configuration{static_cast<lhs_derived_t &&>(lhs)}.push_back(static_cast<rhs_derived_t const &>(rhs));
}

template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> const & lhs,
                         pipeable_config_element<rhs_derived_t, rhs_value_t> && rhs)
{
    return configuration{static_cast<lhs_derived_t const &>(lhs)}.push_back(static_cast<rhs_derived_t &&>(rhs));
}

template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> const & lhs,
                         pipeable_config_element<rhs_derived_t, rhs_value_t> const & rhs)
{
    return configuration{static_cast<lhs_derived_t const &>(lhs)}.push_back(static_cast<rhs_derived_t const &>(rhs));
}
//!\endcond

// ----------------------------------------------------------------------------
// configuration
// ----------------------------------------------------------------------------

/*!\brief Collection of elements to configure an algorithm.
 * \ingroup algorithm
 *
 * \tparam configs_t Template parameter pack containing all configuration elements; Must model
 *                   seqan3::detail::config_element_specialisation
 *
 * \details
 *
 * This class provides a unified interface to create and query such
 * configurations for a specific algorithm. It extends the standard tuple interface with some useful functions to modify
 * and query the user configurations.
 */
template <detail::config_element_specialisation ... configs_t>
class configuration : public std::tuple<configs_t...>
{
    //!\brief Friend declaration for other instances of the configuration.
    template <detail::config_element_specialisation ... _configs_t>
    friend class configuration;

public:
    //!\privatesection
    //!\brief A type alias for the base class.
    using base_type = std::tuple<configs_t...>;

    //!\publicsection
    /*!\name Constructor, destructor and assignment
     * \{
     */
    constexpr configuration()                                  = default; //!< Defaulted.
    constexpr configuration(configuration const &)             = default; //!< Defaulted.
    constexpr configuration(configuration &&)                  = default; //!< Defaulted.
    constexpr configuration & operator=(configuration const &) = default; //!< Defaulted.
    constexpr configuration & operator=(configuration &&)      = default; //!< Defaulted.
    ~configuration()                                           = default; //!< Defaulted.

    /*!\brief Constructs a configuration from a single configuration element.
     * \param elem The element to store.
     */
    template <typename derived_t, typename value_t>
    constexpr configuration(pipeable_config_element<derived_t, value_t> && elem) :
        base_type{static_cast<derived_t &&>(std::move(elem))}
    {}

    /*!\brief Constructs a configuration from a single configuration element.
     * \param elem The element to store.
     */
    template <typename derived_t, typename value_t>
    constexpr configuration(pipeable_config_element<derived_t, value_t> const & elem) :
        base_type{static_cast<derived_t const &>(elem)}
    {}
    //!\}

    /*!\name Capacity
     * \{
     */

    //!\brief Returns the number of contained config elements.
    constexpr size_t size() const noexcept
    {
        return std::tuple_size_v<base_type>;
    }

    /*!\name Observers
     * \{
     */

    /*!\brief Returns the stored configuration element if present otherwise the given alternative.
     * \tparam alternative_t The type of the configuration element that is queried.
     *
     * \param[in] alternative The alternative whose type is used to check for an existing configuration element.
     *
     * \details
     *
     * Uses the type `alternative_t` of the given alternative to check if such an configuration element was already
     * stored inside of the configuration. If no suitable candidate can be found the passed value `alternative` will
     * be returned. If `alternative_t` is a class template, then any specialisation of this alternative type will be
     * searched and returned if present.
     *
     * \returns The stored configuration element identified by `alternative_t` or the alternative if not present.
     *
     * ### Example
     *
     * \include test/snippet/core/algorithm/configuration_get_or.cpp
     *
     * ### Exception
     *
     * no-throw guarantee.
     *
     * ### Complexity
     *
     * Constant time.
     */
    template <typename alternative_t>
    constexpr decltype(auto) get_or(alternative_t && alternative) & noexcept
    {
        return get_or_impl(*this, alternative, std::forward<alternative_t>(alternative));
    }

    //!\overload
    template <typename alternative_t>
    constexpr decltype(auto) get_or(alternative_t && alternative) const & noexcept
    {
        return get_or_impl(*this, alternative, std::forward<alternative_t>(alternative));
    }

    //!\overload
    template <typename alternative_t>
    constexpr decltype(auto) get_or(alternative_t && alternative) && noexcept
    {
        return get_or_impl(std::move(*this), alternative, std::forward<alternative_t>(alternative));
    }

    //!\overload
    template <typename alternative_t>
    constexpr decltype(auto) get_or(alternative_t && alternative) const && noexcept
    {
        return get_or_impl(std::move(*this), alternative, std::forward<alternative_t>(alternative));
    }

    //!\brief Checks if the given type exists in the tuple.
    template <typename query_t>
    static constexpr bool exists() noexcept
    {
        return pack_traits::contains<query_t, configs_t...>;
    }
    //!\brief Checks if the given type exists in the tuple.
    template <template <typename ...> typename query_t>
    static constexpr bool exists() noexcept
    {
        return (pack_traits::find_if<detail::is_same_configuration_f<query_t>::template invoke, configs_t...> > -1);
    }
    //!\}

    /*!\name Modifiers
     * \{
     */
    /*!\brief Remove a config element from the configuration.
     * \tparam query_t The config element type to remove from the configuration.
     * \returns A new configuration object without the config element identified by `query_t`.
     */
    template <typename query_t>
    [[nodiscard]] constexpr auto remove() const
    //!\cond
#if !SEQAN3_WORKAROUND_GCC_95371
        requires (exists<query_t>())
#endif // !SEQAN3_WORKAROUND_GCC_95371
    //!\endcond
    {
        constexpr int index = pack_traits::find<query_t, configs_t...>;
        return remove_at<index>();
    }

    //!\overload
    template <template <typename ...> typename query_t>
    [[nodiscard]] constexpr auto remove() const
    //!\cond
#if !SEQAN3_WORKAROUND_GCC_95371
        requires (exists<query_t>())
#endif // !SEQAN3_WORKAROUND_GCC_95371
    //!\endcond
    {
        constexpr int index = pack_traits::find_if<detail::is_same_configuration_f<query_t>::template invoke,
                                                   configs_t...>;
        return remove_at<index>();
    }
    //!\}

    /*!\name Pipe operator
     * \{
     */
    /*!\brief Combines two seqan3::pipeable_config_element objects to a seqan3::configuration.
     * \tparam lhs_derived_t The derived type of the left hand side operand.
     * \tparam lhs_value_t   The value type of the left hand side operand.
     * \tparam rhs_derived_t The derived type of the right hand side operand.
     * \tparam rhs_value_t   The value type of the right hand side operand.
     * \param[in] lhs        The left hand operand.
     * \param[in] rhs        The right hand operand.
     * \returns A new seqan3::configuration containing `lhs` and `rhs`.
     */
    template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> && lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> && rhs);

    //!\overload
    template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> && lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> const & rhs);

    //!\overload
    template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> const & lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> && rhs);

    //!\overload
    template <typename lhs_derived_t, typename lhs_value_t, typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(pipeable_config_element<lhs_derived_t, lhs_value_t> const & lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> const & rhs);

    /*!\brief Combines a seqan3::configuration with a seqan3::pipeable_config_element.
     * \tparam rhs_derived_t The derived type of the right hand side operand.
     * \tparam rhs_value_t   The value type of the right hand side operand.
     * \param[in] lhs     The left hand operand.
     * \param[in] rhs     The right hand operand.
     * \returns A new seqan3::configuration adding `rhs` to the passed `lhs` object.
     */
    template <typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(configuration && lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> && rhs)
    {
        return std::move(lhs).push_back(static_cast<rhs_derived_t &&>(rhs));
    }

    //!\overload
    template <typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(configuration const & lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> && rhs)
    {
        return lhs.push_back(static_cast<rhs_derived_t &&>(rhs));
    }

    //!\overload
    template <typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(configuration && lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> const & rhs)
    {
        return std::move(lhs).push_back(static_cast<rhs_derived_t const &>(rhs));
    }

    //!\overload
    template <typename rhs_derived_t, typename rhs_value_t>
    friend constexpr auto operator|(configuration const & lhs,
                                    pipeable_config_element<rhs_derived_t, rhs_value_t> const & rhs)
    {
        return lhs.push_back(static_cast<rhs_derived_t const &>(rhs));
    }

    /*!\brief Combines two seqan3::configuration objects.
     * \tparam rhs_configs_t  A template parameter pack for the second seqan3::configuration operand.
     * \param[in] lhs         The left hand operand.
     * \param[in] rhs         The right hand operand.
     * \returns A new seqan3::configuration as the result of concatenating `lhs` and `rhs`.
     */
    template <typename ...rhs_configs_t>
    friend constexpr auto operator|(configuration && lhs,
                                    configuration<rhs_configs_t...> && rhs)
    {
        using lhs_base_t = typename configuration::base_type;
        using rhs_base_t = typename configuration<rhs_configs_t...>::base_type;

        return make_configuration(std::tuple_cat(static_cast<lhs_base_t>(std::move(lhs)),
                                                 static_cast<rhs_base_t>(std::move(rhs))));
    }

    //!\overload
    template <typename ...rhs_configs_t>
    friend constexpr auto operator|(configuration const & lhs,
                                    configuration<rhs_configs_t...> && rhs)
    {
        using lhs_base_t = typename configuration::base_type;
        using rhs_base_t = typename configuration<rhs_configs_t...>::base_type;

        return make_configuration(std::tuple_cat(static_cast<lhs_base_t>(lhs),
                                                 static_cast<rhs_base_t>(std::move(rhs))));
    }

    //!\overload
    template <typename ...rhs_configs_t>
    friend constexpr auto operator|(configuration && lhs,
                                    configuration<rhs_configs_t...> const & rhs)
    {
        using lhs_base_t = typename configuration::base_type;
        using rhs_base_t = typename configuration<rhs_configs_t...>::base_type;

        return make_configuration(std::tuple_cat(static_cast<lhs_base_t>(std::move(lhs)),
                                                 static_cast<rhs_base_t>(rhs)));
    }

    //!\overload
    template <typename ...rhs_configs_t>
    friend constexpr auto operator|(configuration const & lhs,
                                    configuration<rhs_configs_t...> const & rhs)
    {
        using lhs_base_t = typename configuration::base_type;
        using rhs_base_t = typename configuration<rhs_configs_t...>::base_type;

        return make_configuration(std::tuple_cat(static_cast<lhs_base_t>(lhs),
                                                 static_cast<rhs_base_t>(rhs)));
    }
    //!\}

private:

    /*!\name Internal constructor
     * \{
     */
    //!\brief Constructs from std::tuple.
    template <typename ..._configs_t>
    explicit constexpr configuration(std::tuple<_configs_t...> const & cfg) : base_type{cfg}
    {}

    //!\brief Constructs from std::tuple.
    template <typename ..._configs_t>
    explicit constexpr configuration(std::tuple<_configs_t...> && cfg) : base_type{std::move(cfg)}
    {}
    //!\}

    /*!\brief Creates a new configuration object by recursively adding the configs from the tuple.
     * \tparam tuple_t The tuple from which to create a new configuration object; must model seqan3::tuple_like.
     * \param[in] tpl The tuple to create the configuration from.
     * \returns A new configuration object.
     */
    template <tuple_like tuple_t>
    static constexpr auto make_configuration(tuple_t && tpl)
    {
        if constexpr (std::tuple_size_v<remove_cvref_t<tuple_t>> == 0)
        {
            return configuration<>{};
        }
        else
        {
            auto impl = [](auto & impl_ref, auto && config, auto && head, auto && tail)
            {
                using cfg_t = decltype(config);
                if constexpr (std::tuple_size_v<remove_cvref_t<decltype(tail)>> == 0)
                {
                    return std::forward<cfg_t>(config).push_back(std::get<0>(std::forward<decltype(head)>(head)));
                }
                else
                {
                    auto [_head, _tail] = tuple_split<1>(std::forward<decltype(tail)>(tail));
                    auto tmp = std::forward<cfg_t>(config).push_back(std::get<0>(std::forward<decltype(head)>(head)));
                    return impl_ref(impl_ref, std::move(tmp), std::move(_head), std::move(_tail));
                }
            };

            auto [head, tail] = tuple_split<1>(std::forward<decltype(tpl)>(tpl));
            return impl(impl, configuration<>{}, std::move(head), std::move(tail));
        }
    }

    /*!\name Modifiers
     * \brief Note that modifications return new configurations and do not modify `this`.
     * \{
     */

    /*!\brief Adds a new config element to the end of the configuration.
     *
     * \param[in] elem The configuration element to add.
     *
     * \returns A new seqan3::detail::configuration containing the added element.
     *
     * \details
     *
     * Creates a new seqan3::detail::configuration from `this` and appends the passed config element.
     * Note, that `this` is not modified by this operation.
     * Further the configuration checks for an invalid configuration using an algorithm specific lookup table
     * for the configuration elements and tests whether configuration elements are from the same algorithm.
     *
     * ### Complexity
     *
     * Linear in the number of elements.
     *
     * ### Exception
     *
     * Strong exception guarantee.
     */
    template <detail::config_element_specialisation config_element_t>
    constexpr auto push_back(config_element_t elem) const &
    {
        static_assert(detail::is_configuration_valid_v<remove_cvref_t<config_element_t>,
                                                            configs_t...>,
                      "Configuration error: The passed element cannot be combined with one or more elements in the "
                      "current configuration.");

        return configuration<configs_t..., std::remove_reference_t<config_element_t>>{
            std::tuple_cat(static_cast<base_type>(*this),
            std::tuple{std::move(elem)})};
    }

    //!\copydoc push_back
    template <detail::config_element_specialisation config_element_t>
    constexpr auto push_back(config_element_t elem) &&
    {
        static_assert(detail::is_configuration_valid_v<remove_cvref_t<config_element_t>,
                                                            configs_t...>,
                      "Configuration error: The passed element cannot be combined with one or more elements in the "
                      "current configuration.");

        return configuration<configs_t..., std::remove_reference_t<config_element_t>>{
            std::tuple_cat(std::move(static_cast<base_type>(*this)),
            std::tuple{std::move(elem)})};
    }

    /*!\brief Remove a config element from the configuration.
     * \tparam index The config element at `index` is removed from the config.
     * \returns A new configuration object without the config element at `index`.
     */
    template <int index>
    [[nodiscard]] constexpr auto remove_at() const
    {
        static_assert((index >= 0) && (index < sizeof...(configs_t)), "Index to remove from config is out of bounds.");

        auto [head, middle] = tuple_split<index>(static_cast<base_type>(*this));
        auto tail = tuple_pop_front(middle);

        return make_configuration(std::tuple_cat(head, tail));
    }
    //!\}

    /*!\brief Internal implementation of the get_or interace.
     *
     * \tparam this_t The type of this.
     * \tparam query_t The type of the configuration element to query.
     * \tparam alternative_t The type of the alternative.
     *
     * \param[in] me The perfectly forwarded instance of `*this`.
     * \param[in] query The queried configuration element [only the type is needed].
     * \param[in] alternative The alternative configuration element to return if the query_t is not present.
     *
     * \details
     *
     * Use the type `query_t` to check if such a configuration element is stored in `me`. If this is `true` then
     * the stored configuration element is returned using perfect forwarding. If this evaluates to `false` the
     * given alternative is returned. If `query_t` is a class template then it is checked if any
     * specialisation of this class template is stored.
     */
    template <typename this_t, typename query_t, typename alternative_t>
    static constexpr decltype(auto) get_or_impl(this_t && me,
                                                query_t const & SEQAN3_DOXYGEN_ONLY(query),
                                                alternative_t && alternative) noexcept
    {
        if constexpr (exists<query_t>())
        {
            return get<query_t>(std::forward<this_t>(me));
        }
        else
        {
            using ret_type = remove_rvalue_reference_t<decltype(alternative)>;
            return static_cast<ret_type>(alternative);
        }
    }

    //!\overload
    template <typename this_t,
              template <typename ...> typename query_template_t, typename ...parameters_t,
              typename alternative_t>
    static constexpr decltype(auto) get_or_impl(this_t && me,
                                                query_template_t<parameters_t...> const &,
                                                alternative_t && alternative) noexcept
    {
        if constexpr (exists<query_template_t>())
        {
            return get<query_template_t>(std::forward<this_t>(me));
        }
        else
        {
            using ret_type = remove_rvalue_reference_t<decltype(alternative)>;
            return static_cast<ret_type>(alternative);
        }
    }
};

/*!\name Type deduction guides
 * \{
 */

/*!\brief Deduces the correct configuration element type from the passed seqan3::pipeable_config_element.
 * \relates seqan3::configuration
 */
template <typename derived_t, typename value_t>
configuration(pipeable_config_element<derived_t, value_t> &&) -> configuration<remove_cvref_t<derived_t>>;

/*!\brief Deduces the correct configuration element type from the passed seqan3::pipeable_config_element.
 * \relates seqan3::configuration
 */
template <typename derived_t, typename value_t>
configuration(pipeable_config_element<derived_t, value_t> const &) -> configuration<remove_cvref_t<derived_t>>;
//!\}

/*!\name Tuple interface
 * \{
 */

/*!\brief Returns the stored element.
 * \ingroup algorithm
 * \relates seqan3::configuration
 *
 * \tparam    query_t A template template.
 * \param[in] config  The configuration to get the element for.
 *
 * \details
 *
 * Extends the position-based and type based `get` interface for the configuration type, with a version
 * that also accepts template-template types (types that are itself templates), such that the exact template definition
 * must not be known.
 *
 * ### Example
 *
 * The following snippet demonstrates the various versions of get that can be used.
 *
 * \include test/snippet/core/algorithm/configuration_get.cpp
 *
 * ### Exception
 *
 * no-throw guarantee.
 *
 * ### Complexity
 *
 * Constant time.
 */
template <template <typename ...> class query_t, typename ...configs_t>
constexpr auto & get(configuration<configs_t...> & config) noexcept
{
    constexpr auto pos = pack_traits::find_if<detail::is_same_configuration_f<query_t>::template invoke, configs_t...>;
    static_assert(pos > -1, "Access error: The requested type is not contained.");

    return get<pos>(config);
}

//!\overload
template <template <typename ...> class query_t, typename ...configs_t>
constexpr auto const & get(configuration<configs_t...> const & config) noexcept
{
    constexpr auto pos = pack_traits::find_if<detail::is_same_configuration_f<query_t>::template invoke, configs_t...>;
    static_assert(pos > -1, "Access error: The requested type is not contained.");

    return get<pos>(config);
}

//!\overload
template <template <typename ...> class query_t, typename ...configs_t>
constexpr auto && get(configuration<configs_t...> && config) noexcept
{
    constexpr auto pos = pack_traits::find_if<detail::is_same_configuration_f<query_t>::template invoke, configs_t...>;
    static_assert(pos > -1, "Access error: The requested type is not contained.");

    return get<pos>(std::move(config));
}

//!\overload
template <template <typename ...> class query_t, typename ...configs_t>
constexpr auto const && get(configuration<configs_t...> const && config) noexcept
{
    constexpr auto pos = pack_traits::find_if<detail::is_same_configuration_f<query_t>::template invoke, configs_t...>;
    static_assert(pos > -1, "Access error: The requested type is not contained.");

    // TODO: change after GCC-7 bug with const && version of get in std::tuple is fixed.
    // return get<pos>(std::move(config));
    return std::move(get<pos>(config));
}
//!\}

} // namespace seqan3::detail

namespace std
{
//!\cond DEV

/*!\brief Returns the number of elements stored in seqan3::detail::configuration.
 * \implements seqan3::unary_type_trait
 * \see std::tuple_size_v
 * \ingroup algorithm
 */
template <seqan3::detail::config_element_specialisation ... configs_t>
struct tuple_size<seqan3::configuration<configs_t...>>
{
    //!\brief The number of elements.
    static constexpr size_t value = std::tuple_size_v<typename seqan3::configuration<configs_t...>::base_type>;
};

/*!\brief Returns the type of the element at the specified position within seqan3::configuration.
 * \implements seqan3::transformation_trait
 * \see [std::tuple_element](https://en.cppreference.com/w/cpp/utility/tuple/tuple_element)
 * \ingroup algorithm
 */
template <size_t pos, seqan3::detail::config_element_specialisation ... configs_t>
struct tuple_element<pos, seqan3::configuration<configs_t...>>
{
    //!\brief The type of the config at position `pos`
    using type = std::tuple_element_t<pos, typename seqan3::configuration<configs_t...>::base_type>;
};
//!\endcond
}  //namespace std
