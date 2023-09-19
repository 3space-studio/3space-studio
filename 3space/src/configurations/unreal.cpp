#include <limits>
#include "configurations/unreal.hpp"
#include "endian_arithmetic.hpp"

namespace studio::configurations::unreal
{
    namespace endian = boost::endian;

    namespace unreal_1
    {
        constexpr static auto end_line = std::string_view("\r\n");
        constexpr static auto separator = std::string_view("=");

        std::optional<game_config> load_config(std::istream& raw_data, std::size_t stream_size)
        {
            if (stream_size == 0)
            {
                return std::nullopt;
            }

            game_config result{};
            result.raw_data.reserve(stream_size);
            auto current_pos = raw_data.tellg();
            raw_data.read(result.raw_data.data(), result.raw_data.size());
            raw_data.seekg(current_pos, std::ios::beg);

            bool is_valid = result.raw_data.find("[") != std::string::npos &&
                            result.raw_data.find("]") != std::string::npos &&
                            result.raw_data.find("=") != std::string::npos
                            result.raw_data.find("=(") != std::string::npos;


            if (!is_valid)
            {
                return std::nullopt;
            }

            result.config_data.reserve(std::count(result.raw_data.begin(), result.raw_data.end(), end_line[0]));

            auto key_start = 0u;
            auto key_end = 0u;
            auto value_start = 0u;
            auto value_end = 0u;

            std::string_view group_key;

            static std::list<std::string> strings;
            static std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>> generated_keys;

            do
            {
                key_end = result.raw_data.find(end_line, key_start);

                if (key_end == std::string::npos)
                {
                    break;
                }

                auto key = std::string_view(result.raw_data.begin() + key_start, result.raw_data.begin() + key_end);

                auto group_key_start = key.find('[');
                auto group_key_end = key.find(']');

                if (group_key_start != std::string::npos && group_key_end != std::string::npos)
                {
                    group_key = std::string_view(result.raw_data.begin() + key_start + group_key_start, 
                                                result.raw_data.begin() + key_start + group_key_end);
                    key_start = key_end + end_line.size();
                    continue;                            
                }


                key_end = result.raw_data.find(separator, key_start);
                
                if (value_end == std::string::npos)
                {
                    break;
                }

                value_start = key_end + separator.size();
                value_end = result.raw_data.find(end_line, value_start);

                if (value_end == std::string::npos)
                {
                    break;
                }

                auto key = std::string_view(result.raw_data.begin() + key_start, result.raw_data.begin() + key_end);
                auto value = std::string_view(result.raw_data.begin() + value_start, result.raw_data.begin() + value_end);
                
                result.add(key, try_parse(value));

                if (value_end + end_line.size() > result.raw_data.size())
                {
                    break;
                }
                 
                key_start = value_end + end_line.size();
            }
            while (true);


            return result;
        }

        void save_config(std::istream&, const game_config&);
    }
}