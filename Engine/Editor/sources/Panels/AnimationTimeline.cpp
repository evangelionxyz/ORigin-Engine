// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "AnimationTimeline.hpp"

#include <algorithm>

#include "Origin/Asset/AssetManager.h"
#include "Origin/Asset/EditorAssetManager.h"
#include "Origin/Project/Project.h"
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
        const i32 button_color = overBtn ? 0xAAEAFFAA : 0x77A3B2AA;
        if (containedClick && io.MouseDownDuration[0] > 0)
        {
            btnRect.Expand(2.0f);
        }

        const f32 mid_x = pos.x + 16 / 2 - 0.5f;
        const f32 mid_y = pos.y + 16 / 2 - 0.5f;
        
        draw_list->AddRect(btnRect.Min, btnRect.Max, button_color, 4);
        draw_list->AddLine(ImVec2(btnRect.Min.x + 3, mid_y), ImVec2(btnRect.Max.x - 3, mid_y), button_color, 2);
        if (add)
            draw_list->AddLine(ImVec2(mid_x, btnRect.Min.y + 3), ImVec2(mid_x, btnRect.Max.y - 3), button_color, 2);
        return clickedBtn;
    }

    template<typename T>
    void Get(const std::shared_ptr<T> &anim, i32 index, i32 **start, i32 **end, u32 *color)
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
        static i32 selectedEntry = -1;
        static i32 firstFrame = 0;
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
        {
            stateName = std::string(buffer);
        }

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
                for (i32 i = 0; i < sa.State->GetStateStorage().size(); i++)
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

    bool AnimationTimeline::SpriteAnimTimeline(std::shared_ptr<SpriteAnimation> &animation, f32 *currentFrame, bool *expanded, i32 *selectedEntry, i32 *firstFrame, i32 sequenceOptions)
    {
        bool ret = false;

        ImGuiIO &io = ImGui::GetIO();

        static i32 moving_entry = -1;
        static i32 moving_pos = -1;
        static i32 moving_part = -1;

        i32 cx = static_cast<i32>(io.MousePos.x);
        i32 cy = static_cast<i32>(io.MousePos.y);
        i32 legend_width = 130;

        i32 delete_entry = -1;
        i32 item_height = 20;

        bool popup_opened = false;
        i32 sequence_count = static_cast<int>(animation->GetTotalFrames());

        ImGui::BeginGroup();

        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();

        f32 framePixelWidth = (canvas_size.x - legend_width) / f32(animation->MaxFrame - *firstFrame + 5.0f);

        i32 firstFrameUsed = firstFrame ? *firstFrame : 0;
        i32 controlHeight = sequence_count * item_height;
        i32 frameCount = ImMax(animation->MaxFrame, 1);

        static bool MovingScrollBar = false;
        static bool MovingCurrentFrame = false;

        ImVec2 headerSize(canvas_size.x, (f32)item_height);
        ImGui::InvisibleButton("topBar", headerSize);
        draw_list->AddRectFilled(canvas_pos, canvas_pos + headerSize, IM_COL32(20, 20, 20, 255), 0);

        ImVec2 childFramePos = ImGui::GetCursorScreenPos();
        ImVec2 childFrameSize(canvas_size.x, canvas_size.y - 8.f - headerSize.y);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, 0);
        ImGui::BeginChildFrame(889, childFrameSize);

        if (sequence_count)
            ImGui::InvisibleButton("contentBar", ImVec2(canvas_size.x, f32(controlHeight)));

        const ImVec2 contentMin = ImGui::GetItemRectMin();
        const ImVec2 contentMax = ImGui::GetItemRectMax();
        const f32 contentHeight = contentMax.y - contentMin.y;

        // Legend
        draw_list->AddRectFilled(canvas_pos, { canvas_pos.x + legend_width, canvas_pos.y + canvas_size.y }, IM_COL32(20, 20, 20, 255));
        draw_list->AddRectFilled({ canvas_pos.x + legend_width, canvas_pos.y }, { canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y }, IM_COL32(10, 10, 10, 255));

        ImGui::Button("Drag Here", {static_cast<f32>(legend_width), 25.0f});
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
                auto metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(handle);

                // insert all of sprite data
                if (metadata.Type == AssetType::SpritesSheet)
                {
                    Ref<SpriteSheet> sprite_sheet = AssetManager::GetAsset<SpriteSheet>(handle);
                    for (SpriteSheetData data : sprite_sheet->Sprites)
                    {
                        SpriteAnimationFrame frame(sprite_sheet->GetTextureHandle());
                        frame.UV0 = data.uv0;
                        frame.UV1 = data.uv1;

                        frame.FrameBegin += 4 * static_cast<int>(animation->GetTotalFrames());
                        frame.FrameEnd += 4 * static_cast<int>(animation->GetTotalFrames());
                        
                        animation->AddFrame(frame);
                    }
                }
                
                // single texture 2D
                else if (metadata.Type == AssetType::Texture)
                {
                    SpriteAnimationFrame frame(handle);
                    frame.FrameBegin += 4 * static_cast<int>(animation->GetTotalFrames());
                    frame.FrameEnd += 4 * static_cast<int>(animation->GetTotalFrames());
                    animation->AddFrame(frame);
                }
            }

            // single sprite sheet data
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SPRITESHEET_ITEM"))
            {
                auto [rect, atlas_size, uv0, uv1, texture_handle] = *static_cast<SpriteSheetData *>(payload->Data);
                SpriteAnimationFrame frame(texture_handle);
                frame.UV0 = uv0;
                frame.UV1 = uv1;
                frame.FrameBegin += 4 * static_cast<int>(animation->GetTotalFrames());
                frame.FrameEnd += 4 * static_cast<int>(animation->GetTotalFrames());
                animation->AddFrame(frame);
            }
            
            ImGui::EndDragDropTarget();
        }

        // current frame top
        ImRect topRect(ImVec2(canvas_pos.x + legend_width, canvas_pos.y), ImVec2(canvas_pos.x + canvas_size.x + framePixelWidth, canvas_pos.y + item_height));
        if (!MovingCurrentFrame && !MovingScrollBar && moving_entry == -1 && sequenceOptions & SEQUENCER_CHANGE_FRAME && currentFrame && *currentFrame >= 0 && topRect.Contains(io.MousePos) && io.MouseDown[0])
            MovingCurrentFrame = true;
        if (MovingCurrentFrame)
        {
            if (frameCount)
            {
                *currentFrame = (f32)(i32)((io.MousePos.x - topRect.Min.x) / framePixelWidth);
                *currentFrame = std::max(*currentFrame, 0.0f);
                if (*currentFrame >= animation->MaxFrame)
                    *currentFrame = (f32)animation->MaxFrame;
            }
            if (!io.MouseDown[0])
                MovingCurrentFrame = false;
        }

        //header
        draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_size.x + canvas_pos.x, canvas_pos.y + item_height), IM_COL32(20, 10, 15, 255), 0);
        
        //header frame number and lines
        i32 modFrameCount = 10;
        i32 frameStep = 1;
        while ((modFrameCount * framePixelWidth) < 150)
        {
            modFrameCount *= 2;
            frameStep *= 2;
        }
        
        i32 halfModFrameCount = modFrameCount / 2;

        auto drawLine = [&](i32 i, i32 regionHeight)
            {
                bool baseIndex = ((i % modFrameCount) == 0) || (i == animation->MaxFrame || i == 0);
                bool halfIndex = (i % halfModFrameCount) == 0;
                i32 px = (i32)canvas_pos.x + int(i * framePixelWidth) + legend_width - int(firstFrameUsed * framePixelWidth);
                i32 tiretStart = baseIndex ? 4 : (halfIndex ? 10 : 14);
                i32 tiretEnd = baseIndex ? regionHeight : item_height;

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legend_width))
                {
                    draw_list->AddLine(ImVec2(px, canvas_pos.y + tiretStart), ImVec2((f32)px, canvas_pos.y + (f32)tiretEnd - 1), 0xFF606060, 1);
                    draw_list->AddLine(ImVec2(px, canvas_pos.y + item_height), ImVec2((f32)px, canvas_pos.y + (f32)regionHeight - 1), 0x30606060, 1);
                }

                if (baseIndex && px > (canvas_pos.x + legend_width))
                {
                    char tmps[512];
                    ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", i);
                    draw_list->AddText(ImVec2((f32)px + 3.f, canvas_pos.y), 0xFFBBBBBB, tmps);
                }
            };

        auto drawLineContent = [&](i32 i, int)
            {
                i32 px = (i32)canvas_pos.x + int(i * framePixelWidth) + legend_width - int(firstFrameUsed * framePixelWidth);
                i32 tiretStart = int(contentMin.y);
                i32 tiretEnd = int(contentMax.y);

                if (px <= (canvas_size.x + canvas_pos.x) && px >= (canvas_pos.x + legend_width))
                    draw_list->AddLine(ImVec2(f32(px), f32(tiretStart)), ImVec2(f32(px), f32(tiretEnd)), 0x30606060, 1);
            };

        for (i32 i = 0; i <= animation->MaxFrame; i += frameStep)
            drawLine(i, item_height);

        drawLine(0, item_height);
        drawLine(animation->MaxFrame, item_height);

        draw_list->PushClipRect(childFramePos, childFramePos + childFrameSize, true);

        for (i32 i = 0; i < sequence_count; i++)
        {
            ImVec2 tpos(contentMin.x + 3, contentMin.y + i * item_height + 2);
            std::string tag = "SPRITE " + std::to_string(i);
            draw_list->AddText(tpos, 0xFFFFFFFF, tag.c_str());
            if (sequenceOptions & SEQUENCER_DEL)
            {
                if (SequencerAddDelButton(draw_list, ImVec2(contentMin.x + legend_width - item_height + 2 - 10, tpos.y + 2), false))
                    delete_entry = i;
            }
        }

        // slots background
        for (i32 i = 0; i < sequence_count; i++)
        {
            u32 col = (i & 1) ? 0xFF3A3636 : 0xFF413D3D;

            ImVec2 pos = ImVec2(contentMin.x + legend_width, contentMin.y + item_height * i + 1);
            ImVec2 sz = ImVec2(canvas_size.x + canvas_pos.x, pos.y + item_height - 1);
            if (!popup_opened && cy >= pos.y && cy < pos.y + item_height && moving_entry == -1 && cx>contentMin.x && cx < contentMin.x + canvas_size.x)
            {
                col += 0x80201008;
                pos.x -= legend_width;
            }
            draw_list->AddRectFilled(pos, sz, col, 0);
        }

        draw_list->PushClipRect(childFramePos + ImVec2(f32(legend_width), 0.f), childFramePos + childFrameSize, true);

        // vertical frame lines in content area
        for (i32 i = 0; i <= animation->MaxFrame; i += frameStep)
        {
            drawLineContent(i, int(contentHeight));
        }
        drawLineContent(0, int(contentHeight));
        drawLineContent(0, int(contentHeight));

        // selection
        bool selected = selectedEntry && (*selectedEntry >= 0);
        if (selected)
        {
            draw_list->AddRectFilled(
                { contentMin.x, contentMin.y + item_height * *selectedEntry },
                { contentMin.x + canvas_size.x, contentMin.y + item_height * (*selectedEntry + 1)},
                IM_COL32(200, 120, 50, 150), 1.0f);
        }

        // slots
        for (i32 i = 0; i < sequence_count; i++)
        {
            i32 *start, *end;
            u32 color;
            Get<SpriteAnimation>(animation, i, &start, &end, &color);
            ImVec2 pos = ImVec2(contentMin.x + legend_width - firstFrameUsed * framePixelWidth, contentMin.y + item_height * i + 1);
            ImVec2 slotP1(pos.x + *start * framePixelWidth, pos.y + 2);
            ImVec2 slotP2(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + item_height - 2);
            ImVec2 slotP3(pos.x + *end * framePixelWidth + framePixelWidth, pos.y + item_height - 2);
            u32 slotColor = color | 0xFF000000;
            u32 slotColorHalf = (color & 0xFFFFFF) | 0x40000000;

            if (slotP1.x <= (canvas_size.x + contentMin.x) && slotP2.x >= (contentMin.x + legend_width))
            {
                draw_list->AddRectFilled(slotP1, slotP3, slotColorHalf, 2);
                draw_list->AddRectFilled(slotP1, slotP2, slotColor, 2);
            }

            // Ensure grabbable handles
            const f32 max_handle_width = slotP2.x - slotP1.x / 3.0f;
            const f32 min_handle_width = ImMin(10.0f, max_handle_width);
            const f32 handle_width = ImClamp(framePixelWidth / 2.0f, min_handle_width, max_handle_width);

            ImRect rects[3] = {
                ImRect(slotP1, ImVec2(slotP1.x + handle_width, slotP2.y)),
                ImRect(ImVec2(slotP2.x - handle_width, slotP1.y), slotP2),
                ImRect(slotP1, slotP2)
            };

            const u32 quadColor[] = { 0xFFFFFFFF, 0xFFFFFFFF, slotColor + (selected ? 0 : 0x202020) };
            if (moving_entry == -1 && (sequenceOptions & SEQUENCER_EDIT_STARTEND))
            {
                for (i32 j = 2; j >= 0; j--)
                {
                    ImRect &rc = rects[j];
                    if (!rc.Contains(io.MousePos))
                        continue;
                    draw_list->AddRectFilled(rc.Min, rc.Max, quadColor[j], 2);
                }

                for (i32 j = 0; j < 3; j++)
                {
                    ImRect &rc = rects[j];
                    if (!rc.Contains(io.MousePos))
                        continue;
                    if (!ImRect(childFramePos, childFramePos + childFrameSize).Contains(io.MousePos))
                        continue;
                    if (ImGui::IsMouseClicked(0) && !MovingScrollBar && !MovingCurrentFrame)
                    {
                        moving_entry = i;
                        moving_pos = cx;
                        moving_part = j + 1;
                        break;
                    }
                }
            }
        }

        if (moving_entry >= 0)
        {
            ImGui::SetNextFrameWantCaptureMouse(true);

            i32 diffFrame = i32((cx - moving_pos) / framePixelWidth);
            if (std::abs(diffFrame) > 0)
            {
                i32 *start, *end;
                Get<SpriteAnimation>(animation, moving_entry, &start, &end, NULL);
                if (selectedEntry)
                    *selectedEntry = moving_entry;
                i32 &l = *start;
                i32 &r = *end;
                if (moving_part & 1)
                    l += diffFrame;
                if (moving_part & 2)
                    r += diffFrame;
                if (l < 0)
                {
                    if (moving_part & 2)
                        r -= l;
                    l = 0;
                }
                if (moving_part & 1 && l > r)
                    l = r;
                if (moving_part & 2 && r < l)
                    r = l;
                moving_pos += i32(diffFrame * framePixelWidth);
            }
            if (!io.MouseDown[0])
            {
                // single select
                if (!diffFrame && moving_part && selectedEntry)
                {
                    *selectedEntry = moving_entry;
                    ret = true;
                }
                moving_entry = -1;
            }
        }

        // cursor
        if (currentFrame && firstFrame && (i32)*currentFrame >= *firstFrame && (i32)*currentFrame <= animation->MaxFrame)
        {
            static const f32 cursorWidth = 4.0f;
            f32 cursorOffset = contentMin.x + legend_width + ((i32)*currentFrame - firstFrameUsed) * framePixelWidth + framePixelWidth / 2 - cursorWidth * 0.5f;
            draw_list->AddLine(ImVec2(cursorOffset, canvas_pos.y), ImVec2(cursorOffset, contentMax.y), 0xA02A2AFF, cursorWidth);
            char tmps[512];
            ImFormatString(tmps, IM_ARRAYSIZE(tmps), "%d", (i32)*currentFrame);
            draw_list->AddText(ImVec2(cursorOffset + 10, canvas_pos.y + 2), 0xFF2A2AFF, tmps);
        }

        draw_list->PopClipRect();
        draw_list->PopClipRect();

        ImGui::EndChildFrame();
        ImGui::PopStyleColor();

        ImGui::EndGroup();

        if (delete_entry != -1)
        {
            animation->DeleteFrame(delete_entry);
            if (selectedEntry && (*selectedEntry == delete_entry || *selectedEntry >= sequence_count))
            {
                *selectedEntry = -1;
            }
        }

        return ret;
    }
}