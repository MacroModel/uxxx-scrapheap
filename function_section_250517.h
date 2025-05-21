/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @brief       WebAssembly Release 1.0 (2019-07-20)
 * @details     antecedent dependency: null
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-05-07
 * @copyright   APL-2 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

#pragma once

#ifdef UWVM_MODULE
import fast_io;
# ifdef UWVM_TIMER
import uwvm2.utils.debug;
# endif
import uwvm2.parser.wasm.base;
import uwvm2.parser.wasm.concepts;
import uwvm2.parser.wasm.standard.wasm1.type;
import uwvm2.parser.wasm.standard.wasm1.section;
import uwvm2.parser.wasm.standard.wasm1.opcode;
import uwvm2.parser.wasm.binfmt.binfmt_ver1;
import :def;
import :types;
import :feature_def;
import :type_section;
#else
// std
# include <cstddef>
# include <cstdint>
# include <concepts>
# include <type_traits>
# include <utility>
# include <memory>
# include <bit>
// macro
# include <uwvm2/utils/macro/push_macros.h>
// import
# include <fast_io.h>
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/vector.h>
# include <fast_io_dsal/string_view.h>
# ifdef UWVM_TIMER
#  include <uwvm2/utils/debug/impl.h>
# endif
# include <uwvm2/parser/wasm/base/impl.h>
# include <uwvm2/parser/wasm/concepts/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/type/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/section/impl.h>
# include <uwvm2/parser/wasm/standard/wasm1/opcode/impl.h>
# include <uwvm2/parser/wasm/binfmt/binfmt_ver1/impl.h>
# include "def.h"
# include "types.h"
# include "feature_def.h"
# include "type_section.h"
#endif

#ifndef UWVM_MODULE_EXPORT
# define UWVM_MODULE_EXPORT
#endif

UWVM_MODULE_EXPORT namespace uwvm2::parser::wasm::standard::wasm1::features
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct function_section_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        inline static constexpr ::fast_io::u8string_view section_name{u8"Function"};
        inline static constexpr ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte section_id{
            static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(::uwvm2::parser::wasm::standard::wasm1::section::section_id::function_sec)};

        ::uwvm2::parser::wasm::standard::wasm1::section::section_span_view sec_span{};

        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::features::final_local_function_type<Fs...>> funcs{};
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_sequence_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }

#if UINT_LEAST32_MAX < SIZE_MAX && __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                              \
    ((defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)) || (defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)) ||            \
     (defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)) ||                                                                   \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)) || (defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)))

    struct simd128_shuffle_table_t
    {
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;

        u8x16simd shuffle_mask{};   // shuffle mask u8x32 c-> u32x8
        unsigned processed_simd{};  // How many u32 typeidx can be output 0 == error
        unsigned processed_byte{};  // How many bytes are scanned? 0 == error
    };

    inline consteval ::fast_io::array<simd128_shuffle_table_t, 16uz> generate_simd128_shuffle_table() noexcept
    {
        using u8x16simd [[__gnu__::__vector_size__(16)]] = ::std::uint8_t;

        constexpr auto u8_m1{static_cast<::std::uint8_t>(0x80u)};

        constexpr u8x16simd can_mask_zero{u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1, u8_m1};

        ::fast_io::array<simd128_shuffle_table_t, 16uz> ret{};

        for(unsigned i{}; i != 16u; ++i)
        {
            ret[i].shuffle_mask = can_mask_zero;

            // Here it is processed up to 2^28, the rest is almost impossible to meet and is handled using the default processing methods
            unsigned bytes_remaining{4u};
            auto byte{i};
            unsigned simd_counter{};

            bool error{};

            unsigned j_counter{};

            while(bytes_remaining)
            {
                auto const ctro{static_cast<unsigned>(::std::countr_one(byte))};
                if(ctro > 3u)
                {
                    // ??'1111'0000 > 2^28u
                    error = true;
                    break;
                }
                else if(ctro == bytes_remaining)
                {
                    // ??'1110'0000 cannot handle
                    break;
                }

                auto const shr{ctro + 1u};

                for(unsigned j{}; j != shr; ++j)
                {
                    ret[i].shuffle_mask[simd_counter * 4u + j] = static_cast<::std::uint8_t>(j_counter++);
                }

                bytes_remaining -= shr;
                byte >>= shr;
                ++simd_counter;
            }

            if(error)
            {
                ret[i].shuffle_mask = can_mask_zero;
                ret[i].processed_simd = 0u;
                ret[i].processed_byte = 0u;
            }
            else
            {
                ret[i].processed_simd = simd_counter;
                ret[i].processed_byte = 4u - bytes_remaining;
            }
        }

        return ret;
    }

    /// @brief      simd 128 shuffle table
    /// @details    mask 32 == zero
    inline constexpr ::fast_io::array<simd128_shuffle_table_t, 16z> simd128_shuffle_table{generate_simd128_shuffle_table()};

    /// @brief      Accelerated parsing of pure 1-byte typeidx via simd128
    /// @details    Support:
    ///
    ///             (Little Endian), sizeof(::std::size_t) == 8uz (UINT_LEAST32_MAX < SIZE_MAX), [[gnu::vector_size]]
    ///             x86_64-sse2, aarch64-neon, loongarch-SX, wasm-wasmsimd128
    ///
    ///             Due to wasm compact text and after actual testing, the number of functions to function type ratio is 50:1, and in most cases the typeidx of
    ///             commonly used types will be small, with 10,000 functions, almost 90% of the typeidx is single byte. So we only do optimization for
    ///             single-byte simd, not for multi-byte simd, multi-byte relies on shuffle and need to check the table, for almost all single-byte performance
    ///             may not be as good as sequential parsing.
    ///
    ///             Cyclic read 16 bytes, encountered greater than 127 through the mask and crtz calculations are handed over to the rest of the 16 bytes in
    ///             order to deal with the contents of the 16 bytes, and so on the start of the next cycle and then continue to read 16 bytes. There is no mask
    ///             when encountered can not handle the situation can not be located through the mask, only from the beginning of the 16 re-read in order.
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_1b_simd128_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

