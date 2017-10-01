#include "String.h"
#include "../doctest/parts/doctest_fwd.h"

namespace Common
{
  String& Common::ToLower(String& Str)
  {
    transform(Str.begin(), Str.end(), Str.begin(), [](char_t C) { return static_cast<char_t>(tolower(static_cast<int32_t>(C))); });

    return Str;
  }


  String Common::ToLower(const String& Str)
  {
    auto Copy(Str);
    ToLower(Copy);
    return Copy;
  }


  String& ToUpper(String& Str)
  {
    transform(Str.begin(), Str.end(), Str.begin(), [](char_t C) { return static_cast<char_t>(toupper(static_cast<int32_t>(C))); });

    return Str;
  }


  String ToUpper(const String& Str)
  {
    auto Copy(Str);
    ToUpper(Copy);
    return Copy;
  }

  String& TrimLeft(String& Str)
  {
    const auto It = std::find_if(Str.begin(), Str.end(), [](char_t C) { return !Detail::IsWhiteSpace(C); });

    if(It != Str.end())
      Str = Str.substr(std::distance(Str.begin(), It));

    return Str;
  }

  String TrimLeft(const String& Str)
  {
    auto Copy(Str);
    TrimLeft(Copy);
    return Copy;
  }

  String& TrimRight(String& Str)
  {
    const auto It = std::find_if(Str.rbegin(), Str.rend(), [](char_t C) { return !Detail::IsWhiteSpace(C); });

    if(It != Str.rend())
      Str = Str.substr(0, Str.length() - std::distance(Str.rbegin(), It));

    return Str;
  }

  String TrimRight(const String& Str)
  {
    auto Copy(Str);
    TrimRight(Copy);
    return Copy;
  }

  String& Trim(String& Str)
  {
    TrimLeft(Str);
    TrimRight(Str);
    return Str;
  }

  String Trim(const String& Str)
  {
    auto Copy(Str);
    TrimLeft(Copy);
    TrimRight(Copy);
    return Copy;
  }

  bool EqualNoCase(const String& A, const String& B)
  {
    return ToLower(A) == ToLower(B);
  }

  int32_t CompareNoCase(const String& A, const String& B)
  {
    return ToLower(A).compare(ToLower(B));
  }

  namespace Detail
  {
    bool IsWhiteSpace(char_t C)
    {
      for(const auto& W : WHITE_SPACE_CHARS)
        if(W == C)
          return true;

      return false;
    }
  }

  class ToUpperToLowerFixture
  {
  public:
    enum class EStringType
    {
      Upper,
      Lower,
      Mixed
    };

    String m_Str1 = UPPER;
    String m_Str2 = LOWER;
    String m_Str3 = MIXED;

    const String& GetConstString(EStringType Type) const
    {
      switch(Type)
      {
      case EStringType::Upper:
        {
          return m_Str1;
        }
      case EStringType::Lower:
        {
          return m_Str2;
        }
      case EStringType::Mixed:
        {
          return m_Str3;
        }
      default:
        return m_Str1;
      }
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    const char_t* GetConstRawString(EStringType Type) const
    {
      switch(Type)
      {
      case EStringType::Upper:
        {
          return UPPER;
        }
      case EStringType::Lower:
        {
          return LOWER;
        }
      case EStringType::Mixed:
        {
          return MIXED;
        }
      default:
        return _T("");
      }
    }

    char_t* RAW_UPPER = _T("UPPER");
    char_t* RAW_LOWER = _T("lower");
    char_t* RAW_MIXED = _T("MiXeD");

    static constexpr const char_t* UPPER = _T("UPPER");
    static constexpr const char_t* LOWER = _T("lower");
    static constexpr const char_t* MIXED = _T("MiXeD");
    static constexpr const char_t* TO_UPPER_EXPECTED_UPPER = _T("UPPER");
    static constexpr const char_t* TO_UPPER_EXPECTED_LOWER = _T("LOWER");
    static constexpr const char_t* TO_UPPER_EXPECTED_MIXED = _T("MIXED");
    static constexpr const char_t* TO_LOWER_EXPECTED_UPPER = _T("upper");
    static constexpr const char_t* TO_LOWER_EXPECTED_LOWER = _T("lower");
    static constexpr const char_t* TO_LOWER_EXPECTED_MIXED = _T("mixed");
  };


