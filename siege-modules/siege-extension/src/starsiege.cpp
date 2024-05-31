#include <cstring>
#include <cstdint>
#include <algorithm>
#include <array>
#include <unordered_set>
#include <utility>
#include <thread>
#include <string_view>
#include <fstream>
#include <siege/platform/win/core/file.hpp>
#include <siege/platform/win/desktop/window_module.hpp>
#include <siege/platform/win/desktop/window_impl.hpp>
#include <detours.h>
#include "ScriptDispatch.hpp"
#include "MessageHandler.hpp"

extern "C"
{
	#define DARKCALL __attribute__((regparm(3)))

	static DARKCALL char* (*ConsoleEval)(void*, std::int32_t, std::int32_t, const char**) = nullptr;

	constexpr std::array<std::array<std::pair<std::string_view, std::size_t>, 4>, 4> verification_strings = { {
	std::array<std::pair<std::string_view, std::size_t>, 4>{{
		{std::string_view("cls"), std::size_t(0x6fb741)},
		{std::string_view("trace"), std::size_t(0x6fb7af)},
		{std::string_view("Console::logBufferEnabled"), std::size_t(0x6fb7b5)},
		{std::string_view("Console::logMode"), std::size_t(0x6fb7fa)},
		}},
	std::array<std::pair<std::string_view, std::size_t>, 4>{{
		{std::string_view("cls"), std::size_t(0x6fe551)},
		{std::string_view("trace"), std::size_t(0x6fe5bf)},
		{std::string_view("Console::logBufferEnabled"), std::size_t(0x6fe5c5)},
		{std::string_view("Console::logMode"), std::size_t(0x6fe60a)},
		}},
	std::array<std::pair<std::string_view, std::size_t>, 4>{{
		{std::string_view("cls"), std::size_t(0x712cf9)},
		{std::string_view("trace"), std::size_t(0x712d67)},
		{std::string_view("Console::logBufferEnabled"), std::size_t(0x712d6d)},
		{std::string_view("Console::logMode"), std::size_t(0x712db2)},
		}},
	std::array<std::pair<std::string_view, std::size_t>, 4>{{
		{std::string_view("cls"), std::size_t(0x723169)},
		{std::string_view("trace"), std::size_t(0x7231d7)},
		{std::string_view("Console::logBufferEnabled"), std::size_t(0x7231dd)},
		{std::string_view("Console::logMode"), std::size_t(0x723222)},
		}}
	} };

	constexpr static std::array<std::pair<std::string_view, std::string_view>, 25> function_name_ranges{ {
		{std::string_view("icDefaultButtonAction"), std::string_view("icActionAllowed")},
		{std::string_view("dataStore"), std::string_view("dataRelease")},
		{std::string_view("defaultWeapons"), std::string_view("allowWeapon")},
		{std::string_view("goto"), std::string_view("violate")},
		{std::string_view("initializeServer"), std::string_view("isEqualIP")},
		{std::string_view("say"), std::string_view("flushChannel")},
		{std::string_view("dynDataWriteObject"), std::string_view("FlushPilots")},
		{std::string_view("ME::Create"), std::string_view("ME::RebuildCommandMap")},
		{std::string_view("loadObject"), std::string_view("getNextObject")},
		{std::string_view("HTMLOpen"), std::string_view("HTMLOpenAndGoWin")},
		{std::string_view("swapSurfaces"), std::string_view("isGfxDriver")},
		{std::string_view("newMovPlay"), std::string_view("pauseMovie")},
		{std::string_view("netStats"), std::string_view("net::kick")},
		{std::string_view("newRedbook"), std::string_view("rbSetPlayMode")},
		{std::string_view("newInputManager"), std::string_view("defineKey")},
		{std::string_view("simTreeCreate"), std::string_view("simTreeRegScript")},
		{std::string_view("newSfx"), std::string_view("sfxGetMaxBuffers")},
		{std::string_view("newToolWindow"), std::string_view("saveFileAs")},
		{std::string_view("newTerrain"), std::string_view("reCalcCRC")},
		{std::string_view("GuiEditMode"), std::string_view("windowsKeyboardDisable")},
		{std::string_view("LS::Create"), std::string_view("LS::parseCommands")},
		{std::string_view("ircConnect"), std::string_view("ircEcho")},
		{std::string_view("globeLines"), std::string_view("loadSky")},
		{std::string_view("MissionRegType"), std::string_view("missionUndoMoveRotate")},
		{std::string_view("cls"), std::string_view("trace")},
		} };

	constexpr static std::array<std::pair<std::string_view, std::string_view>, 13> variable_name_ranges{ {
		{std::string_view("$pref::softwareTranslucency"), std::string_view("$pref::canvasCursorTrapped")},
		{std::string_view("suspended"), std::string_view("SimGui::firstPreRender")},
		{std::string_view("Console::ForeRGB"), std::string_view("Console::LastLineTimeout")},
		{std::string_view("$pref::mipcap"), std::string_view("$OpenGL::AFK")},
		{std::string_view("GFXMetrics::EmittedPolys"), std::string_view("useLowRes3D")},
		{std::string_view("pref::sfx2DVolume"), std::string_view("pref::sfx3DVolume")},
		{std::string_view("dynDataWriteObject"), std::string_view("FlushPilots")},
		{std::string_view("GuiEdit::GridSnapX"), std::string_view("pref::politeGui")},
		{std::string_view("Console::logBufferEnabled"), std::string_view("Console::logMode")},
		{std::string_view("DNet::ShowStats"), std::string_view("DNet::PacketLoss")},
		{std::string_view("GameInfo::SpawnLimit"), std::string_view("GameInfo::TimeLimit")},
		{std::string_view("ITRMetrics::OutsideBits"), std::string_view("ITRMetrics::NumInteriorLinks")},
		{std::string_view("$server::Mission"), std::string_view("$server::TeamMassLimit")},
		} };

	
	HRESULT __stdcall ExecutableIsSupported(_In_ const wchar_t* filename) noexcept
	{
		if (filename == nullptr)
		{
			return E_POINTER;	
		}

		if (!std::filesystem::exists(filename))
		{
			return E_INVALIDARG;
		}


		try
		{
			win32::file filw(std::filesystem::path(filename), GENERIC_READ, FILE_SHARE_READ, std::nullopt, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);

			auto file_size = filw.GetFileSizeEx();
			auto mapping = filw.CreateFileMapping(std::nullopt, PAGE_READONLY, 0, 0, L"");

			if (mapping && file_size)
			{
				auto view = mapping->MapViewOfFile(FILE_MAP_READ, (std::size_t)file_size->QuadPart);
				std::string_view data((char*)view.get(), (std::size_t)file_size->QuadPart);

				if (data.empty())
				{
					return S_FALSE;
				}

				auto is_executable = data.find("MZ") == 0 && data.find("PE") != std::string_view::npos;

				bool has_all_verification_strings = is_executable && std::all_of(verification_strings[0].begin(), verification_strings[0].end(), [&](auto& item) {
					return data.find(item.first) != std::string_view::npos;
				});		

				bool has_all_functions = has_all_verification_strings && std::all_of(function_name_ranges.begin(), function_name_ranges.end(), [&](auto& item) {

					auto first_index = data.find(item.first);

					if (first_index != std::string_view::npos)
					{
						return data.find(item.second, first_index) != std::string_view::npos;
					}
					return false;
				});		

				bool has_all_variables = has_all_functions && std::all_of(variable_name_ranges.begin(), variable_name_ranges.end(), [&](auto& item) {

					auto first_index = data.find(item.first);

					if (first_index != std::string_view::npos)
					{
						return data.find(item.second, first_index) != std::string_view::npos;
					}
					return false;
				});		

				return has_all_variables ? S_OK : S_FALSE;
			}

			return S_FALSE;

		}
		catch(...)
		{
			return S_FALSE;
		}
	}

	inline void set_1000_exports()
	{
		ConsoleEval = (decltype(ConsoleEval))0x5d3d00;
	}

	inline void set_1002_exports()
	{
		ConsoleEval = (decltype(ConsoleEval))0x5d4dd8;
	}

	inline void set_1003_exports()
	{
		ConsoleEval = (decltype(ConsoleEval))0x5e2bbc;
	}

	inline void set_1004_exports()
	{
		ConsoleEval = (decltype(ConsoleEval))0x5e6460;
	}

	constexpr std::array<void(*)(), 4> export_functions = { {
			set_1000_exports,
			set_1002_exports,
			set_1003_exports,
			set_1004_exports
		} };


	static auto* TrueSetWindowsHookExA = SetWindowsHookExA;
	static auto* TrueAllocConsole = AllocConsole;
	static auto* TrueGetLogicalDrives = GetLogicalDrives;
	static auto* TrueGetDriveTypeA = GetDriveTypeA;
	static auto* TrueGetVolumeInformationA = GetVolumeInformationA;

	static std::string VirtualDriveLetter;
	constexpr static std::string_view StarsiegeDisc1 = "STARSIEGE1";
	constexpr static std::string_view StarsiegeDisc2 = "STARSIEGE2";
	constexpr static std::string_view StarsiegeBetaDisc = "STARSIEGE";

	BOOL WINAPI WrappedAllocConsole()
	{
		win32::com::init_com();

		return TrueAllocConsole();
	}

	HHOOK WINAPI WrappedSetWindowsHookExA(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId)
	{
		win32::com::init_com();

		if (dwThreadId == 0)
		{
			dwThreadId = ::GetCurrentThreadId();
		}

		return TrueSetWindowsHookExA(idHook, lpfn, hmod, dwThreadId);
	}

	DWORD WINAPI WrappedGetLogicalDrives()
	{
		auto result = TrueGetLogicalDrives();
		std::bitset<sizeof(DWORD) * 8> bits(result);

		int driveLetter = static_cast<int>('A');

		for (auto i = 2; i < bits.size(); ++i)
		{
			if (bits[i] == false)
			{
				driveLetter += i;
				bits[i] = true;

				if (VirtualDriveLetter.empty())
				{
					VirtualDriveLetter = static_cast<char>(driveLetter) + std::string(":\\");
				}
				break;
			}
		}


		return bits.to_ulong();
	}

	UINT WINAPI WrappedGetDriveTypeA(LPCSTR lpRootPathName)
	{
		if (lpRootPathName && VirtualDriveLetter == lpRootPathName)
		{
			return DRIVE_CDROM;
		}

		return TrueGetDriveTypeA(lpRootPathName);
	}

	BOOL WINAPI WrappedGetVolumeInformationA(
		LPCSTR lpRootPathName,
		LPSTR lpVolumeNameBuffer,
		DWORD nVolumeNameSize,
		LPDWORD lpVolumeSerialNumber,
		LPDWORD lpMaximumComponentLength,
		LPDWORD lpFileSystemFlags,
		LPSTR lpFileSystemNameBuffer,
		DWORD nFileSystemNameSize)
	{
		if (lpRootPathName && lpRootPathName == VirtualDriveLetter)
		{
			std::vector<char> data(nVolumeNameSize, '\0');
			std::copy(StarsiegeDisc2.begin(), StarsiegeDisc2.end(), data.begin());
			std::copy(data.begin(), data.end(), lpVolumeNameBuffer);
			return TRUE;
		}

		return TrueGetVolumeInformationA(lpRootPathName,
			lpVolumeNameBuffer,
			nVolumeNameSize,
			lpVolumeSerialNumber,
			lpMaximumComponentLength,
			lpFileSystemFlags,
			lpFileSystemNameBuffer,
			nFileSystemNameSize);
	}

	static std::array<std::pair<void**, void*>, 5> detour_functions{ {
  { &(void*&)TrueSetWindowsHookExA, WrappedSetWindowsHookExA },
  { &(void*&)TrueGetLogicalDrives, WrappedGetLogicalDrives },
  { &(void*&)TrueGetDriveTypeA, WrappedGetDriveTypeA },
  { &(void*&)TrueGetVolumeInformationA, WrappedGetVolumeInformationA },
  { &(void*&)TrueAllocConsole, WrappedAllocConsole } } };

	BOOL WINAPI DllMain(
		HINSTANCE hinstDLL,
		DWORD fdwReason,
		LPVOID lpvReserved) noexcept
	{
		if (DetourIsHelperProcess())
		{
			return TRUE;
		}

		if (fdwReason == DLL_PROCESS_ATTACH || fdwReason == DLL_PROCESS_DETACH)
		{
			auto app_module = win32::module_ref(::GetModuleHandleW(nullptr));

			auto value = app_module.GetProcAddress<std::uint32_t*>("DisableSiegeExtensionModule");

			if (value && *value == -1)
			{
				return TRUE;
			}
		}

		if (fdwReason == DLL_PROCESS_ATTACH || fdwReason == DLL_PROCESS_DETACH)
		{
			if (fdwReason == DLL_PROCESS_ATTACH)
			{
				int index = 0;
				try
				{
					auto app_module = win32::module_ref(::GetModuleHandleW(nullptr));

					std::unordered_set<std::string_view> functions;
					std::unordered_set<std::string_view> variables;

					bool module_is_valid = false;

					for (const auto& item : verification_strings)
					{
						win32::module_ref temp((void*)item[0].second);

						if (temp != app_module)
						{
							continue;
						}

						module_is_valid = std::all_of(item.begin(), item.end(), [](const auto& str) {
							return std::memcmp(str.first.data(), (void*)str.second, str.first.size()) == 0;
							});


						if (module_is_valid)
						{
							export_functions[index]();

							std::string_view string_section((const char*)ConsoleEval, 1024 * 1024 * 2);


							for (auto& pair : function_name_ranges)
							{
								auto first_index = string_section.find(pair.first.data(), 0, pair.first.size() + 1);

								if (first_index != std::string_view::npos)
								{
									auto second_index = string_section.find(pair.second.data(), first_index, pair.second.size() + 1);

									if (second_index != std::string_view::npos)
									{
										auto second_ptr = string_section.data() + second_index;
										auto end = second_ptr + std::strlen(second_ptr) + 1;

										for (auto start = string_section.data() + first_index; start != end; start += std::strlen(start) + 1)
										{
											std::string_view temp(start);

											if (temp.size() == 1)
											{
												continue;
											}

											if (!std::all_of(temp.begin(), temp.end(), [](auto c) { return std::isalnum(c) != 0; }))
											{
												break;
											}

											functions.emplace(temp);
										}
									}
								}
							}

							break;
						}
						index++;
					}

					if (!module_is_valid)
					{
						return FALSE;
					}

					DetourRestoreAfterWith();

					DetourTransactionBegin();
					DetourUpdateThread(GetCurrentThread());

					std::for_each(detour_functions.begin(), detour_functions.end(), [](auto& func) { DetourAttach(func.first, func.second); });

					DetourTransactionCommit();

					auto self = win32::window_module_ref(hinstDLL);
					auto atom = self.RegisterClassExW(win32::static_window_meta_class<siege::extension::MessageHandler>{});

					auto type_name = win32::type_name<siege::extension::MessageHandler>();

					auto host = std::make_unique<siege::extension::ScriptDispatch>(std::move(functions), std::move(variables), [](std::string_view eval_string) -> std::string_view {
						std::array<const char*, 2> args{ "eval", eval_string.data() };

						// Luckily this function is static and doesn't need the console instance object nor
						// an ID to identify the callback. It doesn't even check for "eval" and skips straight to the second argument.
						auto result = ConsoleEval(nullptr, 0, 2, args.data());

						if (result == nullptr)
						{
							return "";
						}


						return result;
						});

					// TODO register multiple script hosts
					// using the following convention:
					// siege::extension::starsiege::ScriptHost (first instance)
					// siege::extension::starsiege::ScriptHost.1 (alternative name for first instance)
					// siege::extension::starsiege::ScriptHost[0] (alternative name for first instance)
					// siege::extension::starsiege::ScriptHost.2 (second instance)
					// siege::extension::starsiege::ScriptHost[1] (second instance)
					if (auto message = self.CreateWindowExW(CREATESTRUCTW{
						.lpCreateParams = host.release(),
						.hwndParent = HWND_MESSAGE,
						.style = WS_CHILD,
						.lpszName = L"siege::extension::starsiege::ScriptHost",
						.lpszClass = win32::type_name<siege::extension::MessageHandler>().c_str()
						}); message)
					{
					}
				}
				catch (...)
				{
					return FALSE;
				}
			}
			else if (fdwReason == DLL_PROCESS_DETACH)
			{
				DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());

				std::for_each(detour_functions.begin(), detour_functions.end(), [](auto& func) { DetourDetach(func.first, func.second); });
				DetourTransactionCommit();

				auto window = ::FindWindowExW(HWND_MESSAGE, nullptr, win32::type_name<siege::extension::MessageHandler>().c_str(), L"siege::extension::starsiege::ScriptHost");
				::DestroyWindow(window);
				auto self = win32::window_module(hinstDLL);

				self.UnregisterClassW<siege::extension::MessageHandler>();
			}
		}

		return TRUE;
	}
}


