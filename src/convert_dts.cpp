#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <set>
#include <sstream>
#include <filesystem>
#include <boost/endian/arithmetic.hpp>
#include "structures.hpp"
#include "json_boost.hpp"

namespace fs = std::filesystem;
namespace dts = darkstar::dts;

std::vector<std::byte> read_string(std::vector<std::byte>::iterator& iterator, std::size_t size)
{
    std::vector<std::byte> dest(size + 1, std::byte('\0'));

    // There is always an embedded \0 in the
    // file if the string length is less than 16 bytes.
    if (size < 16)
    {
        size++;
    }

    std::copy(iterator, iterator + size, dest.begin());
    std::advance(iterator, size);

    return dest;
}


template <typename destination_type>
std::vector<destination_type> read_vector(std::vector<std::byte>::iterator& iterator, std::size_t size)
{
    if (size == 0)
    {
        return {};
    }

    std::vector<destination_type> dest(size);
    std::copy(iterator, iterator + sizeof(destination_type) * size, reinterpret_cast<std::byte*>(&dest[0]));
    std::advance(iterator, sizeof(destination_type) * size);

    return dest;
}


template<std::size_t size>
std::array<std::byte, size> read(std::vector<std::byte>::iterator& iterator)
{
    std::array<std::byte, size> dest{};
    std::copy(iterator, iterator + sizeof(dest), dest.begin());
    std::advance(iterator, sizeof(dest));

    return dest;
}

template <typename destination_type>
destination_type read(std::vector<std::byte>::iterator& iterator)
{
    destination_type dest{};
    std::copy(iterator, iterator + sizeof(destination_type), reinterpret_cast<std::byte*>(&dest));
    std::advance(iterator, sizeof(destination_type));

    return dest;
}

dts::tag_header read_object_header(std::vector<std::byte>::iterator& cursor)
{
    dts::tag_header file_header =
            {
                    read<sizeof(dts::file_tag)>(cursor),
                    read<dts::file_info>(cursor)
            };

    if (file_header.tag != dts::pers_tag)
    {
        throw std::invalid_argument("The file provided does not have the appropriate tag to be a Darkstar DTS file.");
    }

    file_header.class_name = read_string(cursor, file_header.file_info.class_name_length);
    file_header.version = read<dts::version>(cursor);

    return file_header;
}

std::vector<fs::path> find_files(std::vector<std::string>& file_names)
{
    std::vector<fs::path> files;

    std::set<std::string> extensions;

    for(const auto& file_name : file_names)
    {
        if (file_name == "*")
        {
            extensions.insert(".dts");
            extensions.insert(".DTS");
            continue;
        }

        if (auto glob_index = file_name.rfind("*.", 0); glob_index == 0)
        {
            extensions.insert(file_name.substr(glob_index + 1));
            continue;
        }

        if (auto path = fs::current_path().append(file_name); fs::exists(path))
        {
            files.push_back(path);
        }
    }

    if (!extensions.empty())
    {
        for(auto& item : fs::recursive_directory_iterator(fs::current_path()))
        {
            if (item.is_regular_file())
            {
                for(auto& extension : extensions)
                {
                    if (item.path().filename().string().rfind(extension) != std::string::npos)
                    {
                        files.push_back(item.path());
                    }
                }
            }
        }
    }

    return files;
}

template <typename ShapeType>
void read_meshes(ShapeType& shape, dts::shape::v7::header& header, std::vector<std::byte>::iterator& cursor)
{
    shape.meshes.reserve(header.num_meshes);

    for (int i = 0; i < header.num_meshes; ++i)
    {
        auto mesh_tag_header = read_object_header(cursor);

        if (mesh_tag_header.version != 3)
        {
            throw std::invalid_argument("The mesh version was not version 3 as expected");
        }

        auto mesh_header = read<dts::mesh::v3::header>(cursor);

        dts::mesh_v3 mesh
                {
                        mesh_header,
                        read_vector<dts::mesh::v3::vertex>(cursor, mesh_header.num_verts),
                        read_vector<dts::mesh::v3::texture_vertex>(cursor, mesh_header.num_texture_verts),
                        read_vector<dts::mesh::v3::face>(cursor, mesh_header.num_faces),
                        read_vector<dts::mesh::v3::frame>(cursor, mesh_header.num_frames)
                };

        shape.meshes.push_back(mesh);
    }
}

