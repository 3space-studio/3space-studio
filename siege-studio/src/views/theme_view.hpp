#ifndef THEME_VIEW_HPP
#define THEME_VIEW_HPP

#include <siege/platform/win/desktop/window_factory.hpp>
#include <siege/platform/win/desktop/common_controls.hpp>
#include <siege/platform/win/desktop/theming.hpp>

#include <type_traits>

namespace siege::views
{
  struct theme_view final : win32::window_ref
    , win32::list_box::notifications
    , win32::list_view::notifications
    , win32::button::notifications
  {
    using win32::list_view::notifications::wm_notify;
    using win32::button::notifications::wm_notify;
    using win32::list_box::notifications::wm_draw_item;
    using win32::button::notifications::wm_draw_item;

    win32::window_ref theme_properties;
    win32::list_box options;

    win32::list_view control_settings;

    struct item_context
    {
      std::size_t item_index;
      std::wstring_view key;
      std::optional<COLORREF> value;
    };

    std::vector<item_context> color_items;

    struct
    {
      win32::button button;
      win32::combo_box combo_box;
      win32::combo_box_ex combo_box_ex;
      win32::edit edit;
      win32::header header;
      win32::list_box list_box;
      win32::list_view list_view;
      win32::menu menu;
      win32::scroll_bar scroll_bar;
      win32::static_control static_control;
      win32::tab_control tab_control;
      win32::tool_bar toolbar;
      win32::window window;
    } sample;

    std::map<std::wstring_view, std::wstring_view> control_labels = {
      { win32::button::class_name, L"Button" },
      { win32::combo_box::class_name, L"Combo Box" },
      { win32::combo_box_ex::class_name, L"Combo Box Ex" },
      { win32::edit::class_name, L"Edit" },
      { win32::header::class_name, L"Header" },
      { win32::list_box::class_name, L"List Box" },
      { win32::list_view::class_name, L"List View" },
      { win32::static_control::class_name, L"Static Control" },
      { win32::scroll_bar::class_name, L"Scroll Bar" },
      { win32::tab_control::class_name, L"Tab Control" },
      { win32::tree_view::class_name, L"Tree View" },

      { win32::tool_bar::class_name, L"Toolbar" },
      { L"Menu", L"Menu" },
      { L"Window", L"Window" }
    };

    std::map<std::wstring_view, std::wstring_view> property_labels = {
      { L"BkColor", L"Background Color" },
      { L"TextColor", L"Text Color" },
      { L"LineColor", L"Line Color" },
      { L"TextBkColor", L"Text Background Color" },
      { L"OutlineColor", L"Outline Color" },
      { L"BtnHighlightColor", L"Button Highlight Color" },
      { L"BtnShadowColor", L"Button Shadow Color" },
      { L"BtnFaceColor", L"Button Face Color" },
      { L"TextHighlightColor", L"Text Highlight Color" },
      { L"MarkColor", L"Mark Color" },
    };

    std::array<COLORREF, 16> colors{};

    // simple settings has preferred theme option (from system or user-defined)
    // simple settings has preferred accent color (from system or user-defined)
    // simple settings has theme selection (light, dark)
    // advanced allows theme settings to be changed
    // allows theme settings to be saved
    // theme settings changed per control type

    theme_view(win32::hwnd_t self, const CREATESTRUCTW& params) : win32::window_ref(self)
    {
      if (IsWindow((win32::hwnd_t)params.lpCreateParams))
      {
        theme_properties.reset((win32::hwnd_t)params.lpCreateParams);
      }
    }

    auto wm_create()
    {
      auto control_factory = win32::window_factory(ref());

      sample.button = *control_factory.CreateWindowExW<win32::button>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        .lpszName = L"Sample button" });

