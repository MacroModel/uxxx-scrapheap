/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the ASHP-1.0 License (see LICENSE file).   *
 *************************************************************/

/**
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-30
 * @copyright   ASHP-1.0 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

// std
#include <cstddef>
#include <memory>
#include <utility>
// macro
#include <uwvm2/utils/macro/push_macros.h>
#include <uwvm2/uwvm/utils/ansies/uwvm_color_push_macro.h>
// import
#ifdef UWVM_MODULE
import fast_io;
import uwvm2.utils.ansies;
import uwvm2.utils.cmdline;
import uwvm2.utils.console;
import uwvm2.uwvm.io;
import uwvm2.uwvm.utils.ansies;
import uwvm2.uwvm.cmdline;
#else
# include <fast_io.h>
# include <fast_io_dsal/array.h>
# include <fast_io_dsal/vector.h>
# include <uwvm2/utils/ansies/impl.h>
# include <uwvm2/utils/cmdline/impl.h>
# include <uwvm2/utils/console/impl.h>
# include <uwvm2/uwvm/io/impl.h>
# include <uwvm2/uwvm/utils/ansies/impl.h>
# include <uwvm2/uwvm/cmdline/impl.h>
#endif

namespace uwvm2::uwvm::cmdline::params::details
{
    template <::std::size_t N>
    inline constexpr ::fast_io::array<char8_t, N + 1> u8nspace() noexcept
    {
        ::fast_io::array<char8_t, N + 1> res{};

        for(::std::size_t i{}; i != N; ++i) { res[i] = u8' '; }

        res[N] = u8'\0';

        return res;
    }

    // max parameter + left "  " + right "  "
    inline constexpr auto parameter_max_principal_name_size_u8nspace{u8nspace<::uwvm2::uwvm::cmdline::parameter_max_principal_name_size + 4uz>()};
    inline constexpr auto description_size_u8nspace{u8nspace<::uwvm2::uwvm::cmdline::parameter_max_principal_name_size + 4uz + 7uz>()};

    namespace details
    {
        inline constexpr bool is_special_char(char8_t cr) noexcept
        {
            switch (cr)
            {
                case u8'|': [[fallthrough]];
                case u8'<': [[fallthrough]];
                case u8'>': [[fallthrough]];
                case u8'{': [[fallthrough]];
                case u8'}': [[fallthrough]];
                case u8'[': [[fallthrough]];
                case u8']': [[fallthrough]];
                case u8'(': [[fallthrough]];
                case u8')': [[fallthrough]];
                case u8'*': [[fallthrough]];
                case u8'&': [[fallthrough]];
                case u8'^': [[fallthrough]];
                case u8'%': [[fallthrough]];
                case u8'$': [[fallthrough]];
                case u8'#': [[fallthrough]];
                case u8'@': [[fallthrough]];
                case u8'!': [[fallthrough]];
                case u8'~': [[fallthrough]];
                case u8'`': [[fallthrough]];
                case u8'\'': [[fallthrough]];
                case u8';': [[fallthrough]];
                case u8':': [[fallthrough]];
                case u8'\"': [[fallthrough]];
                case u8',': [[fallthrough]];
                case u8'.': [[fallthrough]];
                case u8'?': [[fallthrough]];
                case u8'/': [[fallthrough]];
                case u8'\\': 
                    return true;
                default:
                    return false;
            }
        }
        
        enum class description_type_t
        {
            description,
            usage,
            argu
        };

        struct description_printer
        {
            ::fast_io::u8string_view description{};
            description_type_t description_type{};
            ::std::size_t first_line_remaining{};
        };

        /// @brief Output, support: char8_t
        template <typename Stm>
        inline constexpr void print_define(::fast_io::io_reserve_type_t<char8_t, description_printer>, Stm && stream, description_printer const& des) noexcept
        {
            auto const console_width{::uwvm2::utils::console::get_console_width()};
            constexpr ::std::size_t maxsz_of_descripten{::uwvm2::uwvm::cmdline::parameter_max_principal_name_size + 4uz + 7uz};

            if (console_width < maxsz_of_descripten + 15uz)
            {
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), des.description);
            }
            else
            {
                auto const line_width{console_width - maxsz_of_descripten};
                bool first_line{true}; // You don't need to add left justification to the first line
                ::fast_io::u8string_view line{};
                ::std::size_t last_special_case{};

                for (auto const* curr_cr{des.description.cbegin()}; curr_cr != des.description.cend(); ++curr_cr)
                {
                    auto curr_line_width{line_width};

                    if (first_line && des.first_line_remaining != 0uz)
                    {
                        curr_line_width = des.first_line_remaining;
                    }

                    auto const line_size{line.size()};
                    if(line_size == 0uz)
                    {
                        if (::fast_io::char_category::is_c_space(*curr_cr))
                        {
                            continue;
                        }
                        else
                        {
                            line.ptr = curr_cr;
                            line.n = 1uz;
                        }
                    }
                    else if (line_size == curr_line_width - 1uz)
                    {                            
                        if (last_special_case)
                        {
                            // print to the previous special case
                            ::fast_io::u8string_view need_print{line.data(), last_special_case};
                            while(need_print.n && ::fast_io::char_category::is_c_space(need_print.ptr[need_print.n - 1uz]))
                            {
                                --need_print.n;
                            }

                            if (first_line)
                            {
                                first_line = false;
                            }
                            else
                            {
                                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), u8"\n", description_size_u8nspace.element);
                            }
                            ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), need_print);
                            line.ptr += last_special_case;
                            line.n -= last_special_case;
                            last_special_case = 0uz;

                            while(line.n && ::fast_io::char_category::is_c_space(*line.ptr))
                            {
                                ++line.ptr;
                                --line.n;
                            }

                            if (line.n == 0uz)
                            {
                                line.ptr = curr_cr;
                                line.n = 1uz;
                            }
                        }
                        else
                        {
                            if (first_line)
                            {
                                first_line = false;
                            }
                            else
                            {
                                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), u8"\n", description_size_u8nspace.element);
                            }
                            ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), line);
                            line.clear();
                        }
                    }
                    else
                    {
                        if (::fast_io::char_category::is_c_space(*curr_cr) || is_special_char(*curr_cr))
                        {
                            last_special_case = line.n;
                        }

                        ++line.n;
                    }
                }

                // As long as n is guaranteed to be 0, ptr can be either a nullptr or a valid value
                            if (first_line)
                            {
                                first_line = false;
                            }
                            else
                            {
                               ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), u8"\n", description_size_u8nspace.element);
                            }
                ::fast_io::operations::print_freestanding<false>(::std::forward<Stm>(stream), line);
            }
        }
    }

    inline constexpr details::description_printer print_description(::fast_io::u8string_view des) noexcept
    {
        return {des};
    }

    template <typename Stm>
    inline void help_output_singal_cate(Stm&& stm, ::uwvm2::utils::cmdline::categorization cate) noexcept
    {
        for(auto const p: ::uwvm2::uwvm::cmdline::parameters)
        {
            if(p->cate != cate) { continue; }
            ::fast_io::io::perr(::std::forward<Stm>(stm),
                                u8"  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                ::fast_io::mnp::left(p->name, ::uwvm2::uwvm::cmdline::parameter_max_principal_name_size));
            ::fast_io::io::perrln(::std::forward<Stm>(stm),
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  u8"  -----  ",
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  print_description(p->describe));
            ::fast_io::io::perr(::std::forward<Stm>(stm),
                                parameter_max_principal_name_size_u8nspace.element,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_PURPLE),
                                u8"Usage: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"[",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                p->name);
            for(auto curr_base{p->alias.base}; curr_base != p->alias.base + p->alias.len; ++curr_base)
            {
                ::fast_io::io::perr(::std::forward<Stm>(stm),
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"|",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                    *curr_base);
            }
            ::fast_io::io::perrln(::std::forward<Stm>(stm),
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  u8"] ",
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  p->usage,
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
        }
    }

    template <typename Stm>
    inline void help_output_all(Stm&& stm) noexcept
    {
        for(auto const p: ::uwvm2::uwvm::cmdline::parameters)
        {
            ::fast_io::io::perr(::std::forward<Stm>(stm),
                                u8"  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                ::fast_io::mnp::left(p->name, ::uwvm2::uwvm::cmdline::parameter_max_principal_name_size));
            ::fast_io::io::perrln(::std::forward<Stm>(stm),
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  u8"  -----  ",
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  print_description(p->describe));
            ::fast_io::io::perr(::std::forward<Stm>(stm),
                                parameter_max_principal_name_size_u8nspace.element,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_PURPLE),
                                u8"Usage: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"[",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                p->name);
            for(auto curr_base{p->alias.base}; curr_base != p->alias.base + p->alias.len; ++curr_base)
            {
                ::fast_io::io::perr(::std::forward<Stm>(stm),
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                    u8"|",
                                    ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                    *curr_base);
            }
            ::fast_io::io::perrln(::std::forward<Stm>(stm),
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                  u8"] ",
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                  p->usage,
                                  ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL));
        }
    }

    UWVM_GNU_COLD extern ::uwvm2::utils::cmdline::parameter_return_type
        help_callback([[maybe_unused]] ::uwvm2::utils::cmdline::parameter_parsing_results* para_begin,
                      ::uwvm2::utils::cmdline::parameter_parsing_results* para_curr,
                      ::uwvm2::utils::cmdline::parameter_parsing_results* para_end) noexcept
    {
        // No copies will be made here.
        auto u8log_output_osr{::fast_io::operations::output_stream_ref(::uwvm2::uwvm::u8log_output)};
        // Add raii locks while unlocking operations
        ::fast_io::operations::decay::stream_ref_decay_lock_guard u8log_output_lg{
            ::fast_io::operations::decay::output_stream_mutex_ref_decay(u8log_output_osr)};
        // No copies will be made here.
        auto u8log_output_ul{::fast_io::operations::decay::output_stream_unlocked_ref_decay(u8log_output_osr)};

        // [... curr] ...
        // [  safe  ] unsafe (could be the module_end)
        //      ^^ para_curr

        auto currp1{para_curr + 1u};

        // [... curr] ...
        // [  safe  ] unsafe (could be the module_end)
        //            ^^ currp1

        // Check for out-of-bounds and not-argument
        if(currp1 == para_end || currp1->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg) [[unlikely]]
        {
            // (currp1 == para_end):
            // [... curr] (end) ...
            // [  safe  ] unsafe (could be the module_end)
            //            ^^ currp1

            // (currp1->type != ::uwvm2::utils::cmdline::parameter_parsing_results_type::arg):
            // [... curr para] ...
            // [     safe    ] unsafe (could be the module_end)
            //           ^^ currp1

            ::fast_io::io::perr(u8log_output_ul,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n");
            help_output_singal_cate(u8log_output_ul, ::uwvm2::utils::cmdline::categorization::global);

            // display other parameter
            ::fast_io::io::perr(u8log_output_ul,
                                // ln
                                u8"\n",
                                // debug
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  ",
                                ::fast_io::mnp::left(u8"<debug>", ::uwvm2::uwvm::cmdline::parameter_max_principal_name_size),
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"  -----  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Use \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"--help debug",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" to display the debug arguments."
                                // endl
                                u8"\n\n",
                                // wasm
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  ",
                                ::fast_io::mnp::left(u8"<wasm>", ::uwvm2::uwvm::cmdline::parameter_max_principal_name_size),
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"  -----  ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Use \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"--help wasm",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\" to display the wasm arguments.",
                                // endl
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");

            return ::uwvm2::utils::cmdline::parameter_return_type::return_imme;
        }

        // [... curr arg1] ...
        // [     safe     ] unsafe (could be the module_end)
        //           ^^ currp1

        currp1->type = ::uwvm2::utils::cmdline::parameter_parsing_results_type::occupied_arg;

        if(auto const currp1_str{currp1->str}; currp1_str == u8"all")
        {
            ::fast_io::io::perr(u8log_output_ul,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n");
            help_output_all(u8log_output_ul);
            ::fast_io::io::perr(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else if(currp1_str == u8"global")
        {
            ::fast_io::io::perr(u8log_output_ul,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  <global>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8":\n");
            help_output_singal_cate(u8log_output_ul, ::uwvm2::utils::cmdline::categorization::global);
            ::fast_io::io::perr(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else if(currp1_str == u8"debug")
        {
            ::fast_io::io::perr(u8log_output_ul,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  <debug>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8":\n");
            help_output_singal_cate(u8log_output_ul, ::uwvm2::utils::cmdline::categorization::debug);
            ::fast_io::io::perr(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else if(currp1_str == u8"wasm")
        {
            ::fast_io::io::perr(u8log_output_ul,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"Arguments:\n",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_LT_CYAN),
                                u8"  <wasm>",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8":\n");
            help_output_singal_cate(u8log_output_ul, ::uwvm2::utils::cmdline::categorization::wasm);
            ::fast_io::io::perr(u8log_output_ul, ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL), u8"\n");
        }
        else
        {
            ::fast_io::io::perr(u8log_output_ul,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL_AND_SET_WHITE),
                                u8"uwvm: ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RED),
                                u8"[error] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"Invalid Extra Help Name \"",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_CYAN),
                                currp1_str,
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"\". Usage: [",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"--help",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"|",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_GREEN),
                                u8"-h",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_WHITE),
                                u8"] ",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_YELLOW),
                                u8"[<null>|all|global|debug|wasm]",
                                ::fast_io::mnp::cond(::uwvm2::uwvm::utils::ansies::put_color, UWVM_COLOR_U8_RST_ALL),
                                u8"\n\n");

            return ::uwvm2::utils::cmdline::parameter_return_type::return_m1_imme;
        }

        return ::uwvm2::utils::cmdline::parameter_return_type::return_imme;
    }

}  // namespace uwvm2::uwvm::cmdline::params::details

// macro
#include <uwvm2/uwvm/utils/ansies/uwvm_color_pop_macro.h>
#include <uwvm2/utils/macro/pop_macros.h>