# if 0 
        /// @deprecated Tested to appear multi-byte situations, simd128 has no advantage over regular

        if(type_section_count >= 128uz)
        {
            using u8x16simd [[__gnu__::__vector_size__(16)]] = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte;
            using u8x16arr = ::fast_io::array<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, 16uz>;

            // Presence of multibytes
            // on SSE2, usd __builtin_ia32_pmovmskb128, direct mask processing of the first bit of each byte

            while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
            {
                u8x16simd simd_vector_str;  // No initialization necessary

                ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

                // mask processing of the first bit of each byte
                using c8x16simd [[__gnu__::__vector_size__(16)]] = char;
                auto const upper_mask{static_cast<::std::uint16_t>(__builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(simd_vector_str)))};

                if(
#  if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                    __builtin_expect_with_probability(upper_mask == 0u, true, 0.9)
#  else
                    upper_mask == 0u
#  endif
                        )
#  if !(UWVM_HAS_BUILTIN(__builtin_expect_with_probability))
                    [[likely]]
#  endif
                {
                    // The first byte of each byte is 0, so it is straightforward to write

                    func_counter += 16u;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    auto const arr{::std::bit_cast<u8x16arr>(simd_vector_str)};
                    for(auto const i: arr) { functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + i); }

                    // set curr
                    section_curr += 16u;
                }
                else
                {
                    auto const section_curr_end{section_curr + 16u};

                    auto const byte_need_reed{static_cast<unsigned>(::std::countr_zero(upper_mask))};

#  if __has_cpp_attribute(assume)
                    [[assume(byte_need_reed < 16u)]];
#  endif

                    func_counter += byte_need_reed;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    auto const arr{::std::bit_cast<u8x16arr>(simd_vector_str)};
                    auto const arr_write_end{arr.cbegin() + byte_need_reed};

                    for(auto ic{arr.cbegin()}; ic != arr_write_end; ++ic) { functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + *ic); }

                    section_curr += byte_need_reed;

                    while(section_curr < section_curr_end)
                    {
                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

                        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

                        auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                        ::fast_io::mnp::leb128_get(typeidx))};

                        if(typeidx_err != ::fast_io::parse_code::ok)
                        {
                            err.err_curr = section_curr;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
                        }

                        // Ensure data is not end of file to inc one
                        if(++func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        if(typeidx >= type_section_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = typeidx;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

                        section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);
                    }
                }
            }
        }
# endif

        if(type_section_count < 128uz)
        {
            // No multibyte cases
            // on SSE2, judge first then __builtin_ia32_pmovmskb128 to figure out if there are any zeros
            // on SSE4, use __builtin_ia32_ptestz128, optimized for all-0 judgments

            using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
            using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
            using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
            using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
            using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
            using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

            static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
                static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

            while(static_cast<::std::size_t>(section_end - section_curr) >= 16uz) [[likely]]
            {
                // [before_section ... | func_count ... typeidx1 ... (15) ...] ...
                // [                        safe                             ] unsafe (could be the section_end)
                //                                      ^^ section_curr
                //                                      [   simd_vector_str  ]

                u8x16simd simd_vector_str;  // No initialization necessary

                ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x16simd));

                // It's already a little-endian.

                auto const check_upper{simd_vector_str >= simd_vector_check};

                if(
# if defined(__SSE4_1__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz128)
                    !__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(check_upper), ::std::bit_cast<i64x2simd>(check_upper))
