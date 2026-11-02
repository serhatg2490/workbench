#include <workbench/string_view.hpp>

#include <algorithm>
#include <cstring>
#include <cwchar>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

using workbench::basic_string_view;
using workbench::string_view;
using workbench::wstring_view;
using workbench::u8string_view;
using workbench::u16string_view;
using workbench::u32string_view;
namespace detail = workbench::detail;

namespace {
    // Test helper: not part of basic_string_view's interface.
    // Converting to std::basic_string is basic_string's job.
    template<typename Char, typename Traits>
    std::basic_string<Char, Traits> to_string(basic_string_view<Char, Traits> sv) {
        if (sv.empty()) {
            return {};
        }
        return std::basic_string<Char, Traits>(sv.data(), sv.size());
    }
}

namespace {
    struct custom_string_noexcept {
        using value_type = char;
        using traits_type = std::char_traits<char>;
        static constexpr char str[] = "Hello";
        constexpr const char* data() const noexcept { return str; }
        constexpr unsigned int size() const noexcept { return sizeof(str) - 1; }
    };

    struct custom_string_except {
        using value_type = char;
        using traits_type = std::char_traits<char>;
        static constexpr char str[] = "Hello";
        constexpr const char* data() const { return str; }
        constexpr unsigned int size() const { return sizeof(str) - 1; }
    };

    struct custom_string_other_traits {
        using value_type = char;
        struct traits_type {
            using char_type = value_type;
        };
        static constexpr char str[] = "Hello";
        constexpr const char* data() const noexcept{ return str; }
        constexpr unsigned int size() const noexcept { return sizeof(str) - 1; }
    };
}

TEST(BasicStringView, DefaultCtor) {
    basic_string_view<char> sv;
    EXPECT_EQ(sv.data(), nullptr);
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
    EXPECT_EQ(to_string(sv), "");
    EXPECT_TRUE(to_string(sv).empty());
}

TEST(BasicStringView, DataAndSizeCtorDataAndSize) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, std::strlen(data));
    EXPECT_EQ(sv.size(), 5u);
    EXPECT_EQ(data, sv.data());

    const wchar_t* wdata = L"Hello";
    basic_string_view<wchar_t> sv2(wdata, std::wcslen(wdata));
    EXPECT_EQ(sv2.size(), 5u);
    EXPECT_EQ(wdata, sv2.data());

    // Inplace construction
    basic_string_view<char> sv3("Hello", 5);
    EXPECT_EQ(sv3.size(), 5u);
    EXPECT_EQ(sv3.data(), "Hello");

    basic_string_view<wchar_t> sv4(L"Hello", 5);
    EXPECT_EQ(sv4.size(), 5u);
    EXPECT_EQ(sv4.data(), L"Hello");
}

TEST(BasicStringView, DataAndSizeCtorBeginEndPointers) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, 5);
    EXPECT_EQ(sv.begin(), data);
    EXPECT_EQ(sv.end(), data + sv.size());
}

TEST(BasicStringView, DataAndSizeCtorZeroLengthView) {
    basic_string_view<char> sv("Hello", 0);
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
}

TEST(BasicStringView, DataAndSizeCtorEmptyString) {
    const char* data = "";
    const wchar_t* wdata = L"";
    basic_string_view<char> sv(data, std::strlen(data));
    basic_string_view<wchar_t> sv2(wdata, std::wcslen(wdata));
    EXPECT_EQ(sv.data(), data);
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
    EXPECT_EQ(to_string(sv), "");
    EXPECT_TRUE(to_string(sv).empty());
    EXPECT_EQ(sv2.data(), wdata);
    EXPECT_EQ(sv2.size(), 0u);
    EXPECT_EQ(sv2.begin(), sv2.end());
    EXPECT_EQ(to_string(sv2), L"");
    EXPECT_TRUE(to_string(sv2).empty());
}

