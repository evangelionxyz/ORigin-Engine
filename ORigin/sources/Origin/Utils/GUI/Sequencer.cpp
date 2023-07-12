// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Sequencer.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace origin {
	Sequencer::Sequencer()
	{
		if (ImSequencer::Sequencer(m_Sequencer, &m_CurrentFrame, &m_Expanded, &m_SelectedEntry, &m_FirstFrameIndex, m_Options) == false)
			OGN_CORE_ERROR("Sequencer Error: Couldn't Create Sequencer");
	}

	Sequencer::~Sequencer()
	{
	}

	void Sequencer::Add(int indexFrame)
	{
		m_Sequencer->Add(indexFrame);
	}

	void Sequencer::Delete(int frameIndex)
	{
		m_Sequencer->Del(frameIndex);
	}

	void Sequencer::Duplicate(int frameIndex)
	{
		m_Sequencer->Duplicate(frameIndex);
	}

	void Sequencer::Draw(int frameIndex, int numFrames, int currentFrame, ImDrawList* drawList)
	{
		m_CurrentFrame = currentFrame;

		for (; frameIndex < numFrames; ++frameIndex)
		{
			ImRect frameRect(ImVec2(1.0f, 1.0f), ImVec2(128.0f, 72.0f));

			// Determine the frame color based on its state (e.g., selected, active, etc.)
			ImU32 frameColor = (frameIndex == currentFrame) ? IM_COL32(255, 255, 255, 255) : IM_COL32(128, 128, 128, 255);

			drawList->AddRectFilled(frameRect.Min, frameRect.Max, frameColor);
		}
	}

	void Sequencer::Begin()
	{
		m_Sequencer->BeginEdit(0);
	}

	void Sequencer::End()
	{
		m_Sequencer->EndEdit();
	}

}