# elif defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
                    __builtin_ia32_pmovmskb128(::std::bit_cast<c8x16simd>(check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_all_true_i8x16)
                    !__builtin_wasm_all_true_i8x16(::std::bit_cast<i8x16simd>(~check_upper))
# elif defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)
                    __builtin_wasm_bitmask_i8x16(::std::bit_cast<i8x16simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)                  // Only supported by clang
                    __builtin_neon_vmaxvq_u32(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)  // Only supported by GCC
                    __builtin_aarch64_reduc_umax_scal_v4si_uu(::std::bit_cast<u32x4simd>(check_upper))
# elif defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)
                    __builtin_lsx_bnz_v(::std::bit_cast<u8x16simd>(check_upper))
# endif
                        ) [[unlikely]]
                {
                    auto const section_curr_end{section_curr + 16u};

                    // Since parse_by_scan uses section_end, it is possible that section_curr will be greater than section_curr_end, use '<' instead of '!='

                    while(section_curr < section_curr_end)
                    {
                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1] ... (outer) ] typeidx2 ...
                        // [     safe    ] ... (outer) ] unsafe (could be the section_end)
                        //       ^^ section_curr

                        if(++func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

                        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

                        auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                        ::fast_io::mnp::leb128_get(typeidx))};

                        if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
                        }

                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1 ...] ... (outer) ] typeidx2 ...
                        // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                        //       ^^ section_curr

                        // There's a good chance there's an error here.
                        // Or there is a leb redundancy 0
                        if(typeidx >= type_section_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = typeidx;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1 ...] typeidx2 ...] ...
                        // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                        //                     ^^ section_curr
                    }

                    // [before_section ... | func_count ... typeidx1 ... (15) ... ...  ] typeidxN
                    // [                        safe                                   ] unsafe (could be the section_end)
                    //                                                                   ^^ section_curr
                    //                                      [   simd_vector_str  ] ... ] (Depends on the size of section_curr in relation to section_curr_end)
                }
                else
                {
                    // all are single bytes, so there are 16
                    func_counter += 16u;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
# if UWVM_HAS_BUILTIN(__builtin_shufflevector) && (defined(__SSSE3__) || defined(__wasm_simd128__) || defined(__ARM_NEON) || defined(__loongarch_sx))

                    constexpr u8x16simd all_zero{};
                    auto const typesec_types_cbegin_u64{::std::bit_cast<::std::uint64_t>(typesec_types_cbegin)};

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 0, 16, 16, 16, 16, 16, 16, 16, 1, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 2, 16, 16, 16, 16, 16, 16, 16, 3, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 4, 16, 16, 16, 16, 16, 16, 16, 5, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 6, 16, 16, 16, 16, 16, 16, 16, 7, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 8, 16, 16, 16, 16, 16, 16, 16, 9, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 10, 16, 16, 16, 16, 16, 16, 16, 11, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 12, 16, 16, 16, 16, 16, 16, 16, 13, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

                    {
                        auto v0_u64x2{::std::bit_cast<u64x2simd>(
                            __builtin_shufflevector(simd_vector_str, all_zero, 14, 16, 16, 16, 16, 16, 16, 16, 15, 16, 16, 16, 16, 16, 16, 16))};
                        v0_u64x2 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                    }

# else  // defined(__SSE2__) no shuffle

                    // SSE2:
                    // movd    eax, xmm0
                    // movaps  XMMWORD PTR [rsp-24], xmm0
                    // movzx   eax, al
                    // movq    xmm0, rax
                    // movzx   eax, BYTE PTR [rsp-23]
                    // movq    xmm1, rax
                    // punpcklqdq      xmm0, xmm1
                    //
                    // SSSE3:
                    // pxor    xmm1, xmm1
                    // punpcklbw       xmm0, xmm1
                    // pshufb  xmm0, XMMWORD PTR .LC0[rip]

                    for(unsigned i{}; i != 16u; ++i) { functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + simd_vector_str[i]); }
