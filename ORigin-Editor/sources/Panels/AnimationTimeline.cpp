#include "AnimationTimeline.h"
#include "Origin\Scene\Components.h"

namespace origin {

	AnimationTimeline::AnimationTimeline()
	{
	}

	void AnimationTimeline::DrawEntityAnimation(std::vector<std::shared_ptr<Animation>>& animations, int* currentAnim)
	{
    *currentAnim = CurrentAnimIndex;

		static int selectedEntry = -1;
		static int firstFrame = 0;
		static bool expanded = true;

		ImGui::Begin("Animation");

		ImGui::PushItemWidth(130);
		ImGui::Text("New");
		ImGui::SameLine();
		static std::string animName;
		char buffer[256];
		strcpy_s(buffer, sizeof(buffer), animName.c_str());
		if (ImGui::InputText("##anim_name", buffer, sizeof(buffer)))
			animName = std::string(buffer);

		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
			if (!animName.empty())
			{
        std::shared_ptr<Animation> newAnim = Animation::Create(animName);
        animations.push_back(newAnim);
        animName.clear();
			}
		}

    ImGui::PopItemWidth();

    if (!animations.empty())
    {
      ImGui::PushItemWidth(130);
      ImGui::SameLine();
      std::string currentAnimName = animations[CurrentAnimIndex]->GetName();
      if (ImGui::BeginCombo("##animation_state", currentAnimName.c_str()))
      {
        bool isSelected = false;
        for (int i = 0; i < animations.size(); i++)
        {
          isSelected = currentAnimName == animations[i]->GetName();
          if (ImGui::Selectable(animations[i]->GetName().c_str(), isSelected))
            CurrentAnimIndex = i;

          if (isSelected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }

      ImGui::SameLine();
      if (ImGui::Button("-"))
      {
        animations.erase(animations.begin() + CurrentAnimIndex);
        CurrentAnimIndex = animations.size() > 0 ? animations.size() - 1 : 0;
      }


      ImVec4 buttonColor = ImVec4(0.1f, 0.6f, 0.1f, 1.0f);
      ImVec4 buttonColorHovered = ImVec4(0.1f, 0.8f, 0.1f, 1.0f);

      if (animations[CurrentAnimIndex]->Preview)
      {
        buttonColor = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);
        buttonColorHovered = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
      }

      ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
      if (ImGui::Button("Preview"))
      {
        animations[CurrentAnimIndex]->Preview = !animations[CurrentAnimIndex]->Preview;
        animations[CurrentAnimIndex]->Reset();
      }
      ImGui::PopStyleColor(3);

      ImGui::SameLine();
      ImGui::Text("Frame");
      ImGui::SameLine();
      ImGui::InputInt("##current_frame", &animations[CurrentAnimIndex]->CurrentFrame);
      ImGui::SameLine();
      ImGui::Text("Max Frame");
      ImGui::SameLine();
      ImGui::InputInt("##frame_max", &animations[CurrentAnimIndex]->MaxFrame);
      ImGui::SameLine();
      ImGui::Text("Looping");
      ImGui::SameLine();
      ImGui::Checkbox("##looping", &animations[CurrentAnimIndex]->Looping);

      ImGui::PopItemWidth();

      Timeline(animations[CurrentAnimIndex], &animations[CurrentAnimIndex]->CurrentFrame, &expanded, &selectedEntry, &firstFrame,
        SEQUENCER_EDIT_STARTEND | SEQUENCER_ADD | SEQUENCER_DEL | SEQUENCER_COPYPASTE | SEQUENCER_CHANGE_FRAME);

    }
		
		ImGui::End();
	}

	const char* AnimationTimeline::GetItemLabel(AnimationType type, int index) const
	{
		static char tmps[512];
		snprintf(tmps, 512, "[%02d] %s", index, AnimationTypeToString(type));
    return tmps;
	}

	void AnimationTimeline::Get(const std::shared_ptr<Animation>& anim, int index, int** start, int** end, unsigned int* color)
	{
		auto& item = anim->GetFrame(index);
		if (color)
			*color = 0xFFAA8080;
		if (start)
			*start = &item.FrameBegin;
		if (end)
			*end = &item.FrameEnd;
	}

