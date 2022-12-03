// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "EditorTheme.h"
#include "IconFontAwesome.h"

namespace Origin
{
	void EditorTheme::SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor)
	{
		BackGroundColor = backGroundColor;
		TextColor = textColor;
		MainColor = mainColor;
		MainAccentColor = mainAccentColor;
		HighlightColor = highlightColor;
	}

	void EditorTheme::ApplyDarkPurple()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMono-SemiBold.ttf", 16);

		// Origin Gui Theme Settings
		ImGuiStyle& style = ImGui::GetStyle();

		// frame
		style.FrameRounding = 1;
		style.GrabRounding = 1;
		style.FramePadding.x = 1;
		style.FramePadding.y = 1;

		// window
		style.WindowMenuButtonPosition = 0;
		style.WindowTitleAlign.x = 0.5;
		style.WindowTitleAlign.y = 0.5;
		style.WindowRounding = 0;
		style.WindowPadding = ImVec2(3.0f, 3.0f);

		ImVec4* colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_Text] = GetColor(TextColor);
		colors[ImGuiCol_TextDisabled] = Disabled(colors[ImGuiCol_Text]);
		colors[ImGuiCol_WindowBg] = GetColor(BackGroundColor);
		colors[ImGuiCol_ChildBg] = GetColor(Black, Alpha20);
		colors[ImGuiCol_PopupBg] = GetColor(BackGroundColor, Alpha90);
		colors[ImGuiCol_Border] = Lighten(GetColor(BackGroundColor), 0.4f);
		colors[ImGuiCol_BorderShadow] = GetColor(Black);
		colors[ImGuiCol_FrameBg] = GetColor(MainAccentColor, Alpha40);
		colors[ImGuiCol_FrameBgHovered] = Hovered(colors[ImGuiCol_FrameBg]);
		colors[ImGuiCol_FrameBgActive] = Active(colors[ImGuiCol_FrameBg]);
		colors[ImGuiCol_TitleBg] = GetColor(BackGroundColor, Alpha80);
		colors[ImGuiCol_TitleBgActive] = Active(colors[ImGuiCol_TitleBg]);
		colors[ImGuiCol_TitleBgCollapsed] = Collapsed(colors[ImGuiCol_TitleBg]);
		colors[ImGuiCol_MenuBarBg] = Darken(GetColor(BackGroundColor), 0.2f);
		colors[ImGuiCol_ScrollbarBg] = Lighten(GetColor(BackGroundColor, Alpha50), 0.4f);
		colors[ImGuiCol_ScrollbarGrab] = Lighten(GetColor(BackGroundColor), 0.3f);
		colors[ImGuiCol_ScrollbarGrabHovered] = Hovered(colors[ImGuiCol_ScrollbarGrab]);
		colors[ImGuiCol_ScrollbarGrabActive] = Active(colors[ImGuiCol_ScrollbarGrab]);
		colors[ImGuiCol_CheckMark] = GetColor(HighlightColor);
		colors[ImGuiCol_SliderGrab] = GetColor(HighlightColor);
		colors[ImGuiCol_SliderGrabActive] = Active(colors[ImGuiCol_SliderGrab]);
		colors[ImGuiCol_Button] = GetColor(MainColor, Alpha80);
		colors[ImGuiCol_ButtonHovered] = Hovered(colors[ImGuiCol_Button]);
		colors[ImGuiCol_ButtonActive] = Active(colors[ImGuiCol_Button]);
		colors[ImGuiCol_Header] = GetColor(MainAccentColor, Alpha80);
		colors[ImGuiCol_HeaderHovered] = Hovered(colors[ImGuiCol_Header]);
		colors[ImGuiCol_HeaderActive] = Active(colors[ImGuiCol_Header]);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = Hovered(colors[ImGuiCol_Separator]);
		colors[ImGuiCol_SeparatorActive] = Active(colors[ImGuiCol_Separator]);
		colors[ImGuiCol_ResizeGrip] = GetColor(MainColor, Alpha20);
		colors[ImGuiCol_ResizeGripHovered] = Hovered(colors[ImGuiCol_ResizeGrip]);
		colors[ImGuiCol_ResizeGripActive] = Active(colors[ImGuiCol_ResizeGrip]);
		colors[ImGuiCol_Tab] = GetColor(MainColor, Alpha60);
		colors[ImGuiCol_TabHovered] = Hovered(colors[ImGuiCol_Tab]);
		colors[ImGuiCol_TabActive] = Active(colors[ImGuiCol_Tab]);
		colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
		colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
		colors[ImGuiCol_DockingPreview] = Darken(colors[ImGuiCol_HeaderActive], 0.2f);
		colors[ImGuiCol_DockingEmptyBg] = Darken(colors[ImGuiCol_HeaderActive], 0.6f);
		colors[ImGuiCol_PlotLines] = GetColor(HighlightColor);
		colors[ImGuiCol_PlotLinesHovered] = Hovered(colors[ImGuiCol_PlotLines]);
		colors[ImGuiCol_PlotHistogram] = GetColor(HighlightColor);
		colors[ImGuiCol_PlotHistogramHovered] = Hovered(colors[ImGuiCol_PlotHistogram]);
		colors[ImGuiCol_TextSelectedBg] = GetColor(HighlightColor, Alpha40);
		colors[ImGuiCol_DragDropTarget] = GetColor(HighlightColor, Alpha80);;
		colors[ImGuiCol_NavHighlight] = GetColor(White);
		colors[ImGuiCol_NavWindowingHighlight] = GetColor(White, Alpha80);
		colors[ImGuiCol_NavWindowingDimBg] = GetColor(White, Alpha20);
		colors[ImGuiCol_ModalWindowDimBg] = GetColor(Black, Alpha60);

		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_Right;
	}

	void EditorTheme::ApplyRayTek()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle* style = &ImGui::GetStyle();

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	}

	void EditorTheme::ApplyOriginTheme(bool bStyleDark_, float alpha_)
	{
    ImGuiStyle& style = ImGui::GetStyle();

    // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

    if (bStyleDark_)
    {
      for (int i = 0; i <= ImGuiCol_COUNT; i++)
      {
        ImVec4& col = style.Colors[i];
        float H, S, V;
        ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

        if (S < 0.1f)
        {
          V = 1.0f - V;
        }
        ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
        if (col.w < 1.00f)
        {
          col.w *= alpha_;
        }
      }
    }
    else
    {
      for (int i = 0; i <= ImGuiCol_COUNT; i++)
      {
        ImVec4& col = style.Colors[i];
        if (col.w < 1.00f)
        {
          col.x *= alpha_;
          col.y *= alpha_;
          col.z *= alpha_;
          col.w *= alpha_;
        }
      }
    }
  }

  inline bool CheckBoxFont(const char* name_, bool* pB_, const char* pOn_ = "[X]", const char* pOff_ = "[  ]")
  {
    if (*pB_)
    {
      ImGui::Text(pOn_);
    }
    else
    {
      ImGui::Text(pOff_);
    }
    bool bHover = false;
    bHover = bHover || ImGui::IsItemHovered();
    ImGui::SameLine();
    ImGui::Text(name_);
    bHover = bHover || ImGui::IsItemHovered();
    if (bHover && ImGui::IsMouseClicked(0))
    {
      *pB_ = !*pB_;
      return true;
    }
    return false;
  }

  inline bool CheckBoxTick(const char* name_, bool* pB_)
  {
    return CheckBoxFont(name_, pB_, ICON_FA_CHECK_SQUARE_O, ICON_FA_SQUARE_O);
  }

  inline bool MenuItemCheckbox(const char* name_, bool* pB_)
  {
    bool retval = ImGui::MenuItem(name_);
    ImGui::SameLine();
    if (*pB_)
    {
      ImGui::Text(ICON_FA_CHECK_SQUARE_O);
    }
    else
    {
      ImGui::Text(ICON_FA_SQUARE_O);
    }
    if (retval)
    {
      *pB_ = !*pB_;
    }
    return retval;
  }

  struct FrameTimeHistogram
  {
    // configuration params - modify these at will
    static const int NUM = 101; //last value is from T-1 to inf.

    float  dT = 0.001f;    // in seconds, default 1ms
    float  refresh = 1.0f / 60.0f;// set this to your target refresh rate

    static const int NUM_MARKERS = 2;
    float  markers[NUM_MARKERS] = { 0.99f, 0.999f };

    // data
    ImVec2 size = ImVec2(3.0f * NUM, 40.0f);
    float  lastdT = 0.0f;
    float  timesTotal;
    float  countsTotal;
    float  times[NUM];
    float  counts[NUM];
    float  hitchTimes[NUM];
    float  hitchCounts[NUM];

    FrameTimeHistogram()
    {
      Clear();
    }

    void Clear()
    {
      timesTotal = 0.0f;
      countsTotal = 0.0f;
      memset(times, 0, sizeof(times));
      memset(counts, 0, sizeof(counts));
      memset(hitchTimes, 0, sizeof(hitchTimes));
      memset(hitchCounts, 0, sizeof(hitchCounts));
    }

    int GetBin(float time_)
    {
      int bin = (int)floor(time_ / dT);
      if (bin >= NUM)
      {
        bin = NUM - 1;
      }
      return bin;
    }

    void Update(float deltaT_)
    {
      if (deltaT_ < 0.0f)
      {
        assert(false);
        return;
      }
      int bin = GetBin(deltaT_);
      times[bin] += deltaT_;
      timesTotal += deltaT_;
      counts[bin] += 1.0f;
      countsTotal += 1.0f;

      float hitch = abs(lastdT - deltaT_);
      int deltaBin = GetBin(hitch);
      hitchTimes[deltaBin] += hitch;
      hitchCounts[deltaBin] += 1.0f;
      lastdT = deltaT_;
    }

    void PlotRefreshLines(float total_ = 0.0f, float* pValues_ = NULL)
    {
      ImDrawList* draw = ImGui::GetWindowDrawList();
      const ImGuiStyle& style = ImGui::GetStyle();
      ImVec2 pad = style.FramePadding;
      ImVec2 min = ImGui::GetItemRectMin();
      min.x += pad.x;
      ImVec2 max = ImGui::GetItemRectMax();
      max.x -= pad.x;

      float xRefresh = (max.x - min.x) * refresh / (dT * NUM);

      float xCurr = xRefresh + min.x;
      while (xCurr < max.x)
      {
        float xP = ceil(xCurr); // use ceil to get integer coords or else lines look odd
        draw->AddLine(ImVec2(xP, min.y), ImVec2(xP, max.y), 0x50FFFFFF);
        xCurr += xRefresh;
      }

      if (pValues_)
      {
        // calc markers
        float currTotal = 0.0f;
        int   mark = 0;
        for (int i = 0; i < NUM && mark < NUM_MARKERS; ++i)
        {
          currTotal += pValues_[i];
          if (total_ * markers[mark] < currTotal)
          {
            float xP = ceil((float)(i + 1) / (float)NUM * (max.x - min.x) + min.x); // use ceil to get integer coords or else lines look odd
            draw->AddLine(ImVec2(xP, min.y), ImVec2(xP, max.y), 0xFFFF0000);
            ++mark;
          }
        }
      }
    }

    void CalcHistogramSize(int numShown_)
    {
      ImVec2 wRegion = ImGui::GetContentRegionMax();
      float heightGone = 7.0f * ImGui::GetTextLineHeightWithSpacing();
      wRegion.y -= heightGone;
      wRegion.y /= (float)numShown_;
      const ImGuiStyle& style = ImGui::GetStyle();
      ImVec2 pad = style.FramePadding;
      wRegion.x -= 2.0f * pad.x;
      size = wRegion;
    }


    void Draw(const char* name_, bool* pOpen_ = NULL)
    {
      if (ImGui::Begin(name_, pOpen_))
      {
        int numShown = 0;
        if (ImGui::CollapsingHeader("Time Histogram"))
        {
          ++numShown;
          ImGui::PlotHistogram("", times, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
          PlotRefreshLines(timesTotal, times);
        }
        if (ImGui::CollapsingHeader("Count Histogram"))
        {
          ++numShown;
          ImGui::PlotHistogram("", counts, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
          PlotRefreshLines(countsTotal, counts);
        }
        if (ImGui::CollapsingHeader("Hitch Time Histogram"))
        {
          ++numShown;
          ImGui::PlotHistogram("", hitchTimes, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
          PlotRefreshLines();
        }
        if (ImGui::CollapsingHeader("Hitch Count Histogram"))
        {
          ++numShown;
          ImGui::PlotHistogram("", hitchCounts, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
          PlotRefreshLines();
        }
        if (ImGui::Button("Clear"))
        {
          Clear();
        }
        CalcHistogramSize(numShown);
      }
      ImGui::End();
    }
  };
}