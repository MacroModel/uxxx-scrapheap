    // function_section
    static_assert(::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>> &&
                  ::fast_io::freestanding::is_zero_default_constructible_v<::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>> &&
                  ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16>> &&
                  ::fast_io::freestanding::is_zero_default_constructible_v<::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16>> &&
                  ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>> &&
                  ::fast_io::freestanding::is_zero_default_constructible_v<::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>>);

    union vectypeidx_minimize_storage_u
    {
        ::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8 const* typeidx_u8_begin;
        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8> typeidx_u8_vector;
        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16> typeidx_u16_vector;
        ::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32> typeidx_u32_vector;

        inline constexpr vectypeidx_minimize_storage_u() noexcept {}

        inline constexpr vectypeidx_minimize_storage_u(vectypeidx_minimize_storage_u const&) noexcept {}

        inline constexpr vectypeidx_minimize_storage_u(vectypeidx_minimize_storage_u&&) noexcept {}

        inline constexpr vectypeidx_minimize_storage_u& operator= (vectypeidx_minimize_storage_u const&) noexcept { return *this; }

        inline constexpr vectypeidx_minimize_storage_u& operator= (vectypeidx_minimize_storage_u&&) noexcept { return *this; }

        inline constexpr ~vectypeidx_minimize_storage_u() {}
    };

    enum class vectypeidx_minimize_storage_mode : unsigned
    {
        u8_begin = 0u,
        u8_vector,
        u16_vector,
        u32_vector
    };

    struct vectypeidx_minimize_storage_t UWVM_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
    {
        vectypeidx_minimize_storage_u storage;
        vectypeidx_minimize_storage_mode mode{};

        inline constexpr vectypeidx_minimize_storage_t() noexcept
        {
            ::fast_io::freestanding::my_memset(::std::addressof(this->storage), 0, sizeof(vectypeidx_minimize_storage_u));
        }

        inline constexpr vectypeidx_minimize_storage_t(vectypeidx_minimize_storage_t const& other) noexcept : mode{other.mode}
        {
            switch(this->mode)
            {
                case vectypeidx_minimize_storage_mode::u8_begin:
                {
                    this->storage.typeidx_u8_begin = other.storage.typeidx_u8_begin;
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u8_vector))::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u8>{
                        other.storage.typeidx_u8_vector};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u16_vector))::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u16>{
                        other.storage.typeidx_u16_vector};
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    ::new(::std::addressof(this->storage.typeidx_u32_vector))::fast_io::vector<::uwvm2::parser::wasm::standard::wasm1::type::wasm_u32>{
                        other.storage.typeidx_u32_vector};
                    break;
                }
                [[unlikely]] default:
                {
                    ::fast_io::fast_terminate();
                }
            }
        }

        inline constexpr vectypeidx_minimize_storage_t(vectypeidx_minimize_storage_t&& other) noexcept : mode{other.mode}
        {
            // Since fast_io::vector satisfies both is_trivially_copyable_or_relocatable and is_zero_default_constructible, it is possible to do this
            other.mode = {};  // set to u8_begin

            // move data
            switch(this->mode)
            {
                case vectypeidx_minimize_storage_mode::u8_begin:
                {
                    this->storage.typeidx_u8_begin = other.storage.typeidx_u8_begin;
                    ::fast_io::freestanding::my_memset(::std::addressof(other.storage), 0, sizeof(vectypeidx_minimize_storage_u));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    this->storage.typeidx_u8_vector = ::std::move(other.storage.typeidx_u8_vector);
                    UWVM_ASSERT(other.storage.typeidx_u8_vector.imp.begin_ptr == nullptr && other.storage.typeidx_u8_vector.imp.curr_ptr == nullptr &&
                                other.storage.typeidx_u8_vector.imp.end_ptr == nullptr);
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    this->storage.typeidx_u16_vector = ::std::move(other.storage.typeidx_u16_vector);
                    UWVM_ASSERT(other.storage.typeidx_u16_vector.imp.begin_ptr == nullptr && other.storage.typeidx_u16_vector.imp.curr_ptr == nullptr &&
                                other.storage.typeidx_u16_vector.imp.end_ptr == nullptr);
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    this->storage.typeidx_u32_vector = ::std::move(other.storage.typeidx_u32_vector);
                    UWVM_ASSERT(other.storage.typeidx_u32_vector.imp.begin_ptr == nullptr && other.storage.typeidx_u32_vector.imp.curr_ptr == nullptr &&
                                other.storage.typeidx_u32_vector.imp.end_ptr == nullptr);
                    break;
                }
                [[unlikely]] default:
                {
                    ::fast_io::fast_terminate();
                }
            }
        }

        inline constexpr vectypeidx_minimize_storage_t& operator= (vectypeidx_minimize_storage_t const& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            clear_destroy();

            this->mode = other.mode;

            switch(this->mode)
            {
                case vectypeidx_minimize_storage_mode::u8_begin:
                {
                    this->storage.typeidx_u8_begin = other.storage.typeidx_u8_begin;
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    this->storage.typeidx_u8_vector = other.storage.typeidx_u8_vector;
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    this->storage.typeidx_u16_vector = other.storage.typeidx_u16_vector;
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    this->storage.typeidx_u32_vector = other.storage.typeidx_u32_vector;
                    break;
                }
                [[unlikely]] default:
                {
                    ::fast_io::fast_terminate();
                }
            }

            return *this;
        }

        inline constexpr vectypeidx_minimize_storage_t& operator= (vectypeidx_minimize_storage_t&& other) noexcept
        {
            if(::std::addressof(other) == this) [[unlikely]] { return *this; }

            clear_destroy();

            // Since fast_io::vector satisfies both is_trivially_copyable_or_relocatable and is_zero_default_constructible, it is possible to do this

            this->mode = other.mode;

            other.mode = {};  // set to u8_begin

            switch(this->mode)
            {
                case vectypeidx_minimize_storage_mode::u8_begin:
                {
                    this->storage.typeidx_u8_begin = other.storage.typeidx_u8_begin;
                    ::fast_io::freestanding::my_memset(::std::addressof(other.storage), 0, sizeof(vectypeidx_minimize_storage_u));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    this->storage.typeidx_u8_vector = ::std::move(other.storage.typeidx_u8_vector);
                    UWVM_ASSERT(other.storage.typeidx_u8_vector.imp.begin_ptr == nullptr && other.storage.typeidx_u8_vector.imp.curr_ptr == nullptr &&
                                other.storage.typeidx_u8_vector.imp.end_ptr == nullptr);
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    this->storage.typeidx_u16_vector = ::std::move(other.storage.typeidx_u16_vector);
                    UWVM_ASSERT(other.storage.typeidx_u16_vector.imp.begin_ptr == nullptr && other.storage.typeidx_u16_vector.imp.curr_ptr == nullptr &&
                                other.storage.typeidx_u16_vector.imp.end_ptr == nullptr);
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    this->storage.typeidx_u32_vector = ::std::move(other.storage.typeidx_u32_vector);
                    UWVM_ASSERT(other.storage.typeidx_u32_vector.imp.begin_ptr == nullptr && other.storage.typeidx_u32_vector.imp.curr_ptr == nullptr &&
                                other.storage.typeidx_u32_vector.imp.end_ptr == nullptr);
                    break;
                }
                [[unlikely]] default:
                {
                    ::fast_io::fast_terminate();
                }
            }

            return *this;
        }

        inline constexpr ~vectypeidx_minimize_storage_t() { clear_destroy(); }

        inline constexpr void clear_destroy() noexcept
        {
            switch(this->mode)
            {
                case vectypeidx_minimize_storage_mode::u8_begin:
                {
                    ::fast_io::freestanding::my_memset(::std::addressof(this->storage), 0, sizeof(vectypeidx_minimize_storage_u));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    this->storage.typeidx_u8_vector.clear_destroy();
                    UWVM_ASSERT(this->storage.typeidx_u8_vector.imp.begin_ptr == nullptr && this->storage.typeidx_u8_vector.imp.curr_ptr == nullptr &&
                                this->storage.typeidx_u8_vector.imp.end_ptr == nullptr);

                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    this->storage.typeidx_u16_vector.clear_destroy();
                    UWVM_ASSERT(this->storage.typeidx_u16_vector.imp.begin_ptr == nullptr && this->storage.typeidx_u16_vector.imp.curr_ptr == nullptr &&
                                this->storage.typeidx_u16_vector.imp.end_ptr == nullptr);

                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    this->storage.typeidx_u32_vector.clear_destroy();
                    UWVM_ASSERT(this->storage.typeidx_u32_vector.imp.begin_ptr == nullptr && this->storage.typeidx_u32_vector.imp.curr_ptr == nullptr &&
                                this->storage.typeidx_u32_vector.imp.end_ptr == nullptr);

                    break;
                }
                [[unlikely]] default:
                {
                    ::fast_io::fast_terminate();
                }
            }

            this->mode = {};
        }

        inline constexpr void clear() noexcept
        {
            switch(this->mode)
            {
                case vectypeidx_minimize_storage_mode::u8_begin:
                {
                    ::fast_io::freestanding::my_memset(::std::addressof(this->storage), 0, sizeof(vectypeidx_minimize_storage_u));
                    break;
                }
                case vectypeidx_minimize_storage_mode::u8_vector:
                {
                    this->storage.typeidx_u8_vector.clear();
                    break;
                }
                case vectypeidx_minimize_storage_mode::u16_vector:
                {
                    this->storage.typeidx_u16_vector.clear();
                    break;
                }
                case vectypeidx_minimize_storage_mode::u32_vector:
                {
                    this->storage.typeidx_u32_vector.clear();
                    break;
                }
                [[unlikely]] default:
                {
                    ::fast_io::fast_terminate();
                }
            }
        }
    };