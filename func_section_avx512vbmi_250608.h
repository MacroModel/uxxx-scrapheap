#elif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) &&                              \
    ((defined(__AVX512VBMI__) && (UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512_mask) || UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512))) &&                     \
     (defined(__AVX512BW__) && (UWVM_HAS_BUILTIN(__builtin_ia32_ptestmb512) || UWVM_HAS_BUILTIN(__builtin_ia32_cmpb512_mask))))
        /// (Little Endian), [[gnu::vector_size]], has mask-u16, can shuffle, simd512
        /// x86_64-avx512vbmi /// @todo need test

        auto error_handler{[&](::std::size_t n) constexpr UWVM_THROWS -> void
                           {
                               // Need to ensure that section_curr to section_curr + n is memory safe
                               // Processing uses section_curr_end to guarantee entry into the loop for inspection.
                               // Also use section_end to ensure that the scanning of the last leb128 is handled correctly, both memory safe.

                               auto const section_curr_end{section_curr + n};

                               // [ ... typeidx1 ... ] (section_curr_end) ... (outer) ]
                               // [     safe     ... ]                    ... (outer) ] unsafe (could be the section_end)
                               //       ^^ section_curr

                               // Since parse_by_scan uses section_end, it is possible that section_curr will be greater than section_curr_end, use '<' instead
                               // of '!='

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

                                   ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32 typeidx;  // No initialization necessary

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

                                   functionsec.funcs.storage.typeidx_u8_vector.emplace_back_unchecked(
                                       static_cast<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>(typeidx));

                                   section_curr = reinterpret_cast<::std::byte const*>(typeidx_next);

                                   // The outer boundary is unknown and needs to be rechecked
                                   // [ ... typeidx1 ...] typeidx2 ...] ...
                                   // [      safe       ] ... (outer) ] unsafe (could be the section_end)
                                   //                     ^^ section_curr
                               }

                               // [before_section ... | func_count ... typeidx1 ... (n - 1) ... ...  ] typeidxN
                               // [                        safe                                      ] unsafe (could be the section_end)
                               //                                                                      ^^ section_curr
                               //                                      [   simd_vector_str  ]  ...   ] (Depends on the size of section_curr in relation to
                               //                                      section_curr_end)
                           }};

        using i64x8simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int64_t;
        using u64x8simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x15simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x32simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = char;
        using u8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x64simd [[__gnu__::__vector_size__(64)]] [[maybe_unused]] = ::std::int8_t;

        using i64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int64_t;
        using u64x4simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x8simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x16simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = char;
        using u8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x32simd [[__gnu__::__vector_size__(32)]] [[maybe_unused]] = ::std::int8_t;

        using i64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int64_t;
        using u64x2simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint64_t;
        using u32x4simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint32_t;
        using u16x8simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint16_t;
        using c8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = char;
        using u8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::uint8_t;
        using i8x16simd [[__gnu__::__vector_size__(16)]] [[maybe_unused]] = ::std::int8_t;

        static_assert(::std::same_as<::uwvm2::parser::wasm::standard::wasm1::type::wasm_byte, ::std::uint8_t>);

        while(static_cast<::std::size_t>(section_end - section_curr) >= 64uz) [[likely]]
        {

            // [before_section ... | func_count ... typeidx1 ... (63) ...] ...
            // [                        safe                             ] unsafe (could be the section_end)
            //                                      ^^ section_curr
            //                                      [   simd_vector_str  ]

            u8x64simd simd_vector_str;  // No initialization necessary

            ::fast_io::freestanding::my_memcpy(::std::addressof(simd_vector_str), section_curr, sizeof(u8x64simd));

            // It's already a little-endian.

            // When the highest bit of each byte is pop, then mask out 1

            ::std::uint64_t const check_mask{
# if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_cvtb2mask512)
                static_cast<::std::uint64_t>(__builtin_ia32_cvtb2mask512(::std::bit_cast<c8x64simd>(simd_vector_str)))
# else
#  error "missing instructions"
# endif
            };

            if(
# if UWVM_HAS_BUILTIN(__builtin_expect_with_probability)
                __builtin_expect_with_probability(static_cast<bool>(check_mask), true, 1.0 / 4.0)
# else
                check_mask
# endif
            )
            {
                if(func_counter + 64u > func_count) [[unlikely]]
                {
                    // Near the end, jump directly out of the simd section and hand it over to the tail.
                    break;
                }

                ::std::uint16_t const simd_vector_check{static_cast<::std::uint16_t>(type_section_count)};

                ::std::uint64_t check_mask_curr{check_mask};

                ::fast_io::array<u8x32simd, 2uz> need_shu_write_u8x32x2;  // No initialization necessary

                unsigned processed_simd_counter{};
                unsigned processed_byte_counter{};

                // 1st
                if(check_mask_curr & 0xFFFF'FFFFu)
                {
                    bool need_handle_error{};

                    u8x64simd mask_table;  // No initialization necessary

                    for(unsigned i{}; i != 4u; ++i)
                    {
                        unsigned const check_table_index{static_cast<unsigned>(check_mask_curr & 0xFFu)};

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32

                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            need_handle_error = true;
                            break;
                        }

                        bool const first_round_is_seven{static_cast<bool>(check_table_index & 0x80u)};
                        unsigned const first_round{8u - static_cast<unsigned>(first_round_is_seven)};

                        check_mask_curr >>= first_round;

                        ::fast_io::array<u8x16simd, 4uz> const u8x64simd_tmp_u8x16x4{curr_table_shuffle_mask +
                                                                                     static_cast<::std::uint8_t>(processed_byte_counter)};
                        u8x64simd u8x64simd_tmp{::std::bit_cast<u8x64simd>(u8x64simd_tmp_u8x16x4)};

                        switch(processed_simd_counter)
                        {
                            // 1st
                            case 0u:
                            {
                                mask_table = u8x64simd_tmp;
                                break;
                            }
                            case 4u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (8)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (40)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                    -1,
                                                                    -1,
                                                                    -1,
                                                                    -1,
                                                                    -1,
                                                                    -1);
                                break;
                            }
                            case 5u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (8)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (40)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                    -1,
                                                                    -1,
                                                                    -1,
                                                                    -1);
                                break;
                            }
                            case 6u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (8)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (40)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                    -1,
                                                                    -1);
                                break;
                            }
                            case 7u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (14)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (34)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 8u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (16)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (32)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            // 3rd
                            case 14u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (28)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (20)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 15u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (30)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (18)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 16u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (32)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (16)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            // 4th
                            case 21u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (42)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (6)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 22u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (44)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     42,
                                                                     43,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (4)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 23u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (46)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     42,
                                                                     43,
                                                                     44,
                                                                     45,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (2)
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 24u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (48)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     42,
                                                                     43,
                                                                     44,
                                                                     45,
                                                                     46,
                                                                     47,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79);
                                break;
                            }
                            // error
                            [[unlikely]] default:
                            {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
# endif
                                ::fast_io::unreachable();
                            }
                        }

                        processed_byte_counter += first_round;
                        processed_simd_counter += curr_table_processed_simd;
                    }

                    if(need_handle_error) [[unlikely]]
                    {
                        error_handler(64u);
                        continue;
                    }