TEST(BasicStringView, DataAndSizeCtorCompileTime) {
    // Since C++17, lambda call operator is regarded as constexpr implicitly
    // if it satisfies constexpr function rules
    constexpr const char* data = "Hello";
    constexpr const basic_string_view<char> sv(data, 5);
    static_assert(sv.size() == 5u);
    static_assert(sv.data() == data);

    constexpr const wchar_t* wdata = L"Hello";
    constexpr const basic_string_view<wchar_t> sv2(wdata, 5);
    static_assert(sv2.size() == 5u);
    static_assert(sv2.data() == wdata);
}

TEST(BasicStringView, NullTerminatedStringCtorDataAndSize) {
    const char* data = "Hello";
    basic_string_view<char> sv(data);
    EXPECT_EQ(sv.size(), 5u);
    EXPECT_EQ(sv.data(), data);

    const wchar_t* wdata = L"Hello";
    basic_string_view<wchar_t> sv2(wdata);
    EXPECT_EQ(sv2.size(), 5u);
    EXPECT_EQ(sv2.data(), wdata);
    
    // Inplace construction
    basic_string_view<char> sv3("Hello");
    EXPECT_EQ(sv3.size(), 5u);
    EXPECT_EQ(sv3.data(), "Hello");

    basic_string_view<wchar_t> sv4(L"Hello");
    EXPECT_EQ(sv4.size(), 5u);
    EXPECT_EQ(sv4.data(), L"Hello");
}

TEST(BasicStringView, NullTerminatedStringCtorBeginEndPointers) {
    const char* data = "Hello";
    basic_string_view<char> sv(data);
    EXPECT_EQ(sv.begin(), data);
    EXPECT_EQ(sv.end(), data + sv.size());
}

TEST(BasicStringView, NullTerminatedStringCtorEmptyString) {
    const char* data = "";
    const wchar_t* wdata = L"";
    basic_string_view<char> sv(data);
    basic_string_view<wchar_t> sv2(wdata);
    EXPECT_EQ(sv.data(), data);
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
    EXPECT_EQ(to_string(sv), "");
    EXPECT_TRUE(to_string(sv).empty());
    EXPECT_EQ(sv2.data(), wdata);
    EXPECT_EQ(sv2.size(), 0u);
    EXPECT_EQ(sv2.begin(), sv2.end());
    EXPECT_EQ(to_string(sv2), L"");
    EXPECT_TRUE(to_string(sv2).empty());
}

TEST(BasicStringView, NullTerminatedStringCtorCompileTime) {
    // Since C++17, lambda call operator is regarded as constexpr implicitly
    // if it satisfies constexpr function rules
    constexpr const char* data = "Hello";
    constexpr const wchar_t* wdata = L"Hello";
    constexpr const basic_string_view<char> sv(data);
    constexpr const basic_string_view<wchar_t> sv2(wdata);
    static_assert(sv.size() == 5u);
    static_assert(sv.data() == data);
    static_assert(sv2.size() == 5u);
    static_assert(sv2.data() == wdata);
}

TEST(BasicStringView, StringLikeCtorDataAndSize) {
    std::string str{"Hello"};
    basic_string_view<char> sv(str);
    EXPECT_EQ(sv.size(), str.size());
    EXPECT_EQ(sv.data(), str.data());

    std::wstring wstr{L"Hello"};
    basic_string_view<wchar_t> sv2(wstr);
    EXPECT_EQ(sv2.size(), wstr.size());
    EXPECT_EQ(sv2.data(), wstr.data());
}

TEST(BasicStringView, StringLikeCtorEmptyString) {
    std::string str{""};
    basic_string_view<char> sv(str);
    EXPECT_EQ(sv.data(), str.data());
    EXPECT_EQ(sv.size(), str.size());
    EXPECT_EQ(sv.begin(), sv.end());

    std::wstring wstr{L""};
    basic_string_view<wchar_t> sv2(wstr);
    EXPECT_EQ(sv2.data(), wstr.data());
    EXPECT_EQ(sv2.size(), wstr.size());
    EXPECT_EQ(sv2.begin(), sv2.end());
}

TEST(BasicStringView, StringLikeCtorCompileTime) {
    constexpr custom_string_noexcept cs;
    constexpr basic_string_view<char> sv(cs);
    static_assert(sv.data() == cs.data());
    static_assert(sv.size() == cs.size());
}

