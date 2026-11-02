#ifndef WORKBENCH_STRING_VIEW_HPP
#define WORKBENCH_STRING_VIEW_HPP

#include <algorithm>
#include <compare>
#include <concepts>
#include <cstddef>
#include <string>
#include <type_traits>

namespace workbench {

    namespace detail {
        template <typename T, typename ...Us>
        concept any_of = (std::same_as<T, Us> || ...);

        template <typename T>
        concept character = any_of<T, char, wchar_t, char8_t, char16_t, char32_t>;
    
        // Structural requirements of a character type plus
        // no cv-qualifiers (e.g. basic_string_view<const char> is meaningless)
        template <typename T>
        concept char_like = std::same_as<T, std::remove_cv_t<T>> &&
                            !std::is_array_v<T> &&
                            std::is_trivially_copyable_v<T> &&
                            std::is_trivially_default_constructible_v<T> &&
                            std::is_standard_layout_v<T>;
    
        // std::char_traits is only specified for the five standard character types
        // Do not rely on generic char_traits for other types.
        // Allowed cases: 
            // 1) Character is one of five standard types + std::char_traits
            // 2) Character is another type + Traits is a custom traits.
        template <typename Char, typename Traits>
        concept portable_traits = character<Char> || !std::same_as<Traits, std::char_traits<Char>>;

        // What basic_string_view itself requires from Traits
        template <typename Char, typename Traits>
        concept view_traits = requires(const Char *p, std::size_t n) {
            requires std::same_as<typename Traits::char_type, Char>;
            { Traits::length(p) } -> std::convertible_to<std::size_t>;
            { Traits::compare(p, p, n) } -> std::convertible_to<int>;
        };

        // String-like: same traits_type + noexcept data() + noexcept size()
        template<typename T, typename Traits>
        concept string_like = requires(const T& t) {
            requires std::same_as<typename T::traits_type, Traits>;
            { t.data() } noexcept -> std::convertible_to<const typename Traits::char_type*>;
            { t.size() } noexcept -> std::convertible_to<std::size_t>;
        };

        template <typename T>
        concept ordering_category =
            any_of<T, std::partial_ordering, std::weak_ordering, std::strong_ordering>;

        template <typename Traits>
        struct traits_comparison_category {
            using type = std::weak_ordering;
        };

        template <typename Traits>
        requires requires { typename Traits::comparison_category; }
        struct traits_comparison_category<Traits> {
            using type = typename Traits::comparison_category;
        };

        template <typename Traits>
        using traits_comparison_category_t = typename traits_comparison_category<Traits>::type;
    }

    template <detail::char_like Char, typename Traits = std::char_traits<Char>>
        requires detail::portable_traits<Char, Traits> &&
                 detail::view_traits<Char, Traits>
    class basic_string_view {
    public:
        using value_type = Char;
        using traits_type = Traits;
        using const_pointer = const value_type*;
        using const_iterator = const_pointer;
        using iterator = const_iterator;  // same as const_iterator for a view
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
    private:
        // Beginning of the view
        const_pointer data_;
        // Number of characters starting from this position
        size_type size_;
    public:
        // Constructors
        constexpr basic_string_view() noexcept : data_(nullptr), size_(0) {}
        // Preconditions:
        // - If size > 0, data points to at least size valid elements.
        // - If size == 0, data may be nullptr or point to a valid position.
        constexpr basic_string_view(const_pointer data, size_type size) noexcept : 
            data_(data), size_(size) {}
        // Preconditions:
        // - data points to a null-terminated valid string
        // - data is not nullptr
        constexpr basic_string_view(const_pointer data) noexcept : data_(data),
            size_(traits_type::length(data)) {}

        template <detail::string_like<traits_type> S>
        constexpr basic_string_view(const S& s) noexcept :
            data_(s.data()), size_(s.size()) {}

        constexpr iterator begin() const noexcept { return data_; }
        constexpr iterator end() const noexcept { return (data_ == nullptr) ? data_ : data_ + size_; }
        constexpr const_pointer data() const noexcept { return data_; }
        constexpr size_type size() const noexcept { return size_; }

        // Precondition: n <= size_
        constexpr void remove_prefix(size_type n) noexcept {
            if (n == 0) return;
            data_ += n;
            size_  -= n;
        }

        // Precondition: n <= size_
        constexpr void remove_suffix(size_type n) noexcept {
            size_ -= n;
        }

        // Precondition: pos < size_
        constexpr const_reference operator[](size_type pos) const noexcept {
            return data_[pos];
        }

        constexpr int compare(basic_string_view other) const noexcept {
            size_type min_sz = std::min(size(), other.size());
            if (min_sz != 0) {
                if (const int r = traits_type::compare(data_, other.data(), min_sz); r != 0)
                    return r < 0 ? -1 : 1;
            }
            return (size() == other.size()) ? 0 : (size() < other.size() ? -1 : 1);
        }

        constexpr bool empty() const noexcept {
            return size_ == 0;
        }

        // for fast equality check
        constexpr friend bool operator==(basic_string_view lhs,
                                         basic_string_view rhs) noexcept {
            return lhs.size() == rhs.size() &&
                lhs.compare(rhs) == 0;
        }

        [[nodiscard]] constexpr friend auto operator<=>(basic_string_view lhs,
                                          basic_string_view rhs) noexcept {
            using R = detail::traits_comparison_category_t<traits_type>;                              
            static_assert(detail::ordering_category<R>,
                "Traits::comparison_category must be a comparison category type");
            return static_cast<R>(lhs.compare(rhs) <=> 0);
        }
    };

    using string_view    = basic_string_view<char>;
    using wstring_view   = basic_string_view<wchar_t>;
    using u8string_view  = basic_string_view<char8_t>;
    using u16string_view = basic_string_view<char16_t>;
    using u32string_view = basic_string_view<char32_t>;

} // namespace workbench

#endif // WORKBENCH_STRING_VIEW_HPP
