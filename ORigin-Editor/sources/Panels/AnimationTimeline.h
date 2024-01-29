#pragma once
#include "imgui.h"
#include "Origin\Animation\Animation.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "ImCurveEdit.h"

#include <math.h>
#include <vector>
#include <algorithm>

namespace origin {

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

	enum SEQUENCER_OPTIONS
	{
		SEQUENCER_EDIT_NONE = 0,
		SEQUENCER_EDIT_STARTEND = 1 << 1,
		SEQUENCER_CHANGE_FRAME = 1 << 3,
		SEQUENCER_ADD = 1 << 4,
		SEQUENCER_DEL = 1 << 5,
		SEQUENCER_COPYPASTE = 1 << 6,
		SEQUENCER_EDIT_ALL = SEQUENCER_EDIT_STARTEND | SEQUENCER_CHANGE_FRAME
	};

	enum class AnimationType
	{
		None = -1,
		Sprite = 0
	};

	static const char* AnimationTypeToString(AnimationType type)
	{
		switch (type)
		{
		case AnimationType::None: return "Invalid";
		case AnimationType::Sprite: return "Sprite";
		}

		return "Invalid";
	} 

	struct AnimationItem
	{
		AnimationType Type;
		int FrameStart, FrameEnd;
		bool Expanded;
		Animation Anim;
	};

	class AnimationTimeline
	{
	public:
		AnimationTimeline();
		void OnImGuiRender();

		void BeginEdit(int entry);
		void EndEdit();
		int GetFrameMin() const { return m_FrameMin; }
		int GetFrameMax() const { return m_FrameMax; }
		int GetItemCount() const { return (int)m_Items.size(); }
		const char* GetItemLabel(int index) const;
		void Get(int index, int** start, int** end, int* type, unsigned int* color);
		void Add(AnimationType type);
		void Del(int index);
		void DoubleClick(int index);
		void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect);
		void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect);
		size_t GetCustomHeight(int index) { return m_Items[index].Expanded ? 300 : 0; }
		const char* GetCollapseFmt() const { return "%d Frames / %d entries"; }

		int m_FrameMin = 0, m_FrameMax = 60;
		std::vector<AnimationItem> m_Items;
		RampEdit rampEdit;
	};

	bool Timeline(AnimationTimeline* timeline, int* currentFrame, bool* expanded, int* selectedEntry, int* firstFrame, int sequenceOptions);
}