# if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_ucmpb512_mask)
                    constexpr u8x64simd u8x64simd_128{0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u};

                    ::std::uint64_t const mask_table_u8x64simd_lt128_mask{
                        __builtin_ia32_ucmpb512_mask(::std::bit_cast<u8x64simd>(mask_table), u8x64simd_128, 0x01, UINT64_MAX)};
# else
#  error "missing instructions"
# endif

                    auto const mask_res{
# if defined(__AVX512VBMI__) && UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512_mask)
                        ::std::bit_cast<u16x32simd>(__builtin_ia32_permvarqi512_mask(::std::bit_cast<c8x64simd>(simd_vector_str),
                                                                                     ::std::bit_cast<c8x64simd>(mask_table),
                                                                                     c8x64simd{},
                                                                                     mask_table_u8x64simd_lt128_mask))
# elif defined(__AVX512VBMI__) && UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512) && UWVM_HAS_BUILTIN(__builtin_ia32_selectb_512)
                        ::std::bit_cast<u16x32simd>(__builtin_ia32_selectb_512(
                            mask_table_u8x64simd_lt128_mask,
                            __builtin_ia32_permvarqi512(::std::bit_cast<c8x64simd>(simd_vector_str), ::std::bit_cast<c8x64simd>(mask_table)),
                            c8x64simd{}))
# else
#  error "missing instructions"
# endif
                    };

                    auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                    // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                    auto const check_upper{res >= simd_vector_check};

                    if(
# if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestmb512)  // Only supported by GCC
                        // gcc:
                        // vptestmb        k0, zmm0, zmm0
                        // kortestq        k0, k0
                        // sete    al
                        __builtin_ia32_ptestmb512(::std::bit_cast<c8x64simd>(check_upper), ::std::bit_cast<c8x64simd>(check_upper), UINT64_MAX)
# elif defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_cmpb512_mask)
                        // clang:
                        // vptestmd        k0, zmm0, zmm0
                        // kortestw        k0, k0
                        // setne   al
                        // vzeroupper
                        //
                        // gcc:
                        // vpxor   xmm1, xmm1, xmm1
                        // vpcmpb  k0, zmm0, zmm1, 4
                        // kortestq        k0, k0
                        // setne   al
                        __builtin_ia32_cmpb512_mask(::std::bit_cast<c8x64simd>(check_upper), c8x64simd{}, 0x04, UINT64_MAX)
# else
#  error "missing instructions"
# endif
                            ) [[unlikely]]
                    {
                        // [before_section ... | func_count ... typeidx1 ... (63) ...] ...
                        // [                        safe                             ] unsafe (could be the section_end)
                        //                                      ^^ section_curr
                        //                                                             ^^ section_curr + 64uz

                        // it can be processed up to 64
                        error_handler(64uz);

                        // Start the next round straight away
                        continue;
                    }

                    auto const need_write{__builtin_shufflevector(::std::bit_cast<u8x64simd>(res),
                                                                  ::std::bit_cast<u8x64simd>(res),
                                                                  0,
                                                                  2,
                                                                  4,
                                                                  6,
                                                                  8,
                                                                  10,
                                                                  12,
                                                                  14,
                                                                  16,
                                                                  18,
                                                                  20,
                                                                  22,
                                                                  24,
                                                                  26,
                                                                  28,
                                                                  30,
                                                                  32,
                                                                  34,
                                                                  36,
                                                                  38,
                                                                  40,
                                                                  42,
                                                                  44,
                                                                  46,
                                                                  48,
                                                                  50,
                                                                  52,
                                                                  54,
                                                                  56,
                                                                  58,
                                                                  60,
                                                                  62,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1)};

                    auto const need_write_u8x32x2{::std::bit_cast<::fast_io::array<u8x32simd, 2uz>>(need_write)};

                    auto const need_write_u8x32x2v0{need_write_u8x32x2.front_unchecked()};

                    need_shu_write_u8x32x2.index_unchecked(0) = need_write_u8x32x2v0;
                }
                else
                {
                    auto const need_write_u8x32x2{::std::bit_cast<::fast_io::array<u8x32simd, 2uz>>(simd_vector_str)};

                    auto const need_write_u8x32x2v0{need_write_u8x32x2.front_unchecked()};

                    need_shu_write_u8x32x2.index_unchecked(0) = need_write_u8x32x2v0;

                    check_mask_curr >>= 32u;

                    processed_byte_counter = 32u;
                    processed_simd_counter = 32u;
                }

                auto const processed_simd_counter_1st{processed_simd_counter};
                auto const processed_byte_counter_1st{processed_byte_counter};

                processed_simd_counter = 0u;
                processed_byte_counter = 0u;

                // 2nd
                if(check_mask_curr & 0xFFFF'FFFFu)
                {
                    bool need_handle_error{};

                    u8x64simd mask_table;  // No initialization necessary

                    for(unsigned i{}; i != 4u; ++i)
                    {
                        unsigned const check_table_index{static_cast<unsigned>(check_mask_curr & 0xFFu)};

                        auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                        auto const curr_table_shuffle_mask{curr_table.shuffle_mask};
                        auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32

                        if(!curr_table_processed_simd) [[unlikely]]
                        {
                            need_handle_error = true;
                            break;
                        }

                        bool const first_round_is_seven{static_cast<bool>(check_table_index & 0x80u)};
                        unsigned const first_round{8u - static_cast<unsigned>(first_round_is_seven)};

                        check_mask_curr >>= first_round;

                        ::fast_io::array<u8x16simd, 4uz> const u8x64simd_tmp_u8x16x4{
                            curr_table_shuffle_mask + static_cast<::std::uint8_t>(processed_byte_counter_1st + processed_byte_counter)};
                        u8x64simd u8x64simd_tmp{::std::bit_cast<u8x64simd>(u8x64simd_tmp_u8x16x4)};

                        switch(processed_byte_counter)
                        {
                            // 1st
                            case 0u:
                            {
                                mask_table = u8x64simd_tmp;
                                break;
                            }
                            // 2nd
                            case 7u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (14)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (34)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 8u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (16)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (32)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            // 3rd
                            case 14u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (28)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (20)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 15u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (30)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (18)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 16u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (32)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (16)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            // 4th
                            case 21u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (42)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (6)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 22u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (44)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     42,
                                                                     43,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (4)
                                                                     -1,
                                                                     -1,
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 23u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (46)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     42,
                                                                     43,
                                                                     44,
                                                                     45,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79,
                                                                     // -1 (2)
                                                                     -1,
                                                                     -1);
                                break;
                            }
                            case 24u:
                            {
                                mask_table = __builtin_shufflevector(mask_table,
                                                                     u8x64simd_tmp,
                                                                     // raw (48)
                                                                     0,
                                                                     1,
                                                                     2,
                                                                     3,
                                                                     4,
                                                                     5,
                                                                     6,
                                                                     7,
                                                                     8,
                                                                     9,
                                                                     10,
                                                                     11,
                                                                     12,
                                                                     13,
                                                                     14,
                                                                     15,
                                                                     16,
                                                                     17,
                                                                     18,
                                                                     19,
                                                                     20,
                                                                     21,
                                                                     22,
                                                                     23,
                                                                     24,
                                                                     25,
                                                                     26,
                                                                     27,
                                                                     28,
                                                                     29,
                                                                     30,
                                                                     31,
                                                                     32,
                                                                     33,
                                                                     34,
                                                                     35,
                                                                     36,
                                                                     37,
                                                                     38,
                                                                     39,
                                                                     40,
                                                                     41,
                                                                     42,
                                                                     43,
                                                                     44,
                                                                     45,
                                                                     46,
                                                                     47,
                                                                     // new (16)
                                                                     64,
                                                                     65,
                                                                     66,
                                                                     67,
                                                                     68,
                                                                     69,
                                                                     70,
                                                                     71,
                                                                     72,
                                                                     73,
                                                                     74,
                                                                     75,
                                                                     76,
                                                                     77,
                                                                     78,
                                                                     79);
                                break;
                            }
                            // error
                            [[unlikely]] default:
                            {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                                ::uwvm2::utils::debug::trap_and_inform_bug_pos();
# endif
                                ::fast_io::unreachable();
                            }
                        }

                        processed_byte_counter += first_round;
                        processed_simd_counter += curr_table_processed_simd;
                    }

                    if(need_handle_error) [[unlikely]]
                    {
                        error_handler(64u);
                        continue;
                    }