	void AnimationTimeline::CustomDrawCompact(const std::shared_ptr<Animation>& anim, int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
	{
    auto& frame = anim->GetFrame(index);
		rampEdit.mMax = ImVec2(float(anim->MaxFrame), 1.f);
		rampEdit.mMin = ImVec2(0.0f, 0.f);
		draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < rampEdit.mPointCount[i]; j++)
			{
				float p = rampEdit.mPts[i][j].x;
				if (p < frame.FrameBegin || p > frame.FrameEnd)
					continue;
				float r = p / float(anim->MaxFrame);
				float x = ImLerp(rc.Min.x, rc.Max.x, r);
				draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
			}
		}
		draw_list->PopClipRect();
	}

	size_t AnimationTimeline::GetCustomHeight(int index)
	{
    //return m_State.Get.Expanded ? 150 : 0;
    return 0;
	}

	static bool SequencerAddDelButton(ImDrawList* draw_list, ImVec2 pos, bool add = true)
  {
    ImGuiIO& io = ImGui::GetIO();
    ImRect btnRect(pos, ImVec2(pos.x + 16, pos.y + 16));
    bool overBtn = btnRect.Contains(io.MousePos);
    bool containedClick = overBtn && btnRect.Contains(io.MouseClickedPos[0]);
    bool clickedBtn = containedClick && io.MouseReleased[0];
    int btnColor = overBtn ? 0xAAEAFFAA : 0x77A3B2AA;
    if (containedClick && io.MouseDownDuration[0] > 0)
      btnRect.Expand(2.0f);

    float midy = pos.y + 16 / 2 - 0.5f;
    float midx = pos.x + 16 / 2 - 0.5f;
    draw_list->AddRect(btnRect.Min, btnRect.Max, btnColor, 4);
    draw_list->AddLine(ImVec2(btnRect.Min.x + 3, midy), ImVec2(btnRect.Max.x - 3, midy), btnColor, 2);
    if (add)
      draw_list->AddLine(ImVec2(midx, btnRect.Min.y + 3), ImVec2(midx, btnRect.Max.y - 3), btnColor, 2);
    return clickedBtn;
  }

  bool AnimationTimeline::Timeline(std::shared_ptr<Animation>& animation, int* currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions)
  {
    bool ret = false;
    ImGuiIO& io = ImGui::GetIO();
    int cx = (int)(io.MousePos.x);
    int cy = (int)(io.MousePos.y);
    static float framePixelWidth = 10.f;
    static float framePixelWidthTarget = 10.f;
    int legendWidth = 200;

    static int movingEntry = -1;
    static int movingPos = -1;
    static int movingPart = -1;
    int delEntry = -1;
    int dupEntry = -1;
    int itemHeight = 20;

    bool popupOpened = false;
    int sequenceCount = animation->GetTotalFrames();

    ImGui::BeginGroup();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    int firstFrameUsed = firstFrame ? *firstFrame : 0;
    int controlHeight = sequenceCount * itemHeight;
    for (int i = 0; i < sequenceCount; i++)
      controlHeight += int(this->GetCustomHeight(i));
    int frameCount = ImMax(animation->MaxFrame, 1);

    static bool MovingScrollBar = false;
    static bool MovingCurrentFrame = false;

    struct CustomDraw
    {
      int index;
      ImRect customRect;
      ImRect legendRect;
      ImRect clippingRect;
      ImRect legendClippingRect;
    };

    ImVector<CustomDraw> customDraws;
    ImVector<CustomDraw> compactCustomDraws;

    // zoom in/out
    const int visibleFrameCount = (int)floorf((canvasSize.x - legendWidth) / framePixelWidth);
    const float barWidthRatio = ImMin(visibleFrameCount / (float)frameCount, 1.f);
    const float barWidthInPixels = barWidthRatio * (canvasSize.x - legendWidth);

    ImRect regionRect(canvasPos, canvasPos + canvasSize);

    static bool panningView = false;
    static ImVec2 panningViewSource;
    static int panningViewFrame;

    if (ImGui::IsWindowFocused() && io.KeyAlt && io.MouseDown[2])
    {
      if (!panningView)
      {
        panningViewSource = io.MousePos;
        panningView = true;
        panningViewFrame = *firstFrame;
      }

      *firstFrame = panningViewFrame - int((io.MousePos.x - panningViewSource.x) / framePixelWidth);
      *firstFrame = ImClamp(*firstFrame, 0, animation->MaxFrame - visibleFrameCount);
    }

    if (panningView && !io.MouseDown[2])
    {
      panningView = false;
    }
    framePixelWidthTarget = ImClamp(framePixelWidthTarget, 0.1f, 50.f);

    framePixelWidth = ImLerp(framePixelWidth, framePixelWidthTarget, 0.33f);

    frameCount = animation->MaxFrame;
    if (visibleFrameCount >= frameCount && firstFrame)
      *firstFrame = 0;

    if (expanded && !*expanded)
    {
      ImGui::InvisibleButton("canvas", ImVec2(canvasSize.x - canvasPos.x, (float)itemHeight));
      drawList->AddRectFilled(canvasPos, ImVec2(canvasSize.x + canvasPos.x, canvasPos.y + itemHeight), 0xFF3D3837, 0);
      char tmps[512];
      ImFormatString(tmps, IM_ARRAYSIZE(tmps), this->GetCollapseFmt(), frameCount, sequenceCount);
      drawList->AddText(ImVec2(canvasPos.x + 26, canvasPos.y + 2), 0xFFFFFFFF, tmps);
    }
    else
    {
      bool hasScrollBar = true;
      ImVec2 headerSize(canvasSize.x, (float)itemHeight);
      ImVec2 scrollBarSize(canvasSize.x, 14.f);
      ImGui::InvisibleButton("topBar", headerSize);
      drawList->AddRectFilled(canvasPos, canvasPos + headerSize, 0xFFFF0000, 0);
      ImVec2 childFramePos = ImGui::GetCursorScreenPos();
      ImVec2 childFrameSize(canvasSize.x, canvasSize.y - 8.f - headerSize.y - (hasScrollBar ? scrollBarSize.y : 0));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
      ImGui::BeginChildFrame(889, childFrameSize);

      if(sequenceCount)
        ImGui::InvisibleButton("contentBar", ImVec2(canvasSize.x, float(controlHeight)));

      const ImVec2 contentMin = ImGui::GetItemRectMin();
      const ImVec2 contentMax = ImGui::GetItemRectMax();
      const ImRect contentRect(contentMin, contentMax);
      const float contentHeight = contentMax.y - contentMin.y;



			ImVec2 posMin = ImVec2(canvasPos);
			ImVec2 posMax = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
			uint32_t rectColor = IM_COL32(40, 30, 20, 255);
			uint32_t rectTransparentColor = IM_COL32(0, 0, 0, 0);
      // Legend
      drawList->AddRectFilled(posMin, { canvasSize.x + legendWidth, canvasPos.y + canvasSize.y }, IM_COL32(50, 40, 30, 255));
      // Background
			drawList->AddRectFilledMultiColor(posMin, posMax, rectTransparentColor, rectTransparentColor, rectColor, rectColor);

      if (animation->GetType() == AnimationType::SpriteSheet)
      {
				ImGui::Button("Drag Here");
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *(AssetHandle*)payload->Data;
						animation->AddFrame(handle);
					}
					ImGui::EndDragDropTarget();
				}
      }

      // current frame top
      ImRect topRect(ImVec2(canvasPos.x + legendWidth, canvasPos.y), ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + itemHeight));

      if (!MovingCurrentFrame && !MovingScrollBar && movingEntry == -1 && sequenceOptions & SEQUENCER_CHANGE_FRAME && currentFrame && *currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0])
      {
        MovingCurrentFrame = true;
      }
      if (MovingCurrentFrame)
      {
        if (frameCount)
        {
          *currentFrame = (int)((io.MousePos.x - topRect.Min.x) / framePixelWidth) + firstFrameUsed;
          if (*currentFrame < 0)
            *currentFrame = 0;
          if (*currentFrame >= animation->MaxFrame)
            *currentFrame = animation->MaxFrame;
        }
        if (!io.MouseDown[0])
          MovingCurrentFrame = false;
      }

      //header
      drawList->AddRectFilled(canvasPos, ImVec2(canvasSize.x + canvasPos.x, canvasPos.y + itemHeight), IM_COL32(20, 10, 15, 255), 0);
      if (sequenceOptions & SEQUENCER_ADD)
      {
        if (SequencerAddDelButton(drawList, ImVec2(canvasPos.x + legendWidth - itemHeight, canvasPos.y + 2), true))
          ImGui::OpenPopup("Add Entry");

        if (ImGui::BeginPopup("Add Entry"))
        {
          for (int i = 0; i < 1; i++)
          {
            if (ImGui::Selectable(AnimationTypeToString((AnimationType)i)))
            {
              animation->SetType((AnimationType)i);
              *selectedEntry = sequenceCount - 1;
            }
          }

          ImGui::EndPopup();
          popupOpened = true;
        }
      }

      //header frame number and lines
      int modFrameCount = 10;
      int frameStep = 1;
      while ((modFrameCount * framePixelWidth) < 150)
      {
        modFrameCount *= 2;
        frameStep *= 2;
      };
      int halfModFrameCount = modFrameCount / 2;

      auto drawLine = [&](int i, int regionHeight)
        {
          bool baseIndex = ((i % modFrameCount) == 0) || (i == animation->MaxFrame || i == 0);
          bool halfIndex = (i % halfModFrameCount) == 0;
          int px = (int)canvasPos.x + int(i * framePixelWidth) + legendWidth - int(firstFrameUsed * framePixelWidth);
          int tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
          int tiretEnd = baseIndex ? regionHeight : itemHeight;

          if (px <= (canvasSize.x + canvasPos.x) && px >= (canvasPos.x + legendWidth))
          {
            drawList->AddLine(ImVec2((float)px, canvasPos.y + (float)tiretStart), ImVec2((float)px, canvasPos.y + (float)tiretEnd - 1), 0xFF606060, 1);
            drawList->AddLine(ImVec2((float)px, canvasPos.y + (float)itemHeight), ImVec2((float)px, canvasPos.y + (float)regionHeight - 1), 0x30606060, 1);
          }

          if (baseIndex && px > (canvasPos.x + legendWidth))
          {
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", i);
            drawList->AddText(ImVec2((float)px + 3.f, canvasPos.y), 0xFFBBBBBB, tmps);
          }
        };

      auto drawLineContent = [&](int i, int)
        {
          int px = (int)canvasPos.x + int(i * framePixelWidth) + legendWidth - int(firstFrameUsed * framePixelWidth);
          int tiretStart = int(contentMin.y);
          int tiretEnd = int(contentMax.y);

          if (px <= (canvasSize.x + canvasPos.x) && px >= (canvasPos.x + legendWidth))
            drawList->AddLine(ImVec2(float(px), float(tiretStart)), ImVec2(float(px), float(tiretEnd)), 0x30606060, 1);
        };

      for (int i = 0; i <= animation->MaxFrame; i += frameStep)
        drawLine(i, itemHeight);

      drawLine(0, itemHeight);
      drawLine(animation->MaxFrame, itemHeight);

      drawList->PushClipRect(childFramePos, childFramePos + childFrameSize, true);

      size_t customHeight = 0;
      for (int i = 0; i < sequenceCount; i++)
      {
        ImVec2 tpos(contentMin.x + 3, contentMin.y + i * itemHeight + 2 + customHeight);
        drawList->AddText(tpos, 0xFFFFFFFF, this->GetItemLabel(animation->GetType(), i));

        if (sequenceOptions & SEQUENCER_DEL)
        {
          if (SequencerAddDelButton(drawList, ImVec2(contentMin.x + legendWidth - itemHeight + 2 - 10, tpos.y + 2), false))
            delEntry = i;
        }

        customHeight += this->GetCustomHeight(i);
      }

      // slots background
      customHeight = 0;
      for (int i = 0; i < sequenceCount; i++)
      {
        unsigned int col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;

        size_t localCustomHeight = this->GetCustomHeight(i);
        ImVec2 pos = ImVec2(contentMin.x + legendWidth, contentMin.y + itemHeight * i + 1 + customHeight);
        ImVec2 sz = ImVec2(canvasSize.x + canvasPos.x, pos.y + itemHeight - 1 + localCustomHeight);
        if (!popupOpened && cy >= pos.y && cy < pos.y + (itemHeight + localCustomHeight) && movingEntry == -1 && cx>contentMin.x && cx < contentMin.x + canvasSize.x)
        {
          col += 0x80201008;
          pos.x -= legendWidth;
        }

        drawList->AddRectFilled(pos, sz, col, 0);
        customHeight += localCustomHeight;
      }

      drawList->PushClipRect(childFramePos + ImVec2(float(legendWidth), 0.f), childFramePos + childFrameSize, true);

      // vertical frame lines in content area
      for (int i = 0; i <= animation->MaxFrame; i += frameStep)
      {
        drawLineContent(i, int(contentHeight));
      }
      drawLineContent(0, int(contentHeight));
      drawLineContent(0, int(contentHeight));

      // selection
      bool selected = selectedEntry && (*selectedEntry >= 0);
      if (selected)
      {
        customHeight = 0;
        for (int i = 0; i < *selectedEntry; i++)
          customHeight += this->GetCustomHeight(i);
        drawList->AddRectFilled(ImVec2(contentMin.x, contentMin.y + itemHeight * *selectedEntry + customHeight), ImVec2(contentMin.x + canvasSize.x, contentMin.y + itemHeight * (*selectedEntry + 1) + customHeight), 0x801080FF, 1.f);
      }

      // slots
      customHeight = 0;
      for (int i = 0; i < sequenceCount; i++)
      {
        int* start, * end;
        unsigned int color;
        this->Get(animation, i, &start, &end, &color);
        size_t localCustomHeight = this->GetCustomHeight(i);

        ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + itemHeight * i + 1 + customHeight);
        ImVec2 slotP1(pos.x + *start * framePixelWidth, pos.y + 2);
        ImVec2 slotP2(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + itemHeight - 2);
        ImVec2 slotP3(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + itemHeight - 2 + localCustomHeight);
        unsigned int slotColor = color | 0xFF000000;
        unsigned int slotColorHalf = (color & 0xFFFFFF) | 0x40000000;

        if (slotP1.x <= (canvasSize.x + contentMin.x) && slotP2.x >= (contentMin.x + legendWidth))
        {
          drawList->AddRectFilled(slotP1, slotP3, slotColorHalf, 2);
          drawList->AddRectFilled(slotP1, slotP2, slotColor, 2);
        }

        const float max_handle_width = slotP2.x - slotP1.x / 3.0f;
        const float min_handle_width = ImMin(10.0f, max_handle_width);
        const float handle_width = ImClamp(framePixelWidth / 2.0f, min_handle_width, max_handle_width);
        ImRect rects[3] = { ImRect(slotP1, ImVec2(slotP1.x + handle_width, slotP2.y))
            , ImRect(ImVec2(slotP2.x - handle_width, slotP1.y), slotP2)
            , ImRect(slotP1, slotP2) };

        const unsigned int quadColor[] = { 0xFFFFFFFF, 0xFFFFFFFF, slotColor + (selected ? 0 : 0x202020) };
        if (movingEntry == -1 && (sequenceOptions & SEQUENCER_EDIT_STARTEND))
        {
          for (int j = 2; j >= 0; j--)
          {
            ImRect& rc = rects[j];
            if (!rc.Contains(io.MousePos))
              continue;
            drawList->AddRectFilled(rc.Min, rc.Max, quadColor[j], 2);
          }

          for (int j = 0; j < 3; j++)
          {
            ImRect& rc = rects[j];
            if (!rc.Contains(io.MousePos))
              continue;
            if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
              continue;
            if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame)
            {
              movingEntry = i;
              movingPos = cx;
              movingPart = j + 1;
              break;
            }
          }
        }

        // custom draw
        if (localCustomHeight > 0)
        {
          ImVec2 rp(canvasPos.x, contentMin.y + itemHeight * i + 1 + customHeight);
          ImRect customRect(rp + ImVec2(legendWidth - (firstFrameUsed - 0.5f) * framePixelWidth, float(itemHeight)),
            rp + ImVec2(legendWidth + (animation->MaxFrame - firstFrameUsed - 0.5f + 2.f) * framePixelWidth, float(localCustomHeight + itemHeight)));
          ImRect clippingRect(rp + ImVec2(float(legendWidth), float(itemHeight)), rp + ImVec2(canvasSize.x, float(localCustomHeight + itemHeight)));

          ImRect legendRect(rp + ImVec2(0.f, float(itemHeight)), rp + ImVec2(float(legendWidth), float(localCustomHeight)));
          ImRect legendClippingRect(canvasPos + ImVec2(0.f, float(itemHeight)), canvasPos + ImVec2(float(legendWidth), float(localCustomHeight + itemHeight)));
          customDraws.push_back({ i, customRect, legendRect, clippingRect, legendClippingRect });
        }
        else
        {
          ImVec2 rp(canvasPos.x, contentMin.y + itemHeight * i + customHeight);
          ImRect customRect(rp + ImVec2(legendWidth - (firstFrameUsed - 0.5f) * framePixelWidth, float(0.f)),
            rp + ImVec2(legendWidth + (animation->MaxFrame - firstFrameUsed - 0.5f + 2.f) * framePixelWidth, float(itemHeight)));
          ImRect clippingRect(rp + ImVec2(float(legendWidth), float(0.f)), rp + ImVec2(canvasSize.x, float(itemHeight)));

          compactCustomDraws.push_back({ i, customRect, ImRect(), clippingRect, ImRect() });
        }
        customHeight += localCustomHeight;
      }

      if (movingEntry >= 0)
      {
        ImGui::SetNextFrameWantCaptureMouse(true);

        int diffFrame = int((cx - movingPos) / framePixelWidth);
        if (std::abs(diffFrame) > 0)
        {
          int* start, * end;
          this->Get(animation, movingEntry, &start, &end, NULL);
          if (selectedEntry)
            *selectedEntry = movingEntry;
          int& l = *start;
          int& r = *end;
          if (movingPart & 1)
            l += diffFrame;
          if (movingPart & 2)
            r += diffFrame;
          if (l < 0)
          {
            if (movingPart & 2)
              r -= l;
            l = 0;
          }
          if (movingPart & 1 && l > r)
            l = r;
          if (movingPart & 2 && r < l)
            r = l;
          movingPos += int(diffFrame * framePixelWidth);
        }
        if (!io.MouseDown[0])
        {
          // single select
          if (!diffFrame && movingPart && selectedEntry)
          {
            *selectedEntry = movingEntry;
            ret = true;
          }
          movingEntry = -1;
        }
      }

      // cursor
      if (currentFrame && firstFrame && *currentFrame >= *firstFrame && *currentFrame <= animation->MaxFrame)
      {
        static const float cursorWidth = 8.f;
        float cursorOffset = contentMin.x + legendWidth + (*currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2 - cursorWidth * 0.5f;
        drawList->AddLine(ImVec2(cursorOffset, canvasPos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, cursorWidth);
        char tmps[512];
        ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", *currentFrame);
        drawList->AddText(ImVec2(cursorOffset + 10, canvasPos.y + 2), 0xFF2A2AFF, tmps);
      }

      drawList->PopClipRect();
      drawList->PopClipRect();

      for (auto& customDraw : compactCustomDraws)
        this->CustomDrawCompact(animation, customDraw.index, drawList, customDraw.customRect, customDraw.clippingRect);

      ImGui::EndChildFrame();
      ImGui::PopStyleColor();
      if (hasScrollBar)
      {

        ImGui::InvisibleButton("scrollBar", scrollBarSize);
        ImVec2 scrollBarMin = ImGui::GetItemRectMin();
        ImVec2 scrollBarMax = ImGui::GetItemRectMax();

        // ratio = number of frames visible in control / number to total frames
        float startFrameOffset = ((float)(firstFrameUsed) / (float)frameCount) * (canvasSize.x - legendWidth);
        ImVec2 scrollBarA(scrollBarMin.x + legendWidth, scrollBarMin.y - 2);
        ImVec2 scrollBarB(scrollBarMin.x + canvasSize.x, scrollBarMax.y - 1);
        drawList->AddRectFilled(scrollBarA, scrollBarB, 0xFF222222, 0);

        ImRect scrollBarRect(scrollBarA, scrollBarB);
        bool inScrollBar = scrollBarRect.Contains(io.MousePos);

        drawList->AddRectFilled(scrollBarA, scrollBarB, 0xFF101010, 8);

        ImVec2 scrollBarC(scrollBarMin.x + legendWidth + startFrameOffset, scrollBarMin.y);
        ImVec2 scrollBarD(scrollBarMin.x + legendWidth + barWidthInPixels + startFrameOffset, scrollBarMax.y - 2);
        drawList->AddRectFilled(scrollBarC, scrollBarD, (inScrollBar || MovingScrollBar) ? 0xFF606060 : 0xFF505050, 6);

        ImRect barHandleLeft(scrollBarC, ImVec2(scrollBarC.x + 14, scrollBarD.y));
        ImRect barHandleRight(ImVec2(scrollBarD.x - 14, scrollBarC.y), scrollBarD);

        bool onLeft = barHandleLeft.Contains(io.MousePos);
        bool onRight = barHandleRight.Contains(io.MousePos);

        static bool sizingRBar = false;
        static bool sizingLBar = false;

        drawList->AddRectFilled(barHandleLeft.Min, barHandleLeft.Max, (onLeft || sizingLBar) ? 0xFFAAAAAA : 0xFF666666, 6);
        drawList->AddRectFilled(barHandleRight.Min, barHandleRight.Max, (onRight || sizingRBar) ? 0xFFAAAAAA : 0xFF666666, 6);

        ImRect scrollBarThumb(scrollBarC, scrollBarD);
        static const float MinBarWidth = 4.f;
        const int extraWidth = 2;

        if (sizingRBar)
        {
          if (!io.MouseDown[0])
          {
            sizingRBar = false;
          }
          else
          {
            float barNewWidth = ImMax(barWidthInPixels + io.MouseDelta.x, MinBarWidth);
            float barRatio = barNewWidth / barWidthInPixels;
            framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
            int newVisibleFrameCount = int((canvasSize.x - legendWidth + extraWidth) / (framePixelWidthTarget + extraWidth));
            int lastFrame = *firstFrame + newVisibleFrameCount;
            if (lastFrame > animation->MaxFrame)
            {
              framePixelWidthTarget = framePixelWidth = (canvasSize.x - legendWidth) / float(animation->MaxFrame - *firstFrame);
            }
          }
        }
        else if (sizingLBar)
        {
          if (!io.MouseDown[0])
          {
            sizingLBar = false;
          }
          else
          {
            if (fabsf(io.MouseDelta.x) > FLT_EPSILON)
            {
              float barNewWidth = ImMax(barWidthInPixels - io.MouseDelta.x, MinBarWidth);
              float barRatio = barNewWidth / barWidthInPixels;
              float previousFramePixelWidthTarget = framePixelWidthTarget;
              framePixelWidthTarget = framePixelWidth = framePixelWidth / barRatio;
              int newVisibleFrameCount = int(visibleFrameCount / barRatio);
              int newFirstFrame = *firstFrame + newVisibleFrameCount - visibleFrameCount;
              newFirstFrame = ImClamp(newFirstFrame, 0, ImMax(animation->MaxFrame - visibleFrameCount, 0));
              if (newFirstFrame == *firstFrame)
              {
                framePixelWidth = framePixelWidthTarget = previousFramePixelWidthTarget;
              }
              else
              {
                *firstFrame = newFirstFrame;
              }
            }
          }
        }
        else
        {
          // Dragging Scroll Bar
          if (MovingScrollBar)
          {
            if (!io.MouseDown[0])
            {
              MovingScrollBar = false;
            }
            else
            {
              float framesPerPixelInBar = barWidthInPixels / (float)visibleFrameCount;
              *firstFrame = int((io.MousePos.x - panningViewSource.x) / framesPerPixelInBar) - panningViewFrame;
              *firstFrame = ImClamp(*firstFrame, 0, ImMax(animation->MaxFrame - visibleFrameCount + extraWidth, 0));
            }
          }
          else
          {
            if (scrollBarThumb.Contains(io.MousePos) && ImGui::IsMouseClicked(0) && firstFrame && !MovingCurrentFrame && movingEntry == -1)
            {
              MovingScrollBar = true;
              panningViewSource = io.MousePos;
              panningViewFrame = -*firstFrame;
            }
            if (!sizingRBar && onRight && ImGui::IsMouseClicked(0))
              sizingRBar = true;
            if (!sizingLBar && onLeft && ImGui::IsMouseClicked(0))
              sizingLBar = true;

          }
        }
      }
    }

    ImGui::EndGroup();

    if (regionRect.Contains(io.MousePos))
    {
      bool overCustomDraw = false;
      for (auto& custom : customDraws)
      {
        if (custom.customRect.Contains(io.MousePos))
        {
          overCustomDraw = true;
        }
      }
    }

    if (expanded)
    {
      if (SequencerAddDelButton(drawList, ImVec2(canvasPos.x + 2, canvasPos.y + 2), !*expanded))
        *expanded = !*expanded;
    }

    if (delEntry != -1)
    {
      animation->DeleteFrame(delEntry);
      if (selectedEntry && (*selectedEntry == delEntry || *selectedEntry >= sequenceCount))
        *selectedEntry = -1;
    }

    return ret;
  }

}