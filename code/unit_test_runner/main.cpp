#include <cstdlib>
#include <windows.h>
#include <filesystem>
#include <iostream>
#include <tchar.h>

#include "../doctest/runner.h"

#ifdef _MSC_VER
namespace std
{
  namespace filesystem = experimental::filesystem;
}
#endif

constexpr const TCHAR* ARG_AUTO_1 = _T("-autoclose");
constexpr const TCHAR* ARG_AUTO_2 = _T("/autoclose");

struct Settings
{
  bool m_AutoClose = false;
};

int main(int argc, TCHAR* argv[])
{
  Settings Settings;
  auto Result = EXIT_SUCCESS;
  std::filesystem::path CurrentPath(argv[0]);
  CurrentPath = CurrentPath.remove_filename();

  for(auto i = 0; i < argc; i++)
  {
    if(_tcsicmp(ARG_AUTO_1, argv[i]) == 0 || _tcsicmp(ARG_AUTO_2, argv[i]) == 0)
      Settings.m_AutoClose = true;
  }

  for(const auto& Path : std::filesystem::directory_iterator(CurrentPath))
  {
    if(!is_regular_file(Path) || Path.path().extension() != _T(".dll"))
      continue;

    const auto Lib = LoadLibraryW(Path.path().c_str());
    if(Lib == NULL)
      continue;

    const auto Func = reinterpret_cast<UnitTestRun>(GetProcAddress(Lib, RUNNER_FUNC_NAME));
    if(Func == nullptr)
    {
      continue;
    }

    std::cout << _T("Running unit test in: ") << Path.path().filename() << _T("\n");
    if(Func(argc, argv) != 0)
      Result = EXIT_FAILURE;
    std::cout << _T("\n\n");
  }

  if(!Settings.m_AutoClose)
    getchar();

  return Result;
}

