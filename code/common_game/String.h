#pragma once

#include <iostream>
#include <sstream>
#include <array>
#include <initializer_list>
#include <iterator>
#include <iomanip>
#include <cctype>
#include <optional>

#if defined (UNICODE)
#define _T(X) L##X
#else
#define _T(X) X
#endif

namespace Common
{
#if defined (UNICODE)
  using char_t = wchar_t;
#else
  using char_t = char;
#endif

  using String = std::basic_string<char_t>;
  using StringView = std::basic_string_view<char_t>;
  using StringStream = std::basic_stringstream<char_t>;

  String& ToLower(String& Str);
  String ToLower(const String& Str);

  String& ToUpper(String& Str);
  String ToUpper(const String& Str);

  namespace Detail
  {
    static constexpr const char_t WHITE_SPACE_CHARS[2] = { _T(' '), _T('\t') };

    bool IsWhiteSpace(char_t C);
  }

  String& TrimLeft(String& Str);
  String TrimLeft(const String& Str);

  String& TrimRight(String& Str);
  String TrimRight(const String& Str);

  String& Trim(String& Str);
  String Trim(const String& Str);

  bool EqualNoCase(const String& A, const String& B);

  int32_t CompareNoCase(const String& A, const String& B);

  template<typename T, typename = std::enable_if_t<std::is_pointer<T>::value && !std::is_convertible<T, const char*>::value && !std::is_convertible<T, const wchar_t*>::value>>
  String ToString(T Value)
  {
    StringStream Stringstream;
    Stringstream << static_cast<const void*>(Value);
    return Stringstream.str();
  }

  inline String ToString(String Value) { return Value; }

  template<typename T, typename = std::enable_if_t<std::is_floating_point<T>::value>>
  String ToString(T Value, std::optional<int32_t> Precision = std::nullopt, bool Fixed = false, bool Scientific = false, std::optional<int32_t>Width = std::nullopt)
  {
    StringStream Stringstream;

    if(Precision)
      Stringstream << std::setprecision(*Precision);

    if(Fixed)
      Stringstream << std::fixed;

    if(Scientific)
      Stringstream << std::scientific;

    if(Width)
      Stringstream << std::setw(*Width);

    Stringstream << Value;
    return Stringstream.str();
  }

  template <typename T, typename = std::enable_if_t<std::is_same<T, bool>::value>>
  String ToString(T Value, bool Alpha = false)
  {
    StringStream Stringstream;
    if(Alpha)
      Stringstream << std::boolalpha;
    Stringstream << Value;
    return Stringstream.str();
  }

  enum class EIntegralFormat
  {
    Decimal,
    Hexadecimal,
    Octal
  };

  template<typename T, typename = std::enable_if_t<std::is_integral<T>::value && !std::is_same<T, bool>::value>>
  String ToString(T Value, EIntegralFormat Format = EIntegralFormat::Decimal, std::optional<int32_t> Width = std::nullopt, std::optional<String::value_type> Fill = std::nullopt)
  {
    StringStream StringStream;

    switch(Format)
    {
    case EIntegralFormat::Hexadecimal:
      {
        StringStream << std::hex;
        break;
      }
    case EIntegralFormat::Octal:
      {
        StringStream << std::oct;
        break;
      }
    case EIntegralFormat::Decimal:
    default:
      break;
    }

    if(Width)
      StringStream << std::setw(*Width);

    if(Fill)
      StringStream << std::setfill(*Fill);

    StringStream << Value;
    return StringStream.str();
  }

  template<typename T, typename = std::enable_if_t<!std::is_integral<T>::value && !std::is_floating_point<T>::value && !std::is_pointer<T>::value>, typename = void>
  String ToString(T Value)
  {
    StringStream StringStream;
    StringStream << Value;
    return StringStream.str();
  }

  template<typename MatchType, typename RepType, typename = std::enable_if_t<std::is_convertible<MatchType, String>::value && std::is_convertible<RepType, String>::value>>
  String& ReplaceAll(String& Str, MatchType&& Match, RepType&& Rep)
  {
    String M{ Match };

    if(M.empty())
      return Str;

    String R{ Rep };
    String::size_type Index = 0;
    while(true)
    {
      Index = Str.find(M, Index);
      if(Index == String::npos)
        break;
      Str.replace(Index, M.size(), R);
      Index += R.size();
    }

    return Str;
  }

  template<typename MatchType, typename RepType, typename = std::enable_if_t<std::is_convertible<MatchType, String>::value && std::is_convertible<RepType, String>::value>>
  String ReplaceAll(const String& Str, MatchType&& Match, RepType&& Rep)
  {
    auto Copy(Str);
    return ReplaceAll(Copy, std::forward<MatchType>(Match), std::forward<RepType>(Rep));
  }

  template<typename... ArgsT>
  String Format(const String& Format, ArgsT const & ... Args)
  {
    auto Copy(Format);
    std::array<String, sizeof...(Args)> StringArgs = { {ToString(Args)...} };

    for(auto i = 0; i < sizeof...(Args); i++)
    {
      ReplaceAll(Copy, _T("%") + ToString(i), StringArgs[i]);
    }

    return Copy;
  }
}