      sample.combo_box = *control_factory.CreateWindowExW<win32::combo_box>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,
        .lpszName = L"Sample combo box" });

      sample.combo_box_ex = *control_factory.CreateWindowExW<win32::combo_box_ex>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD | CBS_DROPDOWN,
        .lpszName = L"Sample combo box ex" });

      sample.edit = *control_factory.CreateWindowExW<win32::edit>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD });

      sample.header = *control_factory.CreateWindowExW<win32::header>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD });

      sample.header.InsertItem(-1, HDITEMW{ .mask = HDI_TEXT | HDI_WIDTH, .cxy = 30, .pszText = const_cast<wchar_t*>(L"Sample") });
      sample.header.InsertItem(-1, HDITEMW{ .mask = HDI_TEXT | HDI_WIDTH, .cxy = 30, .pszText = const_cast<wchar_t*>(L"Header"), .cchTextMax = 5 });
      sample.header.InsertItem(-1, HDITEMW{ .mask = HDI_TEXT | HDI_WIDTH, .cxy = 30, .pszText = const_cast<wchar_t*>(L"Test"), .cchTextMax = 5 });

      sample.list_box = *control_factory.CreateWindowExW<win32::list_box>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD });

      sample.list_view = *control_factory.CreateWindowExW<win32::list_view>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD });

      options = *control_factory.CreateWindowExW<win32::list_box>(::CREATESTRUCTW{
        .style = WS_VISIBLE | WS_CHILD | LBS_NOTIFY | LBS_HASSTRINGS });

      options.InsertString(-1, L"Simple");
      options.InsertString(-1, L"Advanced");
      options.SetCurrentSelection(0);
      ListBox_SetItemHeight(options, 0, options.GetItemHeight(0) * 2);
      control_settings = *control_factory.CreateWindowExW<win32::list_view>(::CREATESTRUCTW{
        .style = WS_CHILD | LVS_REPORT });

      control_settings.SetExtendedListViewStyle(LVS_EX_TRACKSELECT, LVS_EX_TRACKSELECT);
      control_settings.EnableGroupView(true);

      control_settings.InsertColumn(-1, LVCOLUMNW{
                                          .pszText = const_cast<wchar_t*>(L"Property"),
                                        });

      control_settings.InsertColumn(-1, LVCOLUMNW{
                                          .pszText = const_cast<wchar_t*>(L"Value"),
                                        });

      ListView_SetHoverTime(control_settings, 10);


      std::vector<std::wstring_view> property_names = [] {
        std::vector<std::wstring_view> results;
        results.reserve(32);
        std::copy(win32::properties::button::props.begin(), win32::properties::button::props.end(), std::back_inserter(results));
        std::copy(win32::properties::combo_box::props.begin(), win32::properties::combo_box::props.end(), std::back_inserter(results));
        std::copy(win32::properties::combo_box_ex::props.begin(), win32::properties::combo_box_ex::props.end(), std::back_inserter(results));
        std::copy(win32::properties::edit::props.begin(), win32::properties::edit::props.end(), std::back_inserter(results));
        std::copy(win32::properties::header::props.begin(), win32::properties::header::props.end(), std::back_inserter(results));
        std::copy(win32::properties::tree_view::props.begin(), win32::properties::tree_view::props.end(), std::back_inserter(results));
        std::copy(win32::properties::list_view::props.begin(), win32::properties::list_view::props.end(), std::back_inserter(results));
        std::copy(win32::properties::tool_bar::props.begin(), win32::properties::tool_bar::props.end(), std::back_inserter(results));
        std::copy(win32::properties::list_box::props.begin(), win32::properties::list_box::props.end(), std::back_inserter(results));
        std::copy(win32::properties::window::props.begin(), win32::properties::window::props.end(), std::back_inserter(results));
        std::copy(win32::properties::menu::props.begin(), win32::properties::menu::props.end(), std::back_inserter(results));
        std::copy(win32::properties::static_control::props.begin(), win32::properties::static_control::props.end(), std::back_inserter(results));
        std::copy(win32::properties::tab_control::props.begin(), win32::properties::tab_control::props.end(), std::back_inserter(results));
        return results;
      }();

      std::vector<win32::list_view_group> groups;
      groups.reserve(16);

      std::set<COLORREF> unique_colors;
      color_items.reserve(property_names.size());

      std::size_t item_index = 0;

      for (auto& name : property_names)
      {
        std::vector<win32::list_view_item> items;

        auto& item = color_items.emplace_back();
        item.item_index = item_index++;
        item.key = name;
        item.value = theme_properties.FindPropertyExW<COLORREF>(name);

        std::wstringstream stream;
        std::wstring property_value;

        if (item.value)
        {
          unique_colors.emplace(*item.value);

          stream << L"#";
          stream << std::setfill(L'0') << std::setw(2) << std::hex << GetRValue(*item.value);
          stream << std::setfill(L'0') << std::setw(2) << std::hex << GetGValue(*item.value);
          stream << std::setfill(L'0') << std::setw(2) << std::hex << GetBValue(*item.value);
          property_value = stream.str();
        }
        else
        {
          property_value = L"System Default";
        }

        auto separator = name.find(L'.');
        auto control_name = control_labels.at(name.substr(0, separator));
        auto property_name = property_labels.at(name.substr(separator + 1));

        auto existing_group = std::find_if(groups.begin(), groups.end(), [&](auto& item) {
          return item.text == control_name;
        });

        if (existing_group == groups.end())
        {
          win32::list_view_item item{ std::wstring(property_name) };
          item.mask = LVIF_TEXT | LVIF_GROUPID;
          item.sub_items.emplace_back(property_value);
          groups.emplace_back(std::wstring(control_name), std::vector<win32::list_view_item>{ std::move(item) });
        }
        else
        {
          auto& item = existing_group->items.emplace_back(std::wstring(property_name));

          item.mask = LVIF_TEXT | LVIF_GROUPID;
          item.sub_items.emplace_back(property_value);
        }
      }

      for (auto& group : groups)
      {
        group.state = LVGS_COLLAPSIBLE;
      }

      control_settings.InsertGroups(groups);

      auto copy_count = unique_colors.size() > colors.size() ? colors.size() : unique_colors.size();
      std::copy_n(unique_colors.begin(), copy_count, colors.begin());

      win32::apply_theme(theme_properties, sample.button);

      return 0;
    }

    std::map<std::wstring, COLORREF> hover_colors;

    win32::lresult_t wm_notify(win32::list_view, NMLVCUSTOMDRAW& custom_draw) override
    {
      if (custom_draw.nmcd.dwDrawStage == CDDS_PREPAINT)
      {
        return CDRF_NOTIFYITEMDRAW;
      }

      if (custom_draw.nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
      {
        return CDRF_NOTIFYSUBITEMDRAW;
      }

      if (custom_draw.nmcd.dwDrawStage == (CDDS_SUBITEM | CDDS_ITEMPREPAINT) && custom_draw.dwItemType == LVCDI_ITEM)
      {
        if (custom_draw.iSubItem == 0)
        {
          return CDRF_DODEFAULT;
        }

        std::wstring temp;
        temp.push_back((wchar_t)custom_draw.nmcd.dwItemSpec);
        temp.push_back((wchar_t)custom_draw.iSubItem);

        auto color = hover_colors.find(temp);

        if (color != hover_colors.end())
        {
          custom_draw.clrTextBk = color->second;
        }

        return CDRF_DODEFAULT;
      }

      return CDRF_DODEFAULT;
    }

    std::optional<win32::lresult_t> wm_command(win32::list_box hwndFrom, int code) override
    {
      if (code == LBN_SELCHANGE && hwndFrom == options)
      {
        ShowWindow(control_settings, options.GetCurrentSelection() == 1 ? SW_SHOW : SW_HIDE);
      }

      return std::nullopt;
    }

    std::optional<win32::lresult_t> wm_notify(win32::list_view, const NMLVSCROLL& notice) override
    {
      if (notice.hdr.code == LVN_ENDSCROLL)
      {
        auto client_size = this->GetClientSize();

        if (client_size)
        {
          auto min_width = client_size->cx / 12;
          auto left_size = SIZE{ .cx = min_width * 2, .cy = client_size->cy };
          auto middle_size = SIZE{ .cx = min_width * 4, .cy = client_size->cy };
          auto right_size = SIZE{ .cx = client_size->cx - middle_size.cx - left_size.cx, .cy = client_size->cy };
          resize_controls(left_size, right_size, middle_size);
        }
      }

      return std::nullopt;
    }

    std::optional<win32::lresult_t> wm_notify(win32::list_view hwndFrom, const NMLVDISPINFOW& message) override
    {
      if (message.hdr.code == LVN_BEGINLABELEDITW && hwndFrom == control_settings)
      {
        if (message.item.iSubItem == 0)
        {
          return FALSE;
        }
        return TRUE;
      }

      if (message.hdr.code == LVN_ENDLABELEDITW && hwndFrom == control_settings)
      {
        return TRUE;
      }

      return std::nullopt;
    }

    static UINT_PTR CALLBACK DialogColorHook(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
    {
      if (message == WM_INITDIALOG)
      {
        auto* item = (CHOOSECOLORW*)lParam;
        auto x = LOWORD(item->lCustData);
        auto y = HIWORD(item->lCustData);
        win32::window_ref(dialog).SetWindowPos(POINT{ x, y });
      }

      return 0;
    }

    std::wstring temp_text = std::wstring(255, L'\0');

    virtual std::optional<win32::lresult_t> wm_notify(win32::list_view, const NMITEMACTIVATE& notice)
    {
      POINT point;

      if (notice.hdr.code == NM_CLICK && ::GetCursorPos(&point) && ::ScreenToClient(control_settings, &point))
      {
        LVHITTESTINFO info{};
        info.pt = point;
        info.flags = LVHT_ONITEM | LVHT_EX_ONCONTENTS;
        ListView_SubItemHitTest(control_settings, &info);

        if (info.iSubItem && info.iItem != -1)
        {
          RECT item_rect{};
          ListView_GetSubItemRect(control_settings, info.iItem, info.iSubItem, LVIR_BOUNDS, &item_rect);
          ::ClientToScreen(control_settings, (POINT*)&item_rect);

          ListView_GetItemText(control_settings, info.iItem, info.iSubItem, temp_text.data(), temp_text.size());

          CHOOSECOLORW dialog{};
          dialog.lStructSize = sizeof(CHOOSECOLOR);
          dialog.hwndOwner = *this;
          dialog.lpCustColors = colors.data();
          dialog.Flags = CC_ENABLEHOOK | CC_FULLOPEN;

          LVITEMW item_info{ .mask = LVIF_PARAM, .iItem = info.iItem, .iSubItem = info.iSubItem };
          ListView_GetItem(control_settings, &info);

          auto context = std::find_if(color_items.begin(), color_items.end(), [&](auto& item) {
            return item.item_index == info.iItem;
          });

          if (context != color_items.end() && context->value)
          {
            dialog.Flags |= CC_RGBINIT;
            dialog.rgbResult = *context->value;
          }

          dialog.lpfnHook = DialogColorHook;
          dialog.lCustData = MAKELPARAM(item_rect.left, item_rect.top);

          if (::ChooseColorW(&dialog) && context != color_items.end())
          {
            std::wstringstream stream;
            std::wstring property_value;

            stream << L"#";
            stream << std::setfill(L'0') << std::setw(2) << std::hex << GetRValue(dialog.rgbResult);
            stream << std::setfill(L'0') << std::setw(2) << std::hex << GetGValue(dialog.rgbResult);
            stream << std::setfill(L'0') << std::setw(2) << std::hex << GetBValue(dialog.rgbResult);
            property_value = stream.str();

            ListView_SetItemText(control_settings, info.iItem, info.iSubItem, property_value.data(), property_value.size());

            context->value = dialog.rgbResult;
            theme_properties.SetPropW(context->key, dialog.rgbResult);
            win32::apply_theme(theme_properties, sample.button);
          }
        }
      }

      return std::nullopt;
    }

    std::optional<win32::lresult_t> wm_notify(win32::list_view, const NMHDR& notice) override
    {
      POINT point;
      if ((notice.code == NM_HOVER) && ::GetCursorPos(&point) && ::ScreenToClient(control_settings, &point))
      {
        LVHITTESTINFO info{};
        info.pt = point;
        info.flags = LVHT_ONITEM;
        ListView_SubItemHitTest(control_settings, &info);

        if (info.iSubItem && info.iItem != -1)
        {
          RECT item_rect{};
          for (auto& hover : hover_colors)
          {
            ListView_GetSubItemRect(control_settings, hover.first[0], hover.first[1], LVIR_BOUNDS, &item_rect);
            ::InvalidateRect(control_settings, &item_rect, TRUE);
          }

          hover_colors.clear();

          ListView_GetSubItemRect(control_settings, info.iItem, info.iSubItem, LVIR_BOUNDS, &item_rect);

          std::wstring temp;
          temp.push_back(info.iItem);
          temp.push_back(info.iSubItem);

          auto color = hover_colors.emplace(temp, 0x00aaffaa);
          ::InvalidateRect(control_settings, &item_rect, TRUE);
          ListView_SetHotItem(control_settings, info.iItem);
        }
      }
      return 0;
    }

    auto wm_size(std::size_t, SIZE client_size)
    {
      auto min_width = client_size.cx / 12;
      auto left_size = SIZE{ .cx = min_width * 2, .cy = client_size.cy };
      auto middle_size = SIZE{ .cx = min_width * 4, .cy = client_size.cy };
      auto right_size = SIZE{ .cx = client_size.cx - middle_size.cx - left_size.cx, .cy = client_size.cy };

      options.SetWindowPos(POINT{});
      options.SetWindowPos(left_size);

      control_settings.SetWindowPos(POINT{ .x = left_size.cx });
      control_settings.SetWindowPos(middle_size);

      auto column_count = control_settings.GetColumnCount();

      if (!column_count)
      {
        return 0;
      }

      auto column_width = middle_size.cx / column_count;

      for (auto i = 0u; i < column_count; ++i)
      {
        control_settings.SetColumnWidth(i, column_width);
      }

      resize_controls(left_size, right_size, middle_size);

      return 0;
    }

    void resize_controls(SIZE left_size, SIZE right_size, SIZE middle_size)
    {
      RECT temp{};

      for (auto i = 0; i < control_settings.GetGroupCount(); ++i)
      {
        ListView_GetGroupRect(control_settings, i + 1, LVGGR_GROUP, &temp);

        SIZE temp_size{ .cx = right_size.cx, .cy = temp.bottom - temp.top };
        POINT temp_point{ .x = middle_size.cx + left_size.cx, .y = temp.top };

        auto set_pos = [&](auto& window) {
          window.SetWindowPos(temp_size);
          window.SetWindowPos(temp_point);
        };

        if (i == 0)
        {
          set_pos(sample.button);
        }

        if (i == 1)
        {
          set_pos(sample.combo_box);
        }

        if (i == 2)
        {
          set_pos(sample.combo_box_ex);
        }

        if (i == 3)
        {
          set_pos(sample.edit);
        }

        if (i == 4)
        {
          set_pos(sample.header);
        }

        if (i == 5)
        {
          set_pos(sample.list_box);
        }

        if (i == 6)
        {
          set_pos(sample.list_view);
        }

        if (i == 7)
        {
        }

        if (i == 8)
        {
        }
      }
    }

    std::optional<win32::lresult_t> wm_setting_change(win32::setting_change_message message)
    {
      return std::nullopt;
    }
  };
}// namespace siege::views

#endif