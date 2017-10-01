#pragma once

#ifndef UNICODE
typedef int(*UnitTestRun)(int argc, char* argv[]);

constexpr const char* RUNNER_FUNC_NAME = "RunUnutTests";
#else
typedef int(*UnitTestRun)(int argc, wchar_t* argv[]);

constexpr const wchar_t* RUNNER_FUNC_NAME = L"RunUnutTests";
#endif
