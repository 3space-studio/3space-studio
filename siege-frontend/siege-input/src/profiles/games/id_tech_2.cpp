#include <array>
#include <utility>
#include <string_view>
#include <cstdint>
#include <algorithm>
#include "joystick_info.hpp"

// movement
constexpr static auto forward_on = "+forward"; 
constexpr static auto backward_on = "+back";
constexpr static auto move_left_on = "+moveleft"; // strafe left
constexpr static auto move_right_on = "+moveright"; // strafe right
constexpr static auto move_up_on = "+moveup"; // jump
constexpr static auto move_down_on = "+movedown"; // crouch
constexpr static auto speed_on = "+speed"; // walk/run

// aiming
constexpr static auto look_up_on = "+lookup"; 
constexpr static auto look_down_on = "+lookdown"; 
constexpr static auto look_left_on = "+left"; // turn left
constexpr static auto look_right_on = "+right"; // turn right

// shooting
constexpr static auto attack_on = "+attack";
constexpr static auto weapon_next = "weapnext";
constexpr static auto weapon_previous = "weaprev";


constexpr static auto melee_attack_on = "+melee-attack";

namespace playstation = siege::playstation;
using joystick_info = siege::joystick_info;
using button = siege::button;
using hat = siege::hat;
using axis = siege::axis;

constexpr static auto idtech_dual_stick_defaults = std::array<std::array<std::string_view, 2>, 14> {{
    {"left_y+", forward_on },
    {"left_y-", backward_on },
    {"left_x+", move_left_on },
    {"left_x-", move_right_on },
    {"right_y+", look_up_on },
    {"right_y-", look_down_on },
    {"right_x+", look_right_on },
    {"right_x-", look_left_on },
    {playstation::r2, attack_on },
    {playstation::l3, speed_on },
    {playstation::r3, melee_attack_on },
    {playstation::triangle, weapon_next},
    {playstation::circle, move_down_on},
    {playstation::cross, move_up_on}
}};



axis add_quake_1_axis_metadata(axis result)
{
    constexpr static auto axis_names = std::array<std::string_view, 6> {{
        "joyadvaxisx",
        "joyadvaxisy",
        "joyadvaxisz",
        "joyadvaxisr",
        "joyadvaxisu",
        "joyadvaxisv",
    }};

    if (result.index < axis_names.size())
    {
        result.meta_name_positive.emplace(axis_names[result.index]);
        result.meta_name_negative.emplace(axis_names[result.index]);
    }

    return result;
}

button add_quake_1_button_metadata(button result)
{
    constexpr static auto button_names = std::array<std::string_view, 15> {{
        "JOY1",
        "JOY2",
        "JOY3",
        "JOY4",
        "AUX5",
        "AUX6",
        "AUX7",
        "AUX8",
        "AUX9",
        "AUX10",
        "AUX11",
        "AUX12",
        "AUX13",
        "AUX14",
        "AUX15"
    }};

    if (result.index < button_names.size())
    {
        result.meta_name.emplace(button_names[result.index]);
    }

    return result;
}


hat add_quake_1_hat_metadata(hat result)
{
    if (result.index == 0)
    {
        result.meta_name_up = "AUX29";
        result.meta_name_down = "AUX31";
        result.meta_name_right = "AUX30";
        result.meta_name_left = "AUX32";
    }

    return result;
}


joystick_info add_quake_1_input_metadata(joystick_info info)
{
    std::transform(info.buttons.begin(), info.buttons.end(), info.buttons.begin(), add_quake_1_button_metadata);
    std::transform(info.axes.begin(), info.axes.end(), info.axes.begin(), add_quake_1_axis_metadata);
    std::transform(info.hats.begin(), info.hats.end(), info.hats.begin(), add_quake_1_hat_metadata);

    return info;
}

axis add_quake_2_axis_metadata(axis result)
{
    constexpr static auto axis_names = std::array<std::string_view, 6> {{
        "joy_advaxisx",
        "joy_advaxisy",
        "joy_advaxisz",
        "joy_advaxisr",
        "joy_advaxisu",
        "joy_advaxisv"
    }};


    if (result.index < axis_names.size())
    {
        result.meta_name_positive.emplace(axis_names[result.index]);
        result.meta_name_negative.emplace(axis_names[result.index]);
    }

    return result;
}

joystick_info add_quake_2_input_metadata(joystick_info info)
{
    std::transform(info.buttons.begin(), info.buttons.end(), info.buttons.begin(), add_quake_1_button_metadata);
    std::transform(info.axes.begin(), info.axes.end(), info.axes.begin(), add_quake_2_axis_metadata);
    std::transform(info.hats.begin(), info.hats.end(), info.hats.begin(), add_quake_1_hat_metadata);

    return info;
}


button add_quake_3_button_metadata(button result)
{
    constexpr static auto button_names = std::array<std::string_view, 15> {{
        "JOY1",
        "JOY2",
        "JOY3",
        "JOY4",
        "JOY5",
        "JOY6",
        "JOY7",
        "JOY8",
        "JOY9",
        "JOY10",
        "JOY11",
        "JOY12",
        "JOY13",
        "JOY14",
        "JOY15"
    }};

    if (result.index < button_names.size())
    {
        result.meta_name.emplace(button_names[result.index]);
    }

    return result;
}

axis add_quake_3_axis_metadata(axis result)
{
    if (result.index == 0)
    {
        result.meta_name_positive = "UPARROW";
        result.meta_name_negative = "DOWNARROW";
    }

    if (result.index == 1)
    {
        result.meta_name_positive = "RIGHTARROW";
        result.meta_name_negative = "LEFTARROW";
    }

    if (result.index == 2)
    {
        result.meta_name_positive = "JOY18";
        result.meta_name_negative = "JOY19";
    }

    if (result.index == 3)
    {
        result.meta_name_positive = "JOY17";
        result.meta_name_negative = "JOY16";
    }

    return result;
}

hat add_quake_3_hat_metadata(hat result)
{
    if (result.index == 0)
    {
        result.meta_name_up = "JOY24";
        result.meta_name_down = "JOY25";
        result.meta_name_right = "JOY26";
        result.meta_name_left = "JOY27";
    }

    return result;
}

joystick_info add_quake_3_input_metadata(joystick_info info)
{
    std::transform(info.buttons.begin(), info.buttons.end(), info.buttons.begin(), add_quake_3_button_metadata);
    std::transform(info.axes.begin(), info.axes.end(), info.axes.begin(), add_quake_3_axis_metadata);
    std::transform(info.hats.begin(), info.hats.end(), info.hats.begin(), add_quake_3_hat_metadata);

    return info;
}

std::string_view find_axis_index_for_action(std::string_view)
{
    constexpr static auto controller_button_mapping = std::array<std::array<std::string_view, 2>, 10> {{
        { forward_on, "1" },
        { backward_on, "1" },
        { look_up_on, "2" },
        { look_down_on, "2" },
        { move_left_on, "3" },
        { move_right_on, "3" },
        { look_left_on, "4" },
        { look_right_on, "4" },
        { move_up_on, "5" },
        { move_down_on, "5" }
    }};

    return "";
}

