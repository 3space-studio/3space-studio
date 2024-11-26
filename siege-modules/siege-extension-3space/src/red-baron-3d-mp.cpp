#include <siege/platform/win/desktop/window_module.hpp>
#include <detours.h>
#include "shared.hpp"


extern "C" {
using namespace std::literals;

constexpr static std::array<std::string_view, 5> verification_strings = { {
  "RedBaronClass"sv,
  "Multiplayer"sv,
  "Baronmp.exe"sv,
  "baron.dml"sv,
  "MPString2.dat"sv,
} };


HRESULT executable_is_supported(const wchar_t* filename) noexcept
{
  return siege::executable_is_supported(filename, verification_strings);
}

}