  TEST_CASE_FIXTURE(ToUpperToLowerFixture, "String ToUpper")
  {
    REQUIRE(ToUpper(m_Str1) == TO_UPPER_EXPECTED_UPPER);
    REQUIRE(ToUpper(m_Str2) == TO_UPPER_EXPECTED_LOWER);
    REQUIRE(ToUpper(m_Str3) == TO_UPPER_EXPECTED_MIXED);

    REQUIRE(ToUpper(GetConstString(EStringType::Upper)) == TO_UPPER_EXPECTED_UPPER);
    REQUIRE(ToUpper(GetConstString(EStringType::Lower)) == TO_UPPER_EXPECTED_LOWER);
    REQUIRE(ToUpper(GetConstString(EStringType::Mixed)) == TO_UPPER_EXPECTED_MIXED);

    REQUIRE(ToUpper(GetConstRawString(EStringType::Upper)) == TO_UPPER_EXPECTED_UPPER);
    REQUIRE(ToUpper(GetConstRawString(EStringType::Lower)) == TO_UPPER_EXPECTED_LOWER);
    REQUIRE(ToUpper(GetConstRawString(EStringType::Mixed)) == TO_UPPER_EXPECTED_MIXED);

    REQUIRE(ToUpper(RAW_UPPER) == TO_UPPER_EXPECTED_UPPER);
    REQUIRE(ToUpper(RAW_LOWER) == TO_UPPER_EXPECTED_LOWER);
    REQUIRE(ToUpper(RAW_MIXED) == TO_UPPER_EXPECTED_MIXED);

    REQUIRE(ToUpper(_T("UPPER")) == TO_UPPER_EXPECTED_UPPER);
    REQUIRE(ToUpper(_T("lower")) == TO_UPPER_EXPECTED_LOWER);
    REQUIRE(ToUpper(_T("MiXeD")) == TO_UPPER_EXPECTED_MIXED);
  }

  TEST_CASE_FIXTURE(ToUpperToLowerFixture, "String ToLower")
  {
    REQUIRE(ToLower(m_Str1) == TO_LOWER_EXPECTED_UPPER);
    REQUIRE(ToLower(m_Str2) == TO_LOWER_EXPECTED_LOWER);
    REQUIRE(ToLower(m_Str3) == TO_LOWER_EXPECTED_MIXED);

    REQUIRE(ToLower(GetConstString(EStringType::Upper)) == TO_LOWER_EXPECTED_UPPER);
    REQUIRE(ToLower(GetConstString(EStringType::Lower)) == TO_LOWER_EXPECTED_LOWER);
    REQUIRE(ToLower(GetConstString(EStringType::Mixed)) == TO_LOWER_EXPECTED_MIXED);

    REQUIRE(ToLower(GetConstRawString(EStringType::Upper)) == TO_LOWER_EXPECTED_UPPER);
    REQUIRE(ToLower(GetConstRawString(EStringType::Lower)) == TO_LOWER_EXPECTED_LOWER);
    REQUIRE(ToLower(GetConstRawString(EStringType::Mixed)) == TO_LOWER_EXPECTED_MIXED);

    REQUIRE(ToLower(RAW_UPPER) == TO_LOWER_EXPECTED_UPPER);
    REQUIRE(ToLower(RAW_LOWER) == TO_LOWER_EXPECTED_LOWER);
    REQUIRE(ToLower(RAW_MIXED) == TO_LOWER_EXPECTED_MIXED);

    REQUIRE(ToLower(_T("UPPER")) == TO_LOWER_EXPECTED_UPPER);
    REQUIRE(ToLower(_T("lower")) == TO_LOWER_EXPECTED_LOWER);
    REQUIRE(ToLower(_T("MiXeD")) == TO_LOWER_EXPECTED_MIXED);
  }

  TEST_CASE("String TrimLeft")
  {
    String Str1 = _T("NoWS");
    String Str2 = _T(" WS");
    String Str3 = _T("\tWS");
    String Str4 = _T("   \t  \tWS ");

    REQUIRE(TrimLeft(Str1) == _T("NoWS"));
    REQUIRE(TrimLeft(Str2) == _T("WS"));
    REQUIRE(TrimLeft(Str3) == _T("WS"));
    REQUIRE(TrimLeft(Str4) == _T("WS "));

    const String Str5 = _T("NoWS");
    const String Str6 = _T(" WS");
    const String Str7 = _T("\tWS");
    const String Str8 = _T("   \t  \tWS ");

    REQUIRE(TrimLeft(Str5) == _T("NoWS"));
    REQUIRE(TrimLeft(Str6) == _T("WS"));
    REQUIRE(TrimLeft(Str7) == _T("WS"));
    REQUIRE(TrimLeft(Str8) == _T("WS "));
  }