TEST(BasicStringView, StringLikeFunctionCompileTime) {
    // These are OK
    static_assert(detail::string_like<std::string, std::char_traits<char>>);
    static_assert(detail::string_like<std::wstring, std::char_traits<wchar_t>>);
    static_assert(detail::string_like<custom_string_noexcept, std::char_traits<char>>);

    // Non-noexcept data()/size() methods are NOT supported
    static_assert(!detail::string_like<custom_string_except, std::char_traits<char>>);

    // Non-matching traits are caught at compile time
    static_assert(!detail::string_like<std::string, std::char_traits<wchar_t>>);
    static_assert(!detail::string_like<std::wstring, std::char_traits<char>>);
}

TEST(BasicStringView, CopyCtor) {
    basic_string_view<char> sv("Hello", 5);
    basic_string_view<char> sv2(sv);
    EXPECT_EQ(sv.data(), sv2.data());
    EXPECT_EQ(sv.size(), sv2.size());
    EXPECT_EQ(sv.begin(), sv2.begin());
    EXPECT_EQ(sv.end(), sv2.end());
    EXPECT_EQ(to_string(sv), to_string(sv2));
}

TEST(BasicStringView, CopyAssignment) {
    basic_string_view<char> sv("Hello", 5);
    basic_string_view<char> sv2;
    sv2 = sv;
    EXPECT_EQ(sv.data(), sv2.data());
    EXPECT_EQ(sv.size(), sv2.size());
    EXPECT_EQ(sv.begin(), sv2.begin());
    EXPECT_EQ(sv.end(), sv2.end());
    EXPECT_EQ(to_string(sv), to_string(sv2));
}

TEST(BasicStringView, RemovePrefix) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, 5);
    const size_t offset = 2;
    sv.remove_prefix(offset);
    EXPECT_EQ(sv.data(), data + offset);
}

TEST(BasicStringView, RemovePrefixAllCharacters) {
    const char data[] = {'H', 'e', 'l', 'l', 'o'};
    basic_string_view<char> sv(data, sizeof(data));
    sv.remove_prefix(sv.size());
    EXPECT_EQ(sv.data(), data + sizeof(data));
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
}

TEST(BasicStringView, RemovePrefixCompileTime) {
    // Since C++17, lambda call operator is regarded as constexpr implicitly
    // if it satisfies constexpr function rules
    constexpr const char* data = "Hello";
    constexpr const size_t offset = 2;
    constexpr auto r = [] {
        basic_string_view<char> sv(data, 5);
        sv.remove_prefix(offset);
        return sv;
    }();
    static_assert(r.size() == 3u);
    static_assert(*r.begin() == data[offset]);
}

TEST(BasicStringView, RemovePrefixDefaultConstructedString) {
    basic_string_view<char> sv;
    sv.remove_prefix(0);
    EXPECT_EQ(to_string(sv), "");
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
}

TEST(BasicStringView, RemovePrefixNonEmptyStringAndZeroSize) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, 0);
    sv.remove_prefix(0);
    EXPECT_EQ(sv.data(), data);
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
}

TEST(BasicStringView, AccessCharFirstAndLast) {
    basic_string_view<char> sv("Hello", 5);
    EXPECT_EQ(sv[0], 'H');
    EXPECT_EQ(sv[4], 'o');
}

TEST(BasicStringView, AccessWCharFirstAndLast) {
    basic_string_view<wchar_t> sv(L"Hello", 5);
    EXPECT_EQ(sv[0], L'H');
    EXPECT_EQ(sv[4], L'o');
}

TEST(BasicStringView, AccessAllChar) {
    basic_string_view<char> sv("Hello", 5);
    EXPECT_EQ(sv[0], 'H');
    EXPECT_EQ(sv[1], 'e');
    EXPECT_EQ(sv[2], 'l');
    EXPECT_EQ(sv[3], 'l');
    EXPECT_EQ(sv[4], 'o');
}

TEST(BasicStringView, CompareIdenticalNonEmpty) {
    basic_string_view<char> sv("abc", 3);
    basic_string_view<char> sv2("abc", 3);
    EXPECT_EQ(sv.compare(sv2), 0);
}