# endif

                    section_curr += 16uz;

                    // [before_section ... | func_count ... typeidx1 ... (15) ...] typeidx16
                    // [                        safe                             ] unsafe (could be the section_end)
                    //                                                             ^^ section_curr
                }
            }
        }

        // Conventional methods
        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }
#endif

#if UINT_LEAST32_MAX < SIZE_MAX && __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) && \
    ((defined(__AVX__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz256)) || (defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xbnz_v)))

    /// @brief      Accelerated parsing of pure 1-byte typeidx via simd256
    /// @details    Support:
    ///
    ///             (Little Endian), sizeof(::std::size_t) == 8uz (UINT_LEAST32_MAX < SIZE_MAX), [[gnu::vector_size]]
    ///             mask: x86_64-avx, loongarch-ASX
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_1b_simd256_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        if(type_section_count < 128uz)
        {
            /// @details    Due to the compactness of the wasm text, the ratio of the number of functions to function types was tested to be 50:1 in practice,
            ///             and the typeidx of commonly used types is small in most cases, with almost 90% of the 10,000 functions having a single-byte typeidx.
            ///             So we optimize for single-byte simd.

            // No multibyte cases

            using i64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int64_t;
            using u64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint64_t;
            using u32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint32_t;
            using c8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = char;
            using u8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint8_t;
            using i8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int8_t;

            static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const simd_vector_check{
                static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte>(type_section_count)};

            while(static_cast<::std::size_t>(section_end - section_curr) >= 32uz) [[likely]]
            {
                // [before_section ... | func_count ... typeidx1 ... (31) ...] ...
                // [                        safe                             ] unsafe (could be the section_end)
                //                                      ^^ section_curr
                //                                      [   simd_vector_str  ]

                u8x32simd simd_vector_str;  // No initialization necessary

                ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x32simd));

                // It's already a little-endian.

                auto const check_upper{simd_vector_str >= simd_vector_check};

                if(
# if defined(__AVX__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz256)
                    !__builtin_ia32_ptestz256(::std::bit_cast<i64x4simd>(check_upper), ::std::bit_cast<i64x4simd>(check_upper))
# elif defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xbnz_v)
                    __builtin_lasx_xbnz_v(::std::bit_cast<u8x32simd>(check_upper))
# endif
                        ) [[unlikely]]
                {
                    auto const section_curr_end{section_curr + 32u};

                    // Since parse_by_scan uses section_end, it is possible that section_curr will be greater than section_curr_end, use '<' instead of '!='

                    while(section_curr < section_curr_end)
                    {
                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1] ... (outer) ] typeidx2 ...
                        // [     safe    ] ... (outer) ] unsafe (could be the section_end)
                        //       ^^ section_curr

                        if(++func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

                        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

                        auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                                        reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                                        ::fast_io::mnp::leb128_get(typeidx))};

                        if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
                        }

                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1 ...] ... (outer) ] typeidx2 ...
                        // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                        //       ^^ section_curr

                        // There's a good chance there's an error here.
                        // Or there is a leb redundancy 0
                        if(typeidx >= type_section_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = typeidx;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

                        // The outer boundary is unknown and needs to be rechecked
                        // [ ... typeidx1 ...] typeidx2 ...] ...
                        // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                        //                     ^^ section_curr
                    }

                    // [before_section ... | func_count ... typeidx1 ... (31) ... ...  ] typeidxN
                    // [                        safe                                   ] unsafe (could be the section_end)
                    //                                                                   ^^ section_curr
                    //                                      [   simd_vector_str  ] ... ] (Depends on the size of section_curr in relation to section_curr_end)
                }
                else
                {
                    // all are single bytes, so there are 32
                    func_counter += 32u;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    constexpr u8x32simd all_zero{};
                    auto const typesec_types_cbegin_u64{::std::bit_cast<::std::uint64_t>(typesec_types_cbegin)};

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         0,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         1,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         2,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         3,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         4,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         5,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         6,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         7,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         8,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         9,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         10,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         11,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         12,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         13,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         14,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         15,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         16,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         17,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         18,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         19,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         20,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         21,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         22,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         23,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         24,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         25,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         26,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         27,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         28,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         29,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         30,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         31,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    section_curr += 32uz;

                    // [before_section ... | func_count ... typeidx1 ... (31) ...] typeidx32
                    // [                        safe                             ] unsafe (could be the section_end)
                    //                                                             ^^ section_curr
                }
            }
        }
        else
        {
            /// @details    In the few cases where the maximum length of typeidx encountered exceeds 127, it is necessary to implement multi-byte processing,
            ///             here the simd platform that supports shuffle is used to check the 8-bit table checking, processing 4 times, as much as possible to
            ///             process
# if (defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb256) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb256)) ||                                      \
     (defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xvmskltz_b))

            using i64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int64_t;
            using u64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint64_t;
            using u32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint32_t;
            using c8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = char;
            using u8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint8_t;
            using i8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int8_t;

            static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

            ::std::uint32_t const simd_vector_check{static_cast<::std::uint32_t>(type_section_count)};

            while(static_cast<::std::size_t>(section_end - section_curr) >= 32uz) [[likely]]
            {
                // [before_section ... | func_count ... typeidx1 ... (31) ...] ...
                // [                        safe                             ] unsafe (could be the section_end)
                //                                      ^^ section_curr
                //                                      [   simd_vector_str  ]

                u8x32simd simd_vector_str;  // No initialization necessary

                ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x32simd));

                // It's already a little-endian.

                ::std::uint32_t const test_has_non_zero_mask{
#  if defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb256)
                    static_cast<::std::uint32_t>(__builtin_ia32_pmovmskb256(::std::bit_cast<c8x32simd>(simd_vector_str)))
#  elif defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xvmskltz_b)
                    static_cast<::std::uint32_t>(::std::bit_cast<u32x8simd>(__builtin_lasx_xvmskltz_b(::std::bit_cast<i8x32simd>(simd_vector_str)))[0])
#  endif
                };

                if(
#  if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                    __builtin_expect_with_probability(test_has_non_zero_mask, true, 0.8)
#  else
                    test_has_non_zero_mask
#  endif
                )
                {
                    using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
                    using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
                    using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
                    using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
                    using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
                    using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

                    constexpr u32x4simd all_zero{};

                    // Direct calculation of 4 masks
                    auto test_has_non_zero_mask_can_shu{static_cast<::std::uint16_t>(test_has_non_zero_mask)};
                    auto const typesec_types_cbegin_u64{::std::bit_cast<::std::uint64_t>(typesec_types_cbegin)};

                    ::std::uint8_t process_simd_counter{};

                    for (unsigned i{}; i != 4u; ++i)
                    {
                        auto const& m{simd128_shuffle_table.index_unchecked(test_has_non_zero_mask_can_shu & 0x0Fu)};
                        if(!m.processed_simd) [[unlikely]]
                        {
                            // Greater than 2^28 cannot be calculated. Calculated using common methods
                            break;
                        }

                        func_counter += m.processed_simd;
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // calculate
#  if defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb256)

                        auto const simd_vector_str_u8x16x2{::std::bit_cast<::fast_io::array<u8x16simd, 2>>(simd_vector_str)};
                        auto const simd_vector_str_u8x16x2_v0{simd_vector_str_u8x16x2.index_unchecked(0)};
                        auto const shres_u32x4{::std::bit_cast<u32x4simd>(__builtin_ia32_pshufb128(simd_vector_str_u8x16x2_v0, m.shuffle_mask + process_simd_counter))};
                        process_simd_counter += m.processed_byte;
                        auto const typeidx_u32x4{(shres_u32x4 & static_cast<::std::uint32_t>(0x7Fu)) |
                                                 ((shres_u32x4 & static_cast<::std::uint32_t>(0x7F00u)) >> 1u) |
                                                 ((shres_u32x4 & static_cast<::std::uint32_t>(0x7F0000u)) >> 2u) |
                                                 ((shres_u32x4 & static_cast<::std::uint32_t>(0x7F000000u)) >> 3u)};
                        auto const test_u32x4_match{typeidx_u32x4 >= simd_vector_check};
                        if(!__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(test_u32x4_match), ::std::bit_cast<i64x2simd>(test_u32x4_match))) [[unlikely]]
                        {
                            // Greater than typeidx is invalid
                            break;
                        }

#  elif defined(__loongarch_asx)
#   error "todo"
                        /// @todo
#  endif

                        switch(m.processed_simd)
                        {
                            case 1:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, -1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                                break;
                            }
                            case 2:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, 1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));

                                break;
                            }
                            case 3:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, 1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;
                                auto v1_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 2, 4, -1, 4))};
                                v1_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v1_u64x2[0]));

                                break;
                            }
                            case 4:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, 1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;
                                auto v1_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 2, 4, 3, 4))};
                                v1_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v1_u64x2[0]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v1_u64x2[1]));

                                break;
                            }
                            default:
                            {
                                ::fast_io::unreachable();
                            }
                        }

                        section_curr += m.processed_byte;
                        test_has_non_zero_mask_can_shu >>= m.processed_byte;
                    }

                    process_simd_counter = 0u;

                    for (unsigned i{}; i != 4u; ++i)
                    {
                        auto const& m{simd128_shuffle_table.index_unchecked(test_has_non_zero_mask_can_shu & 0x0Fu)};
                        if(!m.processed_simd) [[unlikely]]
                        {
                            // Greater than 2^28 cannot be calculated. Calculated using common methods
                            break;
                        }

                        func_counter += m.processed_simd;
                        if(func_counter > func_count) [[unlikely]]
                        {
                            err.err_curr = section_curr;
                            err.err_selectable.u32 = func_count;
                            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                        }

                        // calculate
#  if defined(__AVX2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pshufb256)

                        auto const simd_vector_str_u8x16x2{::std::bit_cast<::fast_io::array<u8x16simd, 2>>(simd_vector_str)};
                        auto const simd_vector_str_u8x16x2_v0{simd_vector_str_u8x16x2.index_unchecked(1)};
                        auto const shres_u32x4{::std::bit_cast<u32x4simd>(__builtin_ia32_pshufb128(simd_vector_str_u8x16x2_v0, m.shuffle_mask + process_simd_counter))};
                        process_simd_counter += m.processed_byte;
                        auto const typeidx_u32x4{(shres_u32x4 & static_cast<::std::uint32_t>(0x7Fu)) |
                                                 ((shres_u32x4 & static_cast<::std::uint32_t>(0x7F00u)) >> 1u) |
                                                 ((shres_u32x4 & static_cast<::std::uint32_t>(0x7F0000u)) >> 2u) |
                                                 ((shres_u32x4 & static_cast<::std::uint32_t>(0x7F000000u)) >> 3u)};
                        auto const test_u32x4_match{typeidx_u32x4 >= simd_vector_check};
                        if(!__builtin_ia32_ptestz128(::std::bit_cast<i64x2simd>(test_u32x4_match), ::std::bit_cast<i64x2simd>(test_u32x4_match))) [[unlikely]]
                        {
                            // Greater than typeidx is invalid
                            break;
                        }

#  elif defined(__loongarch_asx)
#   error "todo"
                        /// @todo
#  endif

                        switch(m.processed_simd)
                        {
                            case 1:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, -1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                                break;
                            }
                            case 2:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, 1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));

                                break;
                            }
                            case 3:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, 1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;
                                auto v1_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 2, 4, -1, 4))};
                                v1_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v1_u64x2[0]));

                                break;
                            }
                            case 4:
                            {
                                auto v0_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 0, 4, 1, 4))};
                                v0_u64x2 += typesec_types_cbegin_u64;
                                auto v1_u64x2{::std::bit_cast<u64x2simd>(__builtin_shufflevector(typeidx_u32x4, all_zero, 2, 4, 3, 4))};
                                v1_u64x2 += typesec_types_cbegin_u64;

                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[0]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x2[1]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v1_u64x2[0]));
                                functionsec.funcs.emplace_back_unchecked(
                                    reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v1_u64x2[1]));

                                break;
                            }
                            default:
                            {
                                ::fast_io::unreachable();
                            }
                        }

                        section_curr += m.processed_byte;
                        test_has_non_zero_mask_can_shu >>= m.processed_byte;
                    }

                }
                else
                {
                    // all are single bytes, so there are 32
                    func_counter += 32u;

                    // check counter
                    if(func_counter > func_count) [[unlikely]]
                    {
                        err.err_curr = section_curr;
                        err.err_selectable.u32 = func_count;
                        err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                        ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                    }

                    // write data
                    constexpr u8x32simd all_zero{};
                    auto const typesec_types_cbegin_u64{::std::bit_cast<::std::uint64_t>(typesec_types_cbegin)};

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         0,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         1,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         2,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         3,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         4,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         5,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         6,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         7,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         8,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         9,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         10,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         11,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         12,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         13,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         14,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         15,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         16,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         17,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         18,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         19,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         20,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         21,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         22,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         23,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         24,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         25,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         26,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         27,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    {
                        auto v0_u64x4{::std::bit_cast<u64x4simd>(__builtin_shufflevector(simd_vector_str,
                                                                                         all_zero,
                                                                                         28,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         29,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         30,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         31,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32,
                                                                                         32))};
                        v0_u64x4 += typesec_types_cbegin_u64;
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[0]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[1]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[2]));
                        functionsec.funcs.emplace_back_unchecked(
                            reinterpret_cast<::uwvm2::parser::wasm::standard::wasm1::features::final_function_type<Fs...> const*>(v0_u64x4[3]));
                    }

                    section_curr += 32uz;

                    // [before_section ... | func_count ... typeidx1 ... (31) ...] typeidx32
                    // [                        safe                             ] unsafe (could be the section_end)
                    //                                                             ^^ section_curr
                }
            }
