#pragma once

#include <string>
#include <locale>
#include <algorithm>

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
}