TEST(BasicStringView, CompareEmpty) {
    basic_string_view<char> sv;
    basic_string_view<char> sv2;
    EXPECT_EQ(sv.compare(sv2), 0);
}

TEST(BasicStringView, CompareEmptyWithNonEmpty) {
    basic_string_view<char> sv;
    basic_string_view<char> sv2("abc", 3);
    EXPECT_TRUE(sv.compare(sv2) < 0);
    basic_string_view<char> sv3("abc", 3);
    basic_string_view<char> sv4;
    EXPECT_TRUE(sv3.compare(sv4) > 0);
}

TEST(BasicStringView, CompareSameLength) {
    basic_string_view<char> sv("abc", 3);
    basic_string_view<char> sv2("abd", 3);
    basic_string_view<char> sv3("abc", 3);
    EXPECT_EQ(sv.compare(sv3), 0);
    EXPECT_TRUE(sv.compare(sv2) < 0);
    EXPECT_TRUE(sv2.compare(sv) > 0);
}

TEST(BasicStringView, CompareDifferentLengths) {
    basic_string_view<char> sv("abc", 3);
    basic_string_view<char> sv2("abcd", 4);
    EXPECT_TRUE(sv.compare(sv2) < 0);
    EXPECT_TRUE(sv2.compare(sv) > 0);
}

TEST(BasicStringView, CompareCompileTime) {
    // Since C++17, lambda call operator is regarded as constexpr implicitly
    // if it satisfies constexpr function rules
    constexpr auto r = [] {
        basic_string_view<char> sv("abc", 3);
        basic_string_view<char> sv2("abd", 3);
        basic_string_view<char> sv3("abc", 3);
        return sv.compare(sv2) < 0 &&
           sv2.compare(sv) > 0 &&
           sv.compare(sv3) == 0;
    }();
    static_assert(r);
}

TEST(BasicStringView, EqualityOperator) {
    basic_string_view<char> sv("abc", 3);
    basic_string_view<char> sv2("abd", 3);
    basic_string_view<char> sv3("abc", 3);
    EXPECT_TRUE(sv == sv3);
    EXPECT_FALSE(sv == sv2);
    EXPECT_FALSE(sv2 == sv3);
}

TEST(BasicStringView, EqualityCompileTime) {
    // Since C++17, lambda call operator is regarded as constexpr implicitly
    // if it satisfies constexpr function rules
    constexpr auto r = [] {
        basic_string_view<char> sv("abc", 3);
        basic_string_view<char> sv2("abd", 3);
        basic_string_view<char> sv3("abc", 3);
        return !(sv == sv2) &&
               !(sv2 == sv3) &&
                (sv == sv3);

    }();
    static_assert(r);
}

TEST(BasicStringView, RemoveSuffixCompileTime) {
    // Since C++17, lambda call operator is regarded as constexpr implicitly
    // if it satisfies constexpr function rules
    constexpr const char* data = "Hello";
    constexpr auto r = [] {
        basic_string_view<char> sv(data, 5);
        sv.remove_suffix(2);
        return sv;
    }();
    static_assert(r.size() == 3u);
    static_assert(*r.begin() == *data);
}

TEST(BasicStringView, RemoveSuffixDefaultConstructedString) {
    basic_string_view<char> sv;
    sv.remove_suffix(0);
    EXPECT_EQ(to_string(sv), "");
    EXPECT_EQ(sv.size(), 0u);
}

TEST(BasicStringView, RemoveSuffix) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, 5);
    sv.remove_suffix(2);
    EXPECT_EQ(sv.data(), data);
    EXPECT_EQ(sv.size(), 3u);
    EXPECT_EQ(to_string(sv), "Hel");
}

TEST(BasicStringView, RemoveSuffixAllCharacters) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, 5);
    sv.remove_suffix(sv.size());
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.data(), data);
}