# endif
        }

        // Conventional methods
        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }
#endif

#if __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && (defined(__AVX2__) || defined(__ARM_FEATURE_SVE) || defined(__loongarch_asx))
    /// @brief      Accelerated parsing of pure 1-byte typeidx via simd512
    /// @details    Support:
    ///
    ///             (Little Endian)
    ///             mask: x86_64-avx512vbmi
    ///
    ///             Due to wasm compact text and after actual testing, the number of functions to function type ratio is 50:1, and in most cases the typeidx of
    ///             commonly used types will be small, with 10,000 functions, almost 90% of the typeidx is single byte. So we only do optimization for
    ///             single-byte simd, not for multi-byte simd, multi-byte relies on shuffle and need to check the table, for almost all single-byte performance
    ///             may not be as good as sequential parsing.
    ///
    ///             Cyclic read 64 bytes, encountered greater than 127 is calculated by mask and crtz handed over to the order of 32 bytes of the rest of the
    ///             content. The content will be divided into two 32 bytes, the first 32 bytes of the error cycle of the first 32 bytes of the contents of the
    ///             cycle and then to the last 32 bytes to restart the cycle ahead of the cycle to read 64. 32 bytes after the cycle will wait for the next
    ///             cycle to automatically read 32. to avoid the mixing of xmm, ymm, zmm registers, this is not divided into four 16 bytes and then continue to
    ///             calculate the sse instructions
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr ::std::byte const* scan_function_section_1b_simd512_impl(
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* section_curr,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        [[maybe_unused]] ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32& func_counter,
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 const func_count) UWVM_THROWS
    {
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};
        ::std::size_t const type_section_count{typesec.types.size()};
        auto const typesec_types_cbegin{typesec.types.cbegin()};

        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // [before_section ... | func_count ...] typeidx1 ... typeidx2 ...
        // [              safe                 ] unsafe (could be the section_end)
        //                                       ^^ section_curr

        while(static_cast<::std::size_t>(section_end - section_curr) < 64uz) [[likely]]
        {
            /// @todo
            ::fast_io::fast_terminate();
        }

        // Conventional methods
        while(section_curr != section_end) [[likely]]
        {
            // Ensuring the existence of valid information

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check function counter
            // Ensure content is available before counting (section_curr != section_end)
            if(++func_counter > func_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = func_count;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            // [ ... typeidx1] ... typeidx2 ...
            // [     safe    ] unsafe (could be the section_end)
            //       ^^ section_curr

            ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx{};

            using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

            auto const [typeidx_next, typeidx_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                            reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                            ::fast_io::mnp::leb128_get(typeidx))};

            if(typeidx_err != ::fast_io::parse_code::ok) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(typeidx_err);
            }

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //       ^^ section_curr

            // check: type_index should less than type_section_count
            if(typeidx >= type_section_count) [[unlikely]]
            {
                err.err_curr = section_curr;
                err.err_selectable.u32 = typeidx;
                err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::illegal_type_index;
                ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
            }

            functionsec.funcs.emplace_back_unchecked(typesec_types_cbegin + typeidx);

            section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

            // [ ... typeidx1 ...] typeidx2 ...
            // [      safe       ] unsafe (could be the section_end)
            //                     ^^ section_curr
        }

        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        return section_curr;
    }
