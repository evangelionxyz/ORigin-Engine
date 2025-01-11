// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <string>
#include <unordered_map>
#include <functional>

namespace origin {

class Themes
{
public:
    static void Init();

    static void ApplyTheme(const std::string &name);
    static void AddTheme(const std::string &name, const std::function<void()> &func);

    static void SetDefaultDark();
    static void SetMochaColors();
    static void SetGlassTheme();
    static void SetModernColors();
    static void SetMaterialYouColors();
    static void SetModernDark();
    static void SetBessDarkThemeColors();
    static void SetFluentUITheme();
    static void SetFluentUILightTheme();

    static std::string GetCurrentTheme() { return s_current_theme; }
    static std::unordered_map<std::string, std::function<void()>> GetThemes() { return s_themes_func_map; }

private:
    static std::string s_current_theme;
    static std::unordered_map<std::string, std::function<void()>> s_themes_func_map;
};

}