TEST(BasicStringView, RemoveSuffixNonEmptyStringAndZeroSize) {
    const char* data = "Hello";
    basic_string_view<char> sv(data, 0);
    sv.remove_suffix(0);
    EXPECT_EQ(sv.data(), data);
    EXPECT_EQ(sv.size(), 0u);
    EXPECT_EQ(sv.begin(), sv.end());
}

TEST(BasicStringView, SpaceshipCategory) {
    using sv_t = basic_string_view<char>;
    static_assert(std::same_as<decltype(std::declval<sv_t>() <=> std::declval<sv_t>()), std::strong_ordering>);
    static_assert(std::totally_ordered<sv_t>);
    static_assert(std::three_way_comparable<sv_t, std::strong_ordering>);
}

TEST(BasicStringView, SpaceshipResults) {
    basic_string_view<char> a("abc"), b("abd"), c("abc"), d("abcd"), e;
    EXPECT_EQ(a <=> b, std::strong_ordering::less);
    EXPECT_EQ(b <=> a, std::strong_ordering::greater);
    EXPECT_EQ(a <=> c, std::strong_ordering::equal);
    EXPECT_EQ(a <=> d, std::strong_ordering::less);
    EXPECT_EQ(e <=> a, std::strong_ordering::less);
    EXPECT_EQ(e <=> e, std::strong_ordering::equal);
}

TEST(BasicStringView, RelationalOperators) {
    basic_string_view<char> a("abc"), b("abd"), c("abc");
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= c);
    EXPECT_TRUE(a >= c);
    EXPECT_FALSE(a > b);
    EXPECT_TRUE(a != b);
}

TEST(BasicStringView, RelationalWithConvertibleOperands) {
    basic_string_view<char> sv("abc");
    std::string s{"abd"};
    // Right side is converted using string-like ctor
    EXPECT_TRUE(sv < s);
    // Left side is also converted (hidden friend)
    EXPECT_TRUE("abb" < sv);
    EXPECT_TRUE(s > sv);
}

TEST(BasicStringView, UnsignedCharOrdering) {
    // char_traits<char> compares chars as if they were unsigned char:
    // so '\x80' > 'a' even on platforms where char is signed
    basic_string_view<char> high("\x80"), low("a");
    EXPECT_TRUE(high > low);
}

TEST(BasicStringView, SpaceshipWideAndUtf8) {
    basic_string_view<wchar_t> w1(L"abc"), w2(L"abd");
    EXPECT_TRUE(w1 < w2);
    basic_string_view<char8_t> u1(u8"a"), u2(u8"\u00e7"); // ç
    EXPECT_TRUE(u1 < u2);
}

TEST(BasicStringView, SpaceshipCompileTime) {
    constexpr basic_string_view<char> a("abc"), b("abd");
    static_assert((a <=> b) < 0);
    static_assert(a < b && b > a && a <= a && !(a >= b));
}

TEST(BasicStringView, SortsLikeStdStringView) {
    std::vector<std::string> words { "pear", "apple", "\xC3\xA7" "ay", "Zebra", "app", ""};
    std::vector<string_view> mine(words.begin(), words.end());
    std::vector<std::string_view> ref(words.begin(), words.end());
    std::ranges::sort(mine);
    std::ranges::sort(ref);
    ASSERT_EQ(mine.size(), ref.size());
    for (std::size_t i = 0; i < ref.size(); ++i)
        EXPECT_EQ(to_string(mine[i]), std::string(ref[i]));
}

TEST(BasicStringView, StringLikeTraitsMustMatch) {
    // A type with a different traits_type is not string-like for std::char_traits<char>
    static_assert(!detail::string_like<custom_string_other_traits, std::char_traits<char>>);
    static_assert(!std::is_constructible_v<basic_string_view<char>, custom_string_other_traits>);

    // std::string_view uses std::char_traits<char>, so it converts
    static_assert(detail::string_like<std::string_view, std::char_traits<char>>);
    static_assert(std::is_constructible_v<basic_string_view<char>, std::string_view>);

    // A view with different traits does not accept std::string
    struct other_traits : std::char_traits<char> {};
    using other_view = basic_string_view<char, other_traits>;
    static_assert(!std::is_constructible_v<other_view, std::string>);

    // Views with different traits do not convert to each other
    static_assert(!std::is_convertible_v<basic_string_view<char>, other_view>);
    static_assert(!std::is_convertible_v<other_view, basic_string_view<char>>);

    // Null-terminated strings and (pointer, size) still work
    static_assert(std::is_constructible_v<other_view, const char*>);
    static_assert(std::is_constructible_v<other_view, const char*, std::size_t>);
}

