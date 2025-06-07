# endif __has_cpp_attribute(__gnu__::__vector_size__) && defined(__LITTLE_ENDIAN__) && UWVM_HAS_BUILTIN(__builtin_shufflevector) && \
        ((defined(__AVX512VBMI__) && UWVM_HAS_BUILTIN(__builtin_ia32_permvarqi512_mask)) && (defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_cvtb2mask512)))
        /// (Little Endian), [[gnu::vector_size]], has mask-u16, can shuffle, simd512
        /// x86_64-avx512vbmi

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

            bool need_handle_error{};

            [&] UWVM_ALWAYS_INLINE () constexpr UWVM_THROWS -> void
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
                    ::std::uint16_t const simd_vector_check{static_cast<::std::uint16_t>(type_section_count)};

                    ::std::uint64_t check_mask_curr{check_mask};
                        
                    u8x64simd mask_table;   // No initialization necessary

                    ::std::uint64_t mov_mask{0xFFFFu}; // Up to 16 digits in one mix 

                    ::std::uint8_t processed_simd_counter{};
                    ::std::uint8_t processed_byte_counter{};

                    for (unsigned i{}; i != 4u; ++i)
                    {
                        unsigned const check_table_index{check_mask_curr & 0xFFu};

                        if (check_table_index)
                        {
                            auto const& curr_table{simd128_shuffle_table.index_unchecked(check_table_index)};
                            auto const curr_table_shuffle_mask{curr_table.shuffle_mask};
                            auto const curr_table_processed_simd{curr_table.processed_simd};  // size of handled u32

                            if (!curr_table_shuffle_mask) [[unlikely]]
                            {
                                need_handle_error = true;
                                return;
                            }

                            bool const first_round_is_seven{static_cast<bool>(check_table_index & 0x80u)};
                            unsigned const first_round{8u - static_cast<unsigned>(first_round_is_seven)};

                            processed_byte_counter += first_round;

                            check_mask_curr >>= first_round;

                            mov_mask <<= first_round;

                            ::fast_io::array<u8x16simd, 4uz> const u8x64simd_tmp_u8x16x4{curr_table_shuffle_mask + processed_byte_counter};
                            u8x64simd u8x64simd_tmp{::std::bit_cast<u8x64simd>(u8x64simd_tmp_u8x16x4)};

                            switch()
                            {
                                
                            }

    # if defined(__AVX512BW__) && UWVM_HAS_BUILTIN(__builtin_ia32_movdquqi512_mask)
                            mask_table = __builtin_ia32_movdquqi512_mask(,mask_table,mov_mask);
    #  error "missing instructions"
    # endif

                        }
                        else
                        {

                        }
                    }

                    __builtin_ia32_permvarqi512_mask(::std::bit_cast<c8x64simd>(mask_table), ::std::bit_cast<c8x64simd>(simd_vector_str), c8x64simd{}, permute_mask);

                    permute_mask = 0xFFFFu;

                    for (unsigned i{}; i != 4u; ++i)
                    {
                        unsigned const check_table_index{check_mask_curr & 0xFFu};

                        bool const first_round_is_seven{static_cast<bool>(check_table_index_1st & 0x80u)};
                        unsigned const first_round{8u - static_cast<unsigned>(first_round_is_seven)};

                        check_mask_curr >>= first_round;

                        permute_mask <<= first_round;



                    }

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
            }();

            if (need_handle_error) [[unlikely]]
            {
                error_handler(64uz);
            }
        }