# if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_ucmpb512_mask)
                    constexpr u8x64simd u8x64simd_128{0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
                                                      0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u};

                    ::std::uint64_t const mask_table_u8x64simd_lt128_mask{
                        __builtin_ia32_ucmpb512_mask(::std::bit_cast<u8x64simd>(mask_table), u8x64simd_128, 0x01, UINT64_MAX)};
# else
#  error "missing instructions"
# endif

                    auto const mask_res{
# if defined(__AVX512VBMI__) && UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512_mask)
                        ::std::bit_cast<u16x32simd>(__builtin_ia32_permvarqi512_mask(::std::bit_cast<c8x64simd>(simd_vector_str),
                                                                                     ::std::bit_cast<c8x64simd>(mask_table),
                                                                                     c8x64simd{},
                                                                                     mask_table_u8x64simd_lt128_mask))
# elif defined(__AVX512VBMI__) && UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512) && UWVM_HAS_BUILTIN(__builtin_ia32_selectb_512)
                        ::std::bit_cast<u16x32simd>(__builtin_ia32_selectb_512(
                            mask_table_u8x64simd_lt128_mask,
                            __builtin_ia32_permvarqi512(::std::bit_cast<c8x64simd>(simd_vector_str), ::std::bit_cast<c8x64simd>(mask_table)),
                            c8x64simd{}))
# else
#  error "missing instructions"
# endif
                    };

                    auto const res{(mask_res & static_cast<::std::uint16_t>(0x7Fu)) | ((mask_res & static_cast<::std::uint16_t>(0x7F00u)) >> 1u)};

                    // The data out of shuffle is 16-bit [0, 2^14) and may be greater than or equal to typeidx, which needs to be checked.

                    auto const check_upper{res >= simd_vector_check};

                    if(
# if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_ptestmb512)  // Only supported by GCC
                        // gcc:
                        // vptestmb        k0, zmm0, zmm0
                        // kortestq        k0, k0
                        // sete    al
                        __builtin_ia32_ptestmb512(::std::bit_cast<c8x64simd>(check_upper), ::std::bit_cast<c8x64simd>(check_upper), UINT64_MAX)
# elif defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_cmpb512_mask)
                        // clang:
                        // vptestmd        k0, zmm0, zmm0
                        // kortestw        k0, k0
                        // setne   al
                        // vzeroupper
                        //
                        // gcc:
                        // vpxor   xmm1, xmm1, xmm1
                        // vpcmpb  k0, zmm0, zmm1, 4
                        // kortestq        k0, k0
                        // setne   al
                        __builtin_ia32_cmpb512_mask(::std::bit_cast<c8x64simd>(check_upper), c8x64simd{}, 0x04, UINT64_MAX)
# else
#  error "missing instructions"
# endif
                            ) [[unlikely]]
                    {
                        // [before_section ... | func_count ... typeidx1 ... (63) ...] ...
                        // [                        safe                             ] unsafe (could be the section_end)
                        //                                      ^^ section_curr
                        //                                                             ^^ section_curr + 64uz

                        // it can be processed up to 64
                        error_handler(64uz);

                        // Start the next round straight away
                        continue;
                    }

                    auto const need_write{__builtin_shufflevector(::std::bit_cast<u8x64simd>(res),
                                                                  ::std::bit_cast<u8x64simd>(res),
                                                                  0,
                                                                  2,
                                                                  4,
                                                                  6,
                                                                  8,
                                                                  10,
                                                                  12,
                                                                  14,
                                                                  16,
                                                                  18,
                                                                  20,
                                                                  22,
                                                                  24,
                                                                  26,
                                                                  28,
                                                                  30,
                                                                  32,
                                                                  34,
                                                                  36,
                                                                  38,
                                                                  40,
                                                                  42,
                                                                  44,
                                                                  46,
                                                                  48,
                                                                  50,
                                                                  52,
                                                                  54,
                                                                  56,
                                                                  58,
                                                                  60,
                                                                  62,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1,
                                                                  -1)};

                    auto const need_write_u8x32x2{::std::bit_cast<::fast_io::array<u8x32simd, 2uz>>(need_write)};

                    auto const need_write_u8x32x2v0{need_write_u8x32x2.front_unchecked()};

                    need_shu_write_u8x32x2.index_unchecked(1) = need_write_u8x32x2v0;
                }
                else
                {
                    switch(processed_byte_counter_1st)
                    {
                        case 28u:
                        {
                            need_write = __builtin_shufflevector(simd_vector_str,
                                                                 simd_vector_str,
                                                                28,
                                                                29,
                                                                30,
                                                                31,
                                                                32,
                                                                33,
                                                                34,
                                                                35,
                                                                36,
                                                                37,
                                                                38,
                                                                39,
                                                                40,
                                                                41,
                                                                42,
                                                                43,
                                                                44,
                                                                45,
                                                                46,
                                                                47,
                                                                48,
                                                                49,
                                                                50,
                                                                51,
                                                                52,
                                                                53,
                                                                54,
                                                                55,
                                                                56,
                                                                57,
                                                                58,
                                                                59,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1);
                            break;
                        }
                        case 29u:
                        {
                            need_write = __builtin_shufflevector(simd_vector_str,
                                                                 simd_vector_str,
                                                                29,
                                                                30,
                                                                31,
                                                                32,
                                                                33,
                                                                34,
                                                                35,
                                                                36,
                                                                37,
                                                                38,
                                                                39,
                                                                40,
                                                                41,
                                                                42,
                                                                43,
                                                                44,
                                                                45,
                                                                46,
                                                                47,
                                                                48,
                                                                49,
                                                                50,
                                                                51,
                                                                52,
                                                                53,
                                                                54,
                                                                55,
                                                                56,
                                                                57,
                                                                58,
                                                                59,
                                                                60,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1);
                            break;
                        }
                        case 30u:
                        {
                            need_write = __builtin_shufflevector(simd_vector_str,
                                                                 simd_vector_str,
                                                                30,
                                                                31,
                                                                32,
                                                                33,
                                                                34,
                                                                35,
                                                                36,
                                                                37,
                                                                38,
                                                                39,
                                                                40,
                                                                41,
                                                                42,
                                                                43,
                                                                44,
                                                                45,
                                                                46,
                                                                47,
                                                                48,
                                                                49,
                                                                50,
                                                                51,
                                                                52,
                                                                53,
                                                                54,
                                                                55,
                                                                56,
                                                                57,
                                                                58,
                                                                59,
                                                                60,
                                                                61,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1);
                            break;
                        }
                        case 31u:
                        {
                            need_write = __builtin_shufflevector(simd_vector_str,
                                                                 simd_vector_str,
                                                                31,
                                                                32,
                                                                33,
                                                                34,
                                                                35,
                                                                36,
                                                                37,
                                                                38,
                                                                39,
                                                                40,
                                                                41,
                                                                42,
                                                                43,
                                                                44,
                                                                45,
                                                                46,
                                                                47,
                                                                48,
                                                                49,
                                                                50,
                                                                51,
                                                                52,
                                                                53,
                                                                54,
                                                                55,
                                                                56,
                                                                57,
                                                                58,
                                                                59,
                                                                60,
                                                                61,
                                                                62,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1);                            
                            break;
                        }
                        case 32u:
                        {                       
                            need_write = __builtin_shufflevector(simd_vector_str,
                                                                 simd_vector_str,
                                                                32,
                                                                33,
                                                                34,
                                                                35,
                                                                36,
                                                                37,
                                                                38,
                                                                39,
                                                                40,
                                                                41,
                                                                42,
                                                                43,
                                                                44,
                                                                45,
                                                                46,
                                                                47,
                                                                48,
                                                                49,
                                                                50,
                                                                51,
                                                                52,
                                                                53,
                                                                54,
                                                                55,
                                                                56,
                                                                57,
                                                                58,
                                                                59,
                                                                60,
                                                                61,
                                                                62,
                                                                63,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1,
                                                                -1);      
                            break;
                        }
                        [[unlikely]] default:
                        {
    # if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                            ::uwvm2::utils::debug::trap_and_inform_bug_pos();
    # endif
                            ::fast_io::unreachable();
                        }
                    }

                    auto const need_write_u8x32x2{::std::bit_cast<::fast_io::array<u8x32simd, 2uz>>(need_write)};

                    auto const need_write_u8x32x2v0{need_write_u8x32x2.front_unchecked()};

                    need_shu_write_u8x32x2.index_unchecked(1) = need_write_u8x32x2v0;

                    check_mask_curr >>= 32u;

                    processed_byte_counter = 32u;
                    processed_simd_counter = 32u;
                }

                auto const processed_simd_counter_sum{processed_simd_counter + processed_simd_counter_1st};
                auto const processed_byte_counter_sum{processed_byte_counter + processed_byte_counter_1st};

                auto const need_shu_write_u8x64{::std::bit_cast<u8x64simd>(need_shu_write_u8x32x2)};

                u8x64simd need_write;

                switch(processed_byte_counter_1st)
                {
                    case 28u:
                    {
                        need_write = __builtin_shufflevector(need_shu_write_u8x64,
                                                             need_shu_write_u8x64,
                                                             0,
                                                             1,
                                                             2,
                                                             3,
                                                             4,
                                                             5,
                                                             6,
                                                             7,
                                                             8,
                                                             9,
                                                             10,
                                                             11,
                                                             12,
                                                             13,
                                                             14,
                                                             15,
                                                             16,
                                                             17,
                                                             18,
                                                             19,
                                                             20,
                                                             21,
                                                             22,
                                                             23,
                                                             24,
                                                             25,
                                                             26,
                                                             27,
                                                             32,
                                                             33,
                                                             34,
                                                             35,
                                                             36,
                                                             37,
                                                             38,
                                                             39,
                                                             40,
                                                             41,
                                                             42,
                                                             43,
                                                             44,
                                                             45,
                                                             46,
                                                             47,
                                                             48,
                                                             49,
                                                             50,
                                                             51,
                                                             52,
                                                             53,
                                                             54,
                                                             55,
                                                             56,
                                                             57,
                                                             58,
                                                             59,
                                                             60,
                                                             61,
                                                             62,
                                                             63,
                                                             -1,
                                                             -1,
                                                             -1,
                                                             -1);
                        break;
                    }
                    case 29u:
                    {
                        need_write = __builtin_shufflevector(need_shu_write_u8x64,
                                                             need_shu_write_u8x64,
                                                             0,
                                                             1,
                                                             2,
                                                             3,
                                                             4,
                                                             5,
                                                             6,
                                                             7,
                                                             8,
                                                             9,
                                                             10,
                                                             11,
                                                             12,
                                                             13,
                                                             14,
                                                             15,
                                                             16,
                                                             17,
                                                             18,
                                                             19,
                                                             20,
                                                             21,
                                                             22,
                                                             23,
                                                             24,
                                                             25,
                                                             26,
                                                             27,
                                                             28,
                                                             32,
                                                             33,
                                                             34,
                                                             35,
                                                             36,
                                                             37,
                                                             38,
                                                             39,
                                                             40,
                                                             41,
                                                             42,
                                                             43,
                                                             44,
                                                             45,
                                                             46,
                                                             47,
                                                             48,
                                                             49,
                                                             50,
                                                             51,
                                                             52,
                                                             53,
                                                             54,
                                                             55,
                                                             56,
                                                             57,
                                                             58,
                                                             59,
                                                             60,
                                                             61,
                                                             62,
                                                             63,
                                                             -1,
                                                             -1,
                                                             -1);
                        break;
                    }
                    case 30u:
                    {
                        need_write = __builtin_shufflevector(need_shu_write_u8x64,
                                                             need_shu_write_u8x64,
                                                             0,
                                                             1,
                                                             2,
                                                             3,
                                                             4,
                                                             5,
                                                             6,
                                                             7,
                                                             8,
                                                             9,
                                                             10,
                                                             11,
                                                             12,
                                                             13,
                                                             14,
                                                             15,
                                                             16,
                                                             17,
                                                             18,
                                                             19,
                                                             20,
                                                             21,
                                                             22,
                                                             23,
                                                             24,
                                                             25,
                                                             26,
                                                             27,
                                                             28,
                                                             29,
                                                             32,
                                                             33,
                                                             34,
                                                             35,
                                                             36,
                                                             37,
                                                             38,
                                                             39,
                                                             40,
                                                             41,
                                                             42,
                                                             43,
                                                             44,
                                                             45,
                                                             46,
                                                             47,
                                                             48,
                                                             49,
                                                             50,
                                                             51,
                                                             52,
                                                             53,
                                                             54,
                                                             55,
                                                             56,
                                                             57,
                                                             58,
                                                             59,
                                                             60,
                                                             61,
                                                             62,
                                                             63,
                                                             -1,
                                                             -1);
                        break;
                    }
                    case 31u:
                    {
                        need_write = __builtin_shufflevector(need_shu_write_u8x64,
                                                             need_shu_write_u8x64,
                                                             0,
                                                             1,
                                                             2,
                                                             3,
                                                             4,
                                                             5,
                                                             6,
                                                             7,
                                                             8,
                                                             9,
                                                             10,
                                                             11,
                                                             12,
                                                             13,
                                                             14,
                                                             15,
                                                             16,
                                                             17,
                                                             18,
                                                             19,
                                                             20,
                                                             21,
                                                             22,
                                                             23,
                                                             24,
                                                             25,
                                                             26,
                                                             27,
                                                             28,
                                                             29,
                                                             30,
                                                             32,
                                                             33,
                                                             34,
                                                             35,
                                                             36,
                                                             37,
                                                             38,
                                                             39,
                                                             40,
                                                             41,
                                                             42,
                                                             43,
                                                             44,
                                                             45,
                                                             46,
                                                             47,
                                                             48,
                                                             49,
                                                             50,
                                                             51,
                                                             52,
                                                             53,
                                                             54,
                                                             55,
                                                             56,
                                                             57,
                                                             58,
                                                             59,
                                                             60,
                                                             61,
                                                             62,
                                                             63,
                                                             -1);
                        break;
                    }
                    case 32u:
                    {
                        need_write = need_shu_write_u8x64;
                        break;
                    }
                    [[unlikely]] default:
                    {
# if (defined(_DEBUG) || defined(DEBUG)) && defined(UWVM_ENABLE_DETAILED_DEBUG_CHECK)
                        ::uwvm2::utils::debug::trap_and_inform_bug_pos();
# endif
                        ::fast_io::unreachable();
                    }
                }

                func_counter += processed_simd_counter_sum;

                //  [... curr ... (63) ...]
                //  [      safe           ] unsafe (could be the section_end)
                //       ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                //      [    needwrite   ]

                ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr, ::std::addressof(need_write), sizeof(u8x64simd));

                functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += processed_simd_counter_sum;

                //  [... curr ... (63) ...]
                //  [          safe       ] unsafe (could be the section_end)
                //                    ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                // Or: (Security boundaries for writes are checked)
                //                        ^^ functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr
                //      [    needwrite   ]

                section_curr += processed_byte_counter_sum;

                // [before_section ... | func_count ... typeidx1 ... (63) ...] ...
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                   ^^^^^^^ section_curr (processed_byte_counter_sum <= 64)
                //                                      [   simd_vector_str  ]
            }
            else
            {
                // All correct, can change state: func_counter, section_curr

                // all are single bytes, so there are 64
                func_counter += 64u;

                // check counter
                if(func_counter > func_count) [[unlikely]]
                {
                    err.err_curr = section_curr;
                    err.err_selectable.u32 = func_count;
                    err.err_code = ::uwvm2::parser::wasm::base::wasm_parse_error_code::func_section_resolved_exceeded_the_actual_number;
                    ::uwvm2::parser::wasm::base::throw_wasm_parse_code(::fast_io::parse_code::invalid);
                }

                // Since everything is less than 128, there is no need to check the typeidx.

                // write data
                ::fast_io::freestanding::my_memcpy(functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr,
                                                   ::std::addressof(simd_vector_str),
                                                   sizeof(u8x64simd));

                functionsec.funcs.storage.typeidx_u8_vector.imp.curr_ptr += 64uz;

                section_curr += 64uz;

                // [before_section ... | func_count ... typeidx1 ... (63) ...] typeidx64
                // [                        safe                             ] unsafe (could be the section_end)
                //                                                             ^^ section_curr
            }
        }