#endif

    /// @brief Define the handler function for type_section
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    inline constexpr void handle_binfmt_ver1_extensible_section_define(
        ::uwvm2::parser::wasm::concepts::feature_reserve_type_t<function_section_storage_t<Fs...>> sec_adl,
        ::uwvm2::parser::wasm::binfmt::ver1::wasm_binfmt_ver1_module_extensible_storage_t<Fs...> & module_storage,
        ::std::byte const* const section_begin,
        ::std::byte const* const section_end,
        ::uwvm2::parser::wasm::base::error_impl& err,
        ::uwvm2::parser::wasm::concepts::feature_parameter_t<Fs...> const& fs_para,
        ::std::byte const* const sec_id_module_ptr) UWVM_THROWS
    {
#ifdef UWVM_TIMER
        ::uwvm2::utils::debug::timer parsing_timer{u8"parse function section (id: 3)"};
#endif
        // Note that section_begin may be equal to section_end
        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

        // get function_section_storage_t from storages
        auto& functionsec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<function_section_storage_t<Fs...>>(module_storage.sections)};

        // check duplicate
        if(functionsec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8 = functionsec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::duplicate_section;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        // check has typesec
        auto const& typesec{::uwvm2::parser::wasm::concepts::operation::get_first_type_in_tuple<type_section_storage_t<Fs...>>(module_storage.sections)};

        if(!typesec.sec_span.sec_begin) [[unlikely]]
        {
            err.err_curr = sec_id_module_ptr;
            err.err_selectable.u8arr[0] = typesec.section_id;
            err.err_selectable.u8arr[1] = functionsec.section_id;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::forward_dependency_missing;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }

        using wasm_byte_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = ::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte const*;

        functionsec.sec_span.sec_begin = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_begin);
        functionsec.sec_span.sec_end = reinterpret_cast<wasm_byte_const_may_alias_ptr>(section_end);

        auto section_curr{section_begin};

        // [before_section ... ] | func_count typeidx1 ...
        // [        safe       ] | unsafe (could be the section_end)
        //                         ^^ section_curr

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_count{};

        using char8_t_const_may_alias_ptr UWVM_GNU_MAY_ALIAS = char8_t const*;

        // No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)
        auto const [func_count_next, func_count_err]{::fast_io::parse_by_scan(reinterpret_cast<char8_t_const_may_alias_ptr>(section_curr),
                                                                              reinterpret_cast<char8_t_const_may_alias_ptr>(section_end),
                                                                              ::fast_io::mnp::leb128_get(func_count))};

        if(func_count_err != ::fast_io::parse_code::ok) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::invalid_func_count;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(func_count_err);
        }

        // [before_section ... | func_count ...] typeidx1 ...
        // [             safe                  ] unsafe (could be the section_end)
        //                       ^^ section_curr

        functionsec.funcs.reserve(func_count);

        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 func_counter{};  // use for check

        section_curr = reinterpret_cast<::std::byte const*>(func_count_next);  // never out of bounds
