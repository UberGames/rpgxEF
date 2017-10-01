#define DOCTEST_CONFIG_IMPLEMENT
#include "../doctest/doctest.h"
#include "../common_game/String.h"

int RunUnutTests(int argc, Common::char_t* argv[])
{
  doctest::Context Ctx(argc, argv);
  return Ctx.run();
}

