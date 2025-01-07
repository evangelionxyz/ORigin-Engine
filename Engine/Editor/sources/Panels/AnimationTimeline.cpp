// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "AnimationTimeline.h"
#include "Origin/Scene/Components/Components.h"

namespace origin
{
    static bool SequencerAddDelButton(ImDrawList *draw_list, ImVec2 pos, bool add = true)
    {
        ImGuiIO &io = ImGui::GetIO();
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

    template<typename T>
    void Get(const std::shared_ptr<T> &anim, int index, int **start, int **end, unsigned int *color)
    {
        auto &frame = anim->GetFrame(index);
        if (color)
            *color = IM_COL32(230, 100, 0, 255);
        if (start)
            *start = &frame.FrameBegin;
        if (end)
            *end = &frame.FrameEnd;
    }

    void AnimationTimeline::DrawSpriteAnimTimeline(SpriteAnimationComponent &sa)
    {
        static int selectedEntry = -1;
        static int firstFrame = 0;
        static bool expanded = true;

        ImGui::Begin("Animation");

        ImGui::PushItemWidth(100);
        ImGui::Text("New");
        ImGui::SameLine();
        static std::string stateName;
        char buffer[256];
        strncpy(buffer, stateName.c_str(), sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination
        if (ImGui::InputText("##anim_name", buffer, sizeof(buffer)))
            stateName = std::string(buffer);

        ImGui::SameLine();
        if (ImGui::Button("+"))
        {
            if (!stateName.empty())
            {
                std::shared_ptr<SpriteAnimation> anim = std::make_shared<SpriteAnimation>();
                sa.State->AddState(stateName);
                sa.State->AddAnimation(anim);
                stateName.clear();
            }
        }

        ImGui::PopItemWidth();

        if (sa.State->HasAnimations())
        {
            ImGui::PushItemWidth(100);
            ImGui::SameLine();
            std::string currentAnimName = sa.State->GetCurrentState();

            if (ImGui::BeginCombo("##animation_state", currentAnimName.c_str()))
            {
                bool isSelected = false;
                for (int i = 0; i < sa.State->GetStateStorage().size(); i++)
                {
                    isSelected = currentAnimName == sa.State->GetStateStorage()[i];
                    if (ImGui::Selectable(sa.State->GetStateStorage()[i].c_str(), isSelected))
                        sa.State->SetActiveState(sa.State->GetStateStorage()[i]);

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            bool isDeleting = false;
            ImGui::SameLine();
            if (ImGui::Button("-"))
            {
                sa.State->RemoveState(sa.State->GetCurrentState());
                isDeleting = true;
            }

            if (isDeleting)
            {
                ImGui::End();
                return;
            }

            ImVec4 buttonColor = ImVec4(0.1f, 0.6f, 0.1f, 1.0f);
            ImVec4 buttonColorHovered = ImVec4(0.1f, 0.8f, 0.1f, 1.0f);


            if (sa.State->GetAnimation()->Preview)
            {
                buttonColor = ImVec4(0.6f, 0.1f, 0.1f, 1.0f);
                buttonColorHovered = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
            }

            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
            if (ImGui::Button("Preview"))
            {
                sa.State->GetAnimation()->Preview = !sa.State->GetAnimation()->Preview;
                sa.State->GetAnimation()->Reset();
            }

            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::Text("Frame");
            ImGui::SameLine();
            ImGui::InputFloat("##current_frame", &sa.State->GetAnimation()->CurrentFrame, 1.0f, 0.0f, "%.0f");
            ImGui::SameLine();
            ImGui::Text("Max Frame");
            ImGui::SameLine();
            ImGui::InputInt("##frame_max", &sa.State->GetAnimation()->MaxFrame);
            ImGui::SameLine();
            ImGui::InputFloat("##speed", &sa.State->GetAnimation()->Speed);
            ImGui::SameLine();
            ImGui::Text("Looping");
            ImGui::SameLine();
            ImGui::Checkbox("##looping", &sa.State->GetAnimation()->Looping);
            ImGui::PopItemWidth();

            SpriteAnimTimeline(sa.State->GetAnimation(), &sa.State->GetAnimation()->CurrentFrame, &expanded, &selectedEntry, &firstFrame,
                             SEQUENCER_EDIT_STARTEND | SEQUENCER_ADD | SEQUENCER_DEL | SEQUENCER_COPYPASTE | SEQUENCER_CHANGE_FRAME);
        }

        ImGui::End();
    }

    bool AnimationTimeline::SpriteAnimTimeline(std::shared_ptr<SpriteAnimation> &animation, float *currentFrame, bool *expanded, int *selectedEntry, int *firstFrame, int sequenceOptions)
    {
        bool ret = false;

        ImGuiIO &io = ImGui::GetIO();

        static int movingEntry = -1;
        static int movingPos = -1;
        static int movingPart = -1;

        int cx = (int)(io.MousePos.x);
        int cy = (int)(io.MousePos.y);
        int legendWidth = 130;

        int delEntry = -1;
        int dupEntry = -1;
        int itemHeight = 20;

        bool popupOpened = false;
        int sequenceCount = static_cast<int>(animation->GetTotalFrames());

        ImGui::BeginGroup();

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        float framePixelWidth = (canvasSize.x - legendWidth) / float(animation->MaxFrame - *firstFrame + 5.0f);

        int firstFrameUsed = firstFrame ? *firstFrame : 0;
        int controlHeight = sequenceCount * itemHeight;
        int frameCount = ImMax(animation->MaxFrame, 1);

        static bool MovingScrollBar = false;
        static bool MovingCurrentFrame = false;

        ImVec2 headerSize(canvasSize.x, (float)itemHeight);
        ImGui::InvisibleButton("topBar", headerSize);
        drawList->AddRectFilled(canvasPos, canvasPos + headerSize, IM_COL32(20, 20, 20, 255), 0);

        ImVec2 childFramePos = ImGui::GetCursorScreenPos();
        ImVec2 childFrameSize(canvasSize.x, canvasSize.y - 8.f - headerSize.y);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
        ImGui::BeginChildFrame(889, childFrameSize);

        if (sequenceCount)
            ImGui::InvisibleButton("contentBar", ImVec2(canvasSize.x, float(controlHeight)));

        const ImVec2 contentMin = ImGui::GetItemRectMin();
        const ImVec2 contentMax = ImGui::GetItemRectMax();
        const ImRect contentRect(contentMin, contentMax);
        const float contentHeight = contentMax.y - contentMin.y;

        // Legend
        drawList->AddRectFilled(canvasPos, { canvasPos.x + legendWidth, canvasPos.y + canvasSize.y }, IM_COL32(20, 20, 20, 255));
        drawList->AddRectFilled({ canvasPos.x + legendWidth, canvasPos.y }, { canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y }, IM_COL32(10, 10, 10, 255));

        ImGui::Button("Drag Here");
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                AssetHandle handle = *(AssetHandle *)payload->Data;
                SpriteAnimationFrame frame(handle);
                frame.FrameBegin += 4 * static_cast<int>(animation->GetTotalFrames());
                frame.FrameEnd += 4 * static_cast<int>(animation->GetTotalFrames());
                animation->AddFrame(frame);
            }
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SPRITESHEET_ITEM"))
            {
                SpriteSheetData data = *static_cast<SpriteSheetData *>(payload->Data);
                SpriteAnimationFrame frame(data.texture_handle);
                frame.Min = data.rect.min;
                frame.Max = data.rect.max;
                frame.FrameBegin += 4 * static_cast<int>(animation->GetTotalFrames());
                frame.FrameEnd += 4 * static_cast<int>(animation->GetTotalFrames());
                animation->AddFrame(frame);
            }
            ImGui::EndDragDropTarget();
        }

        // current frame top
        ImRect topRect(ImVec2(canvasPos.x + legendWidth, canvasPos.y), ImVec2(canvasPos.x + canvasSize.x + framePixelWidth, canvasPos.y + itemHeight));
        if (!MovingCurrentFrame && !MovingScrollBar && movingEntry == -1 && sequenceOptions & SEQUENCER_CHANGE_FRAME && currentFrame && *currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0])
            MovingCurrentFrame = true;
        if (MovingCurrentFrame)
        {
            if (frameCount)
            {
                *currentFrame = (float)(int)((io.MousePos.x - topRect.Min.x) / framePixelWidth);
                if (*currentFrame < 0.0f)
                    *currentFrame = 0.0f;
                if (*currentFrame >= animation->MaxFrame)
                    *currentFrame = (float)animation->MaxFrame;
            }
            if (!io.MouseDown[0])
                MovingCurrentFrame = false;
        }

        //header
        drawList->AddRectFilled(canvasPos, ImVec2(canvasSize.x + canvasPos.x, canvasPos.y + itemHeight), IM_COL32(20, 10, 15, 255), 0);
        
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

        for (int i = 0; i < sequenceCount; i++)
        {
            ImVec2 tpos(contentMin.x + 3, contentMin.y + i * itemHeight + 2);
            std::string tag = "SPRITE " + std::to_string(i);
            drawList->AddText(tpos, 0xFFFFFFFF, tag.c_str());
            if (sequenceOptions & SEQUENCER_DEL)
            {
                if (SequencerAddDelButton(drawList, ImVec2(contentMin.x + legendWidth - itemHeight + 2 - 10, tpos.y + 2), false))
                    delEntry = i;
            }
        }

        // slots background
        for (int i = 0; i < sequenceCount; i++)
        {
            unsigned int col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;

            ImVec2 pos = ImVec2(contentMin.x + legendWidth, contentMin.y + itemHeight * i + 1);
            ImVec2 sz = ImVec2(canvasSize.x + canvasPos.x, pos.y + itemHeight - 1);
            if (!popupOpened && cy >= pos.y && cy < pos.y + itemHeight && movingEntry == -1 && cx>contentMin.x && cx < contentMin.x + canvasSize.x)
            {
                col += 0x80201008;
                pos.x -= legendWidth;
            }
            drawList->AddRectFilled(pos, sz, col, 0);
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
            drawList->AddRectFilled(
                { contentMin.x, contentMin.y + itemHeight * *selectedEntry },
                { contentMin.x + canvasSize.x, contentMin.y + itemHeight * (*selectedEntry + 1)},
                IM_COL32(200, 120, 50, 150), 1.0f);
        }

        // slots
        for (int i = 0; i < sequenceCount; i++)
        {
            int *start, *end;
            unsigned int color;
            Get<SpriteAnimation>(animation, i, &start, &end, &color);
            ImVec2 pos = ImVec2(contentMin.x + legendWidth - firstFrameUsed * framePixelWidth, contentMin.y + itemHeight * i + 1);
            ImVec2 slotP1(pos.x + *start * framePixelWidth, pos.y + 2);
            ImVec2 slotP2(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + itemHeight - 2);
            ImVec2 slotP3(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + itemHeight - 2);
            unsigned int slotColor = color | 0xFF000000;
            unsigned int slotColorHalf = (color & 0xFFFFFF) | 0x40000000;

            if (slotP1.x <= (canvasSize.x + contentMin.x) && slotP2.x >= (contentMin.x + legendWidth))
            {
                drawList->AddRectFilled(slotP1, slotP3, slotColorHalf, 2);
                drawList->AddRectFilled(slotP1, slotP2, slotColor, 2);
            }

            // Ensure grabbable handles
            const float max_handle_width = slotP2.x - slotP1.x / 3.0f;
            const float min_handle_width = ImMin(10.0f, max_handle_width);
            const float handle_width = ImClamp(framePixelWidth / 2.0f, min_handle_width, max_handle_width);

            ImRect rects[3] = {
                ImRect(slotP1, ImVec2(slotP1.x + handle_width, slotP2.y)),
                ImRect(ImVec2(slotP2.x - handle_width, slotP1.y), slotP2),
                ImRect(slotP1, slotP2)
            };

            const unsigned int quadColor[] = { 0xFFFFFFFF, 0xFFFFFFFF, slotColor + (selected ? 0 : 0x202020) };
            if (movingEntry == -1 && (sequenceOptions & SEQUENCER_EDIT_STARTEND))
            {
                for (int j = 2; j >= 0; j--)
                {
                    ImRect &rc = rects[j];
                    if (!rc.Contains(io.MousePos))
                        continue;
                    drawList->AddRectFilled(rc.Min, rc.Max, quadColor[j], 2);
                }

                for (int j = 0; j < 3; j++)
                {
                    ImRect &rc = rects[j];
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
        }

        if (movingEntry >= 0)
        {
            ImGui::SetNextFrameWantCaptureMouse(true);

            int diffFrame = int((cx - movingPos) / framePixelWidth);
            if (std::abs(diffFrame) > 0)
            {
                int *start, *end;
                Get<SpriteAnimation>(animation, movingEntry, &start, &end, NULL);
                if (selectedEntry)
                    *selectedEntry = movingEntry;
                int &l = *start;
                int &r = *end;
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
        if (currentFrame && firstFrame && (int)*currentFrame >= *firstFrame && (int)*currentFrame <= animation->MaxFrame)
        {
            static const float cursorWidth = 4.0f;
            float cursorOffset = contentMin.x + legendWidth + ((int)*currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2 - cursorWidth * 0.5f;
            drawList->AddLine(ImVec2(cursorOffset, canvasPos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, cursorWidth);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", (int)*currentFrame);
            drawList->AddText(ImVec2(cursorOffset + 10, canvasPos.y + 2), 0xFF2A2AFF, tmps);
        }

        drawList->PopClipRect();
        drawList->PopClipRect();

        ImGui::EndChildFrame();
        ImGui::PopStyleColor();


        ImGui::EndGroup();

        if (delEntry != -1)
        {
            animation->DeleteFrame(delEntry);
            if (selectedEntry && (*selectedEntry == delEntry || *selectedEntry >= sequenceCount))
                *selectedEntry = -1;
        }

        return ret;
    }
}