// No explicit checking required because ::fast_io::parse_by_scan self-checking (::fast_io::parse_code::end_of_file)

// [before_section ... | func_count ...] typeidx1 ...
// [              safe                 ] unsafe (could be the section_end)
//                                       ^^ section_curr

// handle it
#if UINT_LEAST32_MAX < SIZE_MAX && __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) && \
    ((defined(__AVX__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestz256)) || (defined(__loongarch_asx) && UWVM_HAS_BUILTIN(__builtin_lasx_xbnz_v)))
        section_curr = scan_function_section_1b_simd256_impl(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
#elif UINT_LEAST32_MAX < SIZE_MAX && __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) &&                                            \
    ((defined(__SSE2__) && UWVM_HAS_BUILTIN(__builtin_ia32_pmovmskb128)) || (defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_neon_vmaxvq_u32)) ||            \
     (defined(__ARM_NEON) && UWVM_HAS_BUILTIN(__builtin_aarch64_reduc_umax_scal_v4si_uu)) ||                                                                   \
     (defined(__loongarch_sx) && UWVM_HAS_BUILTIN(__builtin_lsx_bnz_v)) || (defined(__wasm_simd128__) && UWVM_HAS_BUILTIN(__builtin_wasm_bitmask_i8x16)))
        section_curr = scan_function_section_1b_simd128_impl(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
#else
        section_curr = scan_function_section_sequence_impl(sec_adl, module_storage, section_curr, section_end, err, fs_para, func_counter, func_count);
#endif
        // [before_section ... | func_count ... typeidx1 ... ...] (end)
        // [                       safe                         ] unsafe (could be the section_end)
        //                                                        ^^ section_curr

        // check function counter match
        if(func_counter != func_count) [[unlikely]]
        {
            err.err_curr = section_curr;
            err.err_selectable.u32arr[0] = func_counter;
            err.err_selectable.u32arr[1] = func_count;
            err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_not_match_the_actual_number;
            ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
        }
    }
}  // namespace uwvm2::parser::wasm::standard::wasm1::features

/// @brief Define container optimization operations for use with fast_io
UWVM_MODULE_EXPORT namespace fast_io::freestanding
{
    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_trivially_copyable_or_relocatable<::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };

    template <::uwvm2::parser::wasm::concepts::wasm_feature... Fs>
    struct is_zero_default_constructible<::uwvm2::parser::wasm::standard::wasm1::features::function_section_storage_t<Fs...>>
    {
        inline static constexpr bool value = true;
    };
}

#ifndef UWVM_MODULE
// macro
# include <uwvm2/utils/macro/pop_macros.h>
#endif
