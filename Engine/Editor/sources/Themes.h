// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <string>
#include <unordered_map>
#include <functional>

namespace origin {

class Themes
{
public:
    Themes();

    void ApplyTheme(const std::string &name);
    void AddTheme(const std::string &name, const std::function<void()> &func);

    void SetDefaultDark();
    void SetMochaColors();
    void SetGlassTheme();
    void SetModernColors();
    void SetMaterialYouColors();
    void SetModernDark();
    void SetBessDarkThemeColors();
    void SetFluentUITheme();
    void SetFluentUILightTheme();

    const std::string GetCurrentTheme() { return m_CurrentTheme; }
    const std::unordered_map<std::string, std::function<void()>> GetThemes() { return m_Themes; }

private:
    std::string m_CurrentTheme;
    std::unordered_map<std::string, std::function<void()>> m_Themes;
};

}