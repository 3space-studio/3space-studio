#ifndef DARKSTARDTSCONVERTER_VIEW_FACTORY_HPP
#define DARKSTARDTSCONVERTER_VIEW_FACTORY_HPP

#include <istream>
#include <memory>
#include "graphics_view.hpp"
#include "default_view.hpp"
#include "resource/resource_explorer.hpp"
#include "3space-studio/utility.hpp"

using stream_validator = bool(std::basic_istream<std::byte>&);

using view_creator = std::unique_ptr<studio_view>(const studio::resource::file_info&, std::basic_istream<std::byte>&, const studio::resource::resource_explorer&);

class view_factory
{
public:
  void add_file_type(stream_validator* checker, view_creator* creator);

  void add_extension(std::string_view extension, stream_validator* checker);

  [[nodiscard]] std::vector<std::string_view> get_extensions() const;

  std::unique_ptr<studio_view> create_view(const studio::resource::file_info& file_info, std::basic_istream<std::byte>& stream, const studio::resource::resource_explorer& manager) const;

private:
  std::set<std::string> extensions;

  std::map<stream_validator*, view_creator*> creators;
  std::multimap<std::string_view, stream_validator*> validators;
};



#endif//DARKSTARDTSCONVERTER_VIEW_FACTORY_HPP