template <typename ShapeType>
void read_materials(ShapeType& shape, dts::shape::v7::header& header, std::vector<std::byte>::iterator& cursor)
{
    if (auto has_material_list = read<dts::shape::v7::has_material_list_flag>(cursor); has_material_list == 1)
    {
        auto object_header = read_object_header(cursor);

        if (object_header.version == 3)
        {
            auto main_header = read<dts::material_list::v3::header>(cursor);

            shape.material_list =
                    dts::material_list_v3
                    {
                            main_header,
                            read_vector<dts::material_list::v3::material>(cursor, main_header.num_materials * main_header.num_details)
                    };
        }
        else if (object_header.version == 2)
        {
            auto main_header = read<dts::material_list::v3::header>(cursor);

            auto materials = read_vector<dts::material_list::v2::material>(cursor, main_header.num_materials * main_header.num_details);

            std::vector<dts::material_list::v3::material> upgraded_materials;
            upgraded_materials.reserve(materials.size());

            std::transform(materials.begin(), materials.end(), std::back_inserter(upgraded_materials),
                           [](const auto& old)
                           {
                               dts::material_list::v3::material new_mat = {0};
                               auto start = reinterpret_cast<const std::byte*>(&old);
                               std::copy(start, start + sizeof(old), reinterpret_cast<std::byte*>(&new_mat));
                               return new_mat;
                           });

            shape.material_list =
                    dts::material_list_v3
                    {
                            main_header,
                            upgraded_materials
                    };
        }

    }
}

dts::shape_v7 read_shape_v7(std::vector<std::byte>::iterator& cursor)
{
    auto header = read<dts::shape::v7::header>(cursor);

    dts::shape_v7 shape
            {
                    header,
                    read<dts::shape::v7::data>(cursor),
                    read_vector<dts::shape::v7::node>(cursor, header.num_nodes),
                    read_vector<dts::shape::v7::sequence>(cursor, header.num_sequences),
                    read_vector<dts::shape::v7::sub_sequence>(cursor, header.num_sub_sequences),
                    read_vector<dts::shape::v7::keyframe>(cursor, header.num_key_frames),
                    read_vector<dts::shape::v7::transform>(cursor, header.num_transforms),
                    read_vector<dts::shape::v7::name>(cursor, header.num_names),
                    read_vector<dts::shape::v7::object>(cursor, header.num_objects),
                    read_vector<dts::shape::v7::detail>(cursor, header.num_details),
                    read_vector<dts::shape::v7::transition>(cursor, header.num_transitions),
                    read_vector<dts::shape::v7::frame_trigger>(cursor, header.num_frame_triggers),
                    read<dts::shape::v7::footer>(cursor)
            };

    read_meshes(shape, header, cursor);

    read_materials(shape, header, cursor);

    return shape;
}

dts::shape_v6 read_shape_v6(std::vector<std::byte>::iterator& cursor)
{
    auto header = read<dts::shape::v7::header>(cursor);

    dts::shape_v6 shape
            {
                    header,
                    read<dts::shape::v7::data>(cursor),
                    read_vector<dts::shape::v7::node>(cursor, header.num_nodes),
                    read_vector<dts::shape::v7::sequence>(cursor, header.num_sequences),
                    read_vector<dts::shape::v7::sub_sequence>(cursor, header.num_sub_sequences),
                    read_vector<dts::shape::v7::keyframe>(cursor, header.num_key_frames),
                    read_vector<dts::shape::v6::transform>(cursor, header.num_transforms),
                    read_vector<dts::shape::v7::name>(cursor, header.num_names),
                    read_vector<dts::shape::v7::object>(cursor, header.num_objects),
                    read_vector<dts::shape::v7::detail>(cursor, header.num_details),
                    read_vector<dts::shape::v6::transition>(cursor, header.num_transitions),
                    read_vector<dts::shape::v7::frame_trigger>(cursor, header.num_frame_triggers),
                    read<dts::shape::v7::footer>(cursor)
            };

    read_meshes(shape, header, cursor);

    read_materials(shape, header, cursor);

    return shape;
}

template <typename ShapeType>
void convert_to_json(const std::filesystem::path& file_name, const ShapeType& shape)
{
    nlohmann::ordered_json shape_as_json = shape;

    auto new_file_name = file_name.string() + ".json";
    {
        std::ofstream someone_as_file(new_file_name, std::ios::trunc);
        someone_as_file << shape_as_json.dump(4);
    }

    {
        std::ifstream test_file(new_file_name);
        auto fresh_shape_json = nlohmann::json::parse(test_file);
        dts::shape_v7 fresh_shape = fresh_shape_json;

        std::cout << fresh_shape.header.num_meshes << '\n';
    }
}


int main(int argc, const char** argv)
{
    for (auto& file_name : find_files(std::vector<std::string>(argv + 1, argv + argc)))
    {
        try
        {
            auto file_size = fs::file_size(file_name);
            std::vector<std::byte> file_buffer(file_size);
            std::basic_ifstream<std::byte> input(file_name, std::ios::binary);

            input.read(&file_buffer[0], file_size);

            auto cursor = file_buffer.begin();

            dts::tag_header file_header = read_object_header(cursor);

            if (file_header.version == 7)
            {
                auto shape = read_shape_v7(cursor);
                convert_to_json(file_name, shape);
            }
            else if (file_header.version == 6)
            {
                auto shape = read_shape_v6(cursor);
                convert_to_json(file_name, shape);
            }
            else
            {
                std::stringstream error;
                error << file_name << " is DTS version " << file_header.version << " which is currently unsupported.";
                throw std::invalid_argument(error.str());
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << '\n';
        }
    }

    return 0;
}