namespace {
    // ASCII-only, constexpr friendly, case-insensitive traits
    struct ci_traits : std::char_traits<char> {
        // "A" and "a" are equal but not substitutable
        using comparison_category = std::weak_ordering;

        static constexpr char fold(char c) noexcept {
            return (c >= 'a' && c <= 'z') ? static_cast<char> (c - 'a' + 'A') : c; 
        }
        static constexpr bool eq(char a, char b) noexcept { 
            return fold(a) == fold(b); 
        }
        static constexpr bool lt(char a, char b) noexcept { 
            return static_cast<unsigned char>(fold(a)) < static_cast<unsigned char>(fold(b));
        }
        static constexpr int compare(const char* p, const char* q, std::size_t n) noexcept {
            for (std::size_t i = 0; i < n; ++i) {
                if (lt(p[i], q[i])) return -1;
                if (lt(q[i], p[i])) return 1;
            }
            return 0;
        }
    };

    struct no_category_traits {
        using char_type = char;
        static constexpr std::size_t length(const char* s) noexcept {
            return std::char_traits<char_type>::length(s);
        }
        static constexpr int compare(const char* p, const char* q, std::size_t n) noexcept {
            return std::char_traits<char_type>::compare(p, q, n);
        }
    };
}

TEST(BasicStringView, ComparisonCategoryDefaultsToWeak) {
    static_assert(std::same_as<detail::traits_comparison_category_t<no_category_traits>, std::weak_ordering>);
    using nc_view = basic_string_view<char, no_category_traits>;
    static_assert(std::same_as<decltype(std::declval<nc_view>() <=> std::declval<nc_view>()), std::weak_ordering>);
    nc_view a("abc"), b("abd");
    EXPECT_TRUE(a < b);
    EXPECT_EQ(a <=> a, std::weak_ordering::equivalent);
}

TEST(BasicStringView, CaseInsensitiveTraits) {
    static_assert(std::same_as<detail::traits_comparison_category_t<ci_traits>, std::weak_ordering>);
    using ci_view = basic_string_view<char, ci_traits>;
    ci_view hello("Hello"), HELLO("HELLO"), help("help");
    EXPECT_TRUE(hello == HELLO);
    EXPECT_EQ(hello <=> HELLO, std::weak_ordering::equivalent);
    EXPECT_EQ(hello <=> help, std::weak_ordering::less); // "HELLO" < "HELP"

    // Same text but not equal in default traits
    EXPECT_FALSE(basic_string_view<char>("Hello") == basic_string_view<char>("HELLO"));
}

TEST(BasicStringView, CaseInsensitiveCompileTime) {
    using ci_view = basic_string_view<char, ci_traits>;
    static_assert(ci_view("abc") == ci_view("ABC"));
    static_assert(ci_view("abc") < ci_view("ABD"));
}

TEST(BasicStringView, ToStringKeepsTraits) {
    using ci_view = basic_string_view<char, ci_traits>;
    ci_view v("Hello");
    auto s = to_string(v);
    static_assert(std::same_as<decltype(s), std::basic_string<char, ci_traits>>);
    EXPECT_EQ(s.size(), 5u);
    using ci_string = std::basic_string<char, ci_traits>;
    EXPECT_TRUE(s == ci_string("HELLO"));
    EXPECT_TRUE(to_string(ci_view()).empty());
}

TEST(BasicStringView, ToStringDefaultTraits) {
    static_assert(std::same_as<decltype(to_string(basic_string_view<char>())), std::string>);
    static_assert(std::same_as<decltype(to_string(basic_string_view<char8_t>())), std::u8string>);
    EXPECT_TRUE(to_string(basic_string_view<char8_t>(u8"\u00e7ay")) == u8"\u00e7ay");
    EXPECT_TRUE(to_string(basic_string_view<char32_t>(U"abc")) == U"abc");
}

