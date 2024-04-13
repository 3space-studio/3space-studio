#ifndef SIEGEPLUGINHPP
#define SIEGEPLUGINHPP

#include <memory>
#include <filesystem>
#include <stdexcept>
#include <expected>
#include <string>
#include <vector>
#include <libloaderapi.h>
#include "win32_com_collection.hpp"

namespace siege
{
	class siege_plugin
	{
		std::unique_ptr<HINSTANCE__, void(*)(HINSTANCE)> plugin;

		HRESULT (__stdcall *GetSupportedExtensionsProc)(win32::com::IReadOnlyCollection** formats) = nullptr;
		HRESULT (__stdcall *GetSupportedFormatCategoriesProc)(LCID, win32::com::IReadOnlyCollection** formats) = nullptr;
		HRESULT (__stdcall *GetSupportedExtensionsForCategoryProc)(const wchar_t* category, win32::com::IReadOnlyCollection** formats) = nullptr;
		HRESULT (__stdcall *IsStreamSupportedProc)(::IStream* data) = nullptr;
		HRESULT (__stdcall *GetWindowClassForStreamProc)(::IStream* data, wchar_t**) = nullptr;

	public: 
		siege_plugin(std::filesystem::path plugin_path) : plugin(nullptr, [](auto handle) { assert(FreeLibrary(handle) == TRUE); })
		{
			if (!std::filesystem::exists(plugin_path))
			{
				throw std::invalid_argument("plugin_path");
			}

			auto temp = LoadLibraryW(plugin_path.c_str());

			if (!temp)
			{
				throw std::runtime_error("Could not load dll");
			}

			plugin.reset(temp);

			GetSupportedExtensionsProc = reinterpret_cast<decltype(GetSupportedExtensionsProc)>(::GetProcAddress(temp, "GetSupportedExtensions"));
			GetSupportedFormatCategoriesProc = reinterpret_cast<decltype(GetSupportedFormatCategoriesProc)>(::GetProcAddress(temp, "GetSupportedFormatCategories"));
			GetSupportedExtensionsForCategoryProc = reinterpret_cast<decltype(GetSupportedExtensionsForCategoryProc)>(::GetProcAddress(temp, "GetSupportedExtensionsForCategory"));
			IsStreamSupportedProc = reinterpret_cast<decltype(IsStreamSupportedProc)>(::GetProcAddress(temp, "IsStreamSupported"));
			GetWindowClassForStreamProc = reinterpret_cast<decltype(GetWindowClassForStreamProc)>(::GetProcAddress(temp, "GetWindowClassForStream"));
		

			if (!(GetSupportedExtensionsProc || GetSupportedFormatCategoriesProc || GetSupportedExtensionsForCategoryProc || IsStreamSupportedProc
				|| GetWindowClassForStreamProc
				))
			{
				throw std::runtime_error("Could not find module functions");
			}
		}

		std::vector<std::wstring> GetSupportedExtensions() const noexcept
		{
			std::vector<std::wstring> results;

			win32::com::IReadOnlyCollection* raw = nullptr;

			if (GetSupportedExtensionsProc(&raw) == S_OK)
			{
				for (auto& value : *raw)
				{
					results.emplace_back(std::wstring(value));
				}
			}

			return results;
		}

		std::vector<std::wstring> GetSupportedFormatCategories(LCID locale_id) const noexcept
		{
			std::vector<std::wstring> results;

			win32::com::IReadOnlyCollection* raw = nullptr;

			if (GetSupportedFormatCategoriesProc(locale_id, &raw) == S_OK)
			{
				for (auto& value : *raw)
				{
					results.emplace_back(value);
				}
			}

			return results;
		}

		std::vector<std::wstring> GetSupportedExtensionsForCategory(const std::wstring& category) const noexcept
		{
			std::vector<std::wstring> results;

			win32::com::IReadOnlyCollection* raw = nullptr;

			if (GetSupportedExtensionsForCategoryProc(category.c_str(), &raw) == S_OK)
			{
				for (auto& value : *raw)
				{
					results.emplace_back(value);
				}
			}

			return results;
		}

		bool IsStreamSupported(IStream& data) const noexcept
		{
			return IsStreamSupportedProc(&data) == S_OK;
		}

		std::wstring GetWindowClassForStream(IStream& data) const noexcept
		{
			wchar_t* result;

			if (GetWindowClassForStreamProc(&data, &result) == S_OK)
			{
				return result;
			}

			return L"";
		}
	};

}


#endif // !SIEGEPLUGINHPP
