#include <siege/platform/win/desktop/window_module.hpp>
#include <detours.h>
#include "shared.hpp"


extern "C" {
using namespace std::literals;

// +connect IP:port
// +connect IP:0	
// HKCU\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers
// Name == full path
// value == WINXPSP3 HIGHDPIAWARE

constexpr static std::array<std::string_view, 11> verification_strings = { {
  "GAME.DIC"sv,
  "[FINAL_DRIVE]"sv,
  "[GEAR_RATIOS]"sv,
  "gtrui.mnu"sv,
  "specialfx.tec"sv,
  "GTR_ONLINE_LOBBY"sv,
  "GTR_ONLINE_LOGIN"sv,
  "GTR_ONLINE_CDKEY"sv,
  "GARAGEONLINEPAGE"sv,
  "STEX.GTR"sv,
  "Net.dll"sv
} };


HRESULT executable_is_supported(const wchar_t* filename) noexcept
{
  return siege::executable_is_supported(filename, verification_strings);
}

static auto* TrueSetProcessAffinityMask = ::SetProcessAffinityMask;

BOOL WrappedSetProcessAffinityMask(
  HANDLE hProcess,
  DWORD_PTR dwProcessAffinityMask)
{
  if (dwProcessAffinityMask == 1)
  {
    return TRUE;
  }

  return TrueSetProcessAffinityMask(hProcess, dwProcessAffinityMask);
}

#include "dll-main.hpp"

}