namespace {
    struct uchar_traits {
        using char_type = unsigned char;
        using comparison_category = std::strong_ordering;

        static constexpr std::size_t length(const char_type* s) noexcept {
            std::size_t n = 0;
            while(s[n] != 0) ++n;
            return n;
        }
        static constexpr int compare(const unsigned char* p, const unsigned char* q,
                                std::size_t n) noexcept {
            for (std::size_t i = 0; i < n; ++i)
                if (p[i] != q[i]) return p[i] < q[i] ? -1 : 1;
            return 0;
        }
    };

    template <typename C, typename T>
    concept valid_view = requires { typename basic_string_view<C, T>; };
}

TEST(BasicStringView, CharLikeConcept) {
    static_assert(detail::char_like<char>);
    static_assert(detail::char_like<unsigned char>);
    static_assert(detail::char_like<int>);
    static_assert(detail::char_like<char*>);   // structurally fine, meaning comes from traits
    static_assert(!detail::char_like<const char>);
    static_assert(!detail::char_like<volatile char>);
    static_assert(!detail::char_like<char[3]>);
    static_assert(!detail::char_like<std::string>);   // not trivially copyable
}

TEST(BasicStringView, StdTraitsOnlyForStandardCharacters) {
    static_assert(valid_view<char, std::char_traits<char>>);
    static_assert(valid_view<char8_t, std::char_traits<char8_t>>);
    static_assert(!valid_view<unsigned char, std::char_traits<unsigned char>>);
    static_assert(!valid_view<int, std::char_traits<int>>);
    static_assert(!valid_view<char*, std::char_traits<char*>>);
    // cv-qualified and array types are rejected
    static_assert(!valid_view<const char, std::char_traits<char>>);
    static_assert(!valid_view<char[3], std::char_traits<char>>);
    // Traits for another character type
    static_assert(!valid_view<char, std::char_traits<wchar_t>>);
}

TEST(BasicStringView, ViewTraitsConcept) {
    static_assert(detail::view_traits<char, std::char_traits<char>>);
    static_assert(detail::view_traits<char, ci_traits>);
    static_assert(detail::view_traits<char, no_category_traits>);
    static_assert(detail::view_traits<unsigned char, uchar_traits>);
    static_assert(!detail::view_traits<char, uchar_traits>);
    struct no_length {
        using char_type = char;
        static int compare(const char*, const char*, std::size_t) {
            return 0;
        }
    };
    static_assert(!detail::view_traits<char, no_length>);
    static_assert(!valid_view<char, no_length>);
}

TEST(BasicStringView, UnsignedCharWithCustomTraits) {
    using u_view = basic_string_view<unsigned char, uchar_traits>;
    static constexpr unsigned char a[] = {0x41, 0x80, 0};
    static constexpr unsigned char b[] = {0x41, 0x7f, 0};
    u_view x(a), y(b);
    EXPECT_EQ(x.size(), 2u);
    EXPECT_TRUE(x > y);
    EXPECT_EQ(x <=> y, std::strong_ordering::greater);
    static_assert(u_view(a).size() == 2u);
    static_assert(u_view(a) > u_view(b));
}

TEST(BasicStringView, Aliases) {
    static_assert(std::same_as<string_view,    basic_string_view<char,     std::char_traits<char>>>);
    static_assert(std::same_as<wstring_view,   basic_string_view<wchar_t,  std::char_traits<wchar_t>>>);
    static_assert(std::same_as<u8string_view,  basic_string_view<char8_t,  std::char_traits<char8_t>>>);
    static_assert(std::same_as<u16string_view, basic_string_view<char16_t, std::char_traits<char16_t>>>);
    static_assert(std::same_as<u32string_view, basic_string_view<char32_t, std::char_traits<char32_t>>>);
    
    std::vector<string_view> v{"b", "a"};
    std::ranges::sort(v);
    EXPECT_EQ(v[0], string_view("a"));

    constexpr u16string_view u(u"abc");
    static_assert(u.size() == 3u);
}
