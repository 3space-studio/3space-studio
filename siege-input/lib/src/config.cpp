#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <optional>
#include <SDL.h>

struct binding
{
  std::size_t value;
  SDL_GameControllerBindType type;
};

struct stick_indexes
{
  binding x;
  binding y;
  std::optional<binding> twist;
};

struct throttle_indexes
{
  binding y;
  std::optional<binding> mini_rudder;
};

inline SDL_GameControllerType from_string(std::string_view type)
{
  if (type == "axis")
  {
    return SDL_GameControllerType::SDL_CONTROLLER_BINDTYPE_AXIS;
  }

  if (type == "button")
  {
    return SDL_GameControllerType::SDL_CONTROLLER_BINDTYPE_BUTTON;
  }

  if (type == "hat")
  {
    return SDL_GameControllerType::SDL_CONTROLLER_BINDTYPE_HAT;
  }

  return SDL_GameControllerType::SDL_CONTROLLER_BINDTYPE_NONE;
}


auto parse_profile(std::filesystem::path file_name)
{
  std::ifstream profile_file(file_name);
  return nlohmann::json::parse(profile_file);
}

stick_indexes binding_for_primary_stick(const nlohmann::json& data)
{
  auto default_binding = [&]() {
    stick_indexes result{};

    if (data["numAxes"].get<int>() >= 2)
    {
      result.x.value = 0;
      result.x.type = SDL_GameControllerBindType::SDL_CONTROLLER_BINDTYPE_AXIS;
      result.y.value = 1;
      result.y.type = SDL_GameControllerBindType::SDL_CONTROLLER_BINDTYPE_AXIS;
    }

    return result;
  };

  if (!data.contains("bindings") &&
      !data["bindings"].contains("axes"))
  {
    return default_binding();
  }

  const auto& axes = data["bindings"]["axes"];

  auto parse_stick = [&](const nlohmann::json& stick) {
    if (!stick.contains("x") && !stick.contains("y"))
    {
      return default_binding();
    }

    stick_indexes result{};

    result.x.value = stick["x"]["index"].get<std::size_t>();
    result.x.type = from_string(stick["x"]["type"].get<std::string>());
    result.y.value = stick["y"]["index"].get<std::size_t>();
    result.y.type = from_string(stick["y"]["type"].get<std::string>());

    if (stick.contains("twist"))
    {
      result.twist.emplace({
        stick["twist"]["index"].get<std::size_t>(),
        from_string(stick["twist"]["type"].get<std::string>())
      });
    }
  };

  if (axes.contains("mainStick"))
  {
    return parse_stick(axes["mainStick"]);
  }

  if (axes.contains("analogueStick"))
  {
    return parse_stick(axes["analogueStick"]);
  }

  if (axes.contains("analogStick"))
  {
    return parse_stick(axes["analogStick"]);
  }

  return default_binding();
}

throttle_indexes binding_for_primary_throttle(const nlohmann::json& data)
{
  if (!data.contains("bindings") &&
      !data["bindings"].contains("axes"))
  {
    return {};
  }

  const auto& axes = data["bindings"]["axes"];

  auto parse_throttle = [&](const nlohmann::json& throttle) {
    throttle_indexes result{};

    result.y.value = throttle["y"]["index"].get<std::size_t>();
    result.y.type = from_string(throttle["y"]["type"].get<std::string>());

    if (throttle.contains("miniRudder"))
    {
      result.mini_rudder.emplace({
        throttle["miniRudder"]["index"].get<std::size_t>(),
        from_string(throttle["miniRudder"]["type"].get<std::string>())
      });
    }

    return result;
  };

  if (axes.contains("mainThrottle"))
  {
    auto result = parse_throttle(axes["mainThrottle"]);

    if (!result.mini_rudder.has_value() &&
        axes.contains("miniRudder") &&
        axes["miniRudder"].contains("location") &&
        axes["miniRudder"]["location"].contains("part") &&
        axes["miniRudder"]["location"]["part"] == "mainThrottle")
    {
      result.mini_rudder.emplace({
        axes["miniRudder"]["x"]["index"].get<std::size_t>(),
        from_string(axes["miniRudder"]["x"]["type"].get<std::string>())
      });
    }

    return result;
  }

  if (axes.contains("miniThrottle"))
  {
    return parse_throttle(axes["miniThrottle"]);
  }

  return {};
}

std::optional<binding> binding_for_primary_rudder(const nlohmann::json& data)
{
  auto result = binding_for_primary_stick(data).twist
          .value_or(binding_for_primary_throttle(data).mini_rudder);

  if (!data.contains("bindings") &&
      !data["bindings"].contains("axes"))
  {
    return result;
  }

  const auto& axes = data["bindings"]["axes"];

  if (!result.has_value() &&
      axes.contains("pedals") &&
    axes["pedals"].contains("rudder"))
  {
    return {
      axes["pedals"]["rudder"]["index"].get<std::size_t>(),
        from_string(axes["pedals"]["rudder"]["type"].get<std::string>())
    };
  }

  if (!result.has_value() &&
      axes.contains("externalPedals") &&
      axes["externalPedals"].contains("rudder"))
  {
    return {
      axes["externalPedals"]["rudder"]["index"].get<std::size_t>(),
      from_string(axes["externalPedals"]["rudder"]["type"].get<std::string>())
    };
  }

  if (!result.has_value() &&
      axes.contains("rudder"))
  {
    return {
      axes["rudder"]["index"].get<std::size_t>(),
      from_string(axes["rudder"]["type"].get<std::string>())
    };
  }

  return result;
}