  TEST_CASE("String TrimRight")
  {
    String Str1 = _T("NoWS");
    String Str2 = _T(" WS ");
    String Str3 = _T("\tWS\t");
    String Str4 = _T("   \t  \tWS \t  ");

    REQUIRE(TrimRight(Str1) == _T("NoWS"));
    REQUIRE(TrimRight(Str2) == _T(" WS"));
    REQUIRE(TrimRight(Str3) == _T("\tWS"));
    REQUIRE(TrimRight(Str4) == _T("   \t  \tWS"));

    const String Str5 = _T("NoWS");
    const String Str6 = _T(" WS ");
    const String Str7 = _T("\tWS\t");
    const String Str8 = _T("   \t  \tWS \t  ");

    REQUIRE(TrimRight(Str5) == _T("NoWS"));
    REQUIRE(TrimRight(Str6) == _T(" WS"));
    REQUIRE(TrimRight(Str7) == _T("\tWS"));
    REQUIRE(TrimRight(Str8) == _T("   \t  \tWS"));
  }

  TEST_CASE("String Trim")
  {
    String Str1 = _T("NoWS");
    String Str2 = _T(" WS ");
    String Str3 = _T("\tWS\t");
    String Str4 = _T("   \t  \tWS \t  ");

    REQUIRE(Trim(Str1) == _T("NoWS"));
    REQUIRE(Trim(Str2) == _T("WS"));
    REQUIRE(Trim(Str3) == _T("WS"));
    REQUIRE(Trim(Str4) == _T("WS"));

    const String Str5 = _T("NoWS");
    const String Str6 = _T(" WS ");
    const String Str7 = _T("\tWS\t");
    const String Str8 = _T("   \t  \tWS \t  ");

    REQUIRE(Trim(Str5) == _T("NoWS"));
    REQUIRE(Trim(Str6) == _T("WS"));
    REQUIRE(Trim(Str7) == _T("WS"));
    REQUIRE(Trim(Str8) == _T("WS"));
  }

  TEST_CASE("String EqualNoCase")
  {
    const String Str1 = _T("Str");
    const String Str2 = _T("Str");
    const String Str3 = _T("str");
    const String Str4 = _T("StR");

    REQUIRE(EqualNoCase(Str1, Str1));
    REQUIRE(EqualNoCase(Str2, Str2));
    REQUIRE(EqualNoCase(Str3, Str3));
    REQUIRE(EqualNoCase(Str4, Str4));

    REQUIRE(EqualNoCase(Str1, Str2));
    REQUIRE(EqualNoCase(Str1, Str3));
    REQUIRE(EqualNoCase(Str1, Str4));

    REQUIRE(EqualNoCase(Str2, Str1));
    REQUIRE(EqualNoCase(Str2, Str3));
    REQUIRE(EqualNoCase(Str2, Str4));

    REQUIRE(EqualNoCase(Str3, Str1));
    REQUIRE(EqualNoCase(Str3, Str2));
    REQUIRE(EqualNoCase(Str3, Str4));

    REQUIRE(EqualNoCase(Str4, Str1));
    REQUIRE(EqualNoCase(Str4, Str2));
    REQUIRE(EqualNoCase(Str4, Str3));

    const String Str5 = _T("Ha");

    REQUIRE(!EqualNoCase(Str1, Str5));
    REQUIRE(!EqualNoCase(Str2, Str5));
    REQUIRE(!EqualNoCase(Str3, Str5));
    REQUIRE(!EqualNoCase(Str4, Str5));
  }

  TEST_CASE("String CompareNoCase")
  {
    const String Str1 = _T("Str");
    const String Str2 = _T("Str");
    const String Str3 = _T("str");
    const String Str4 = _T("StR");

    REQUIRE(CompareNoCase(Str1, Str1) == 0);
    REQUIRE(CompareNoCase(Str2, Str2) == 0);
    REQUIRE(CompareNoCase(Str3, Str3) == 0);
    REQUIRE(CompareNoCase(Str4, Str4) == 0);

    REQUIRE(CompareNoCase(Str1, Str2) == 0);
    REQUIRE(CompareNoCase(Str1, Str3) == 0);
    REQUIRE(CompareNoCase(Str1, Str4) == 0);

    REQUIRE(CompareNoCase(Str2, Str1) == 0);
    REQUIRE(CompareNoCase(Str2, Str3) == 0);
    REQUIRE(CompareNoCase(Str2, Str4) == 0);

    REQUIRE(CompareNoCase(Str3, Str1) == 0);
    REQUIRE(CompareNoCase(Str3, Str2) == 0);
    REQUIRE(CompareNoCase(Str3, Str4) == 0);

    REQUIRE(CompareNoCase(Str4, Str1) == 0);
    REQUIRE(CompareNoCase(Str4, Str2) == 0);
    REQUIRE(CompareNoCase(Str4, Str3) == 0);

    const String Str5 = _T("Ha");

    REQUIRE(CompareNoCase(Str1, Str5) == 1);
    REQUIRE(CompareNoCase(Str2, Str5) == 1);
    REQUIRE(CompareNoCase(Str3, Str5) == 1);
    REQUIRE(CompareNoCase(Str4, Str5) == 1);
  }
}

