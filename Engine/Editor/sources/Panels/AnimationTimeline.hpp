// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"

#include "Origin/Animation/Animation.h"
#include "Origin/Animation/AnimationState.h"
#include "Origin/Scene/Entity.h"

#include "imgui_internal.h"
#include "ImCurveEdit.h"
#include "Origin/Core/Time.h"
#include <math.h>
#include <vector>
#include <algorithm>

namespace origin {

    enum SEQUENCER_OPTIONS
    {
        SEQUENCER_EDIT_NONE     = BIT(0),
        SEQUENCER_EDIT_STARTEND = BIT(1),
        SEQUENCER_CHANGE_FRAME  = BIT(3),
        SEQUENCER_ADD           = BIT(4),
        SEQUENCER_DEL           = BIT(5),
        SEQUENCER_COPYPASTE     = BIT(6),
        SEQUENCER_EDIT_ALL      = SEQUENCER_EDIT_STARTEND | SEQUENCER_CHANGE_FRAME
    };

    struct RampEdit : public ImCurveEdit::Delegate
    {
        RampEdit()
        {
            mPts[0][0] = ImVec2(-10.f, 0);
            mPts[0][1] = ImVec2(20.f, 0.6f);
            mPts[0][2] = ImVec2(25.f, 0.2f);
            mPts[0][3] = ImVec2(70.f, 0.4f);
            mPts[0][4] = ImVec2(120.f, 1.f);
            mPointCount[0] = 5;

            mPts[1][0] = ImVec2(-50.f, 0.2f);
            mPts[1][1] = ImVec2(33.f, 0.7f);
            mPts[1][2] = ImVec2(80.f, 0.2f);
            mPts[1][3] = ImVec2(82.f, 0.8f);
            mPointCount[1] = 4;


            mPts[2][0] = ImVec2(40.f, 0);
            mPts[2][1] = ImVec2(60.f, 0.1f);
            mPts[2][2] = ImVec2(90.f, 0.82f);
            mPts[2][3] = ImVec2(150.f, 0.24f);
            mPts[2][4] = ImVec2(200.f, 0.34f);
            mPts[2][5] = ImVec2(250.f, 0.12f);
            mPointCount[2] = 6;
            mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
            mMax = ImVec2(1.f, 1.f);
            mMin = ImVec2(0.f, 0.f);
        }

        size_t GetCurveCount()
        {
            return 3;
        }

        bool IsVisible(size_t curveIndex)
        {
            return mbVisible[curveIndex];
        }
        size_t GetPointCount(size_t curveIndex)
        {
            return mPointCount[curveIndex];
        }

        uint32_t GetCurveColor(size_t curveIndex)
        {
            uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
            return cols[curveIndex];
        }
        ImVec2* GetPoints(size_t curveIndex)
        {
            return mPts[curveIndex];
        }
        virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
        virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
        {
            mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
            SortValues(curveIndex);
            for (size_t i = 0; i < GetPointCount(curveIndex); i++)
            {
                if (mPts[curveIndex][i].x == value.x)
                    return (int)i;
            }
            return pointIndex;
        }
        virtual void AddPoint(size_t curveIndex, ImVec2 value)
        {
            if (mPointCount[curveIndex] >= 8)
                return;
            mPts[curveIndex][mPointCount[curveIndex]++] = value;
            SortValues(curveIndex);
        }
        virtual ImVec2& GetMax() { return mMax; }
        virtual ImVec2& GetMin() { return mMin; }
        virtual unsigned int GetBackgroundColor() { return 0; }
        ImVec2 mPts[3][8];
        size_t mPointCount[3];
        bool mbVisible[3];
        ImVec2 mMin;
        ImVec2 mMax;

    private:
        void SortValues(size_t curveIndex)
        {
            auto b = std::begin(mPts[curveIndex]);
            auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
            std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });
        }
    };

    class AnimationTimeline
    {
    public:
        static void DrawSpriteAnimTimeline(SpriteAnimationComponent &anim);
        static bool SpriteAnimTimeline(std::shared_ptr<SpriteAnimation> &anim, float *currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions);
    };
}

