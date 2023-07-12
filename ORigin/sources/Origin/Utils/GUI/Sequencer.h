// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <ImSequencer.h>

namespace origin {

	class Sequencer
	{
	public:
		Sequencer();
		~Sequencer();

		void Add(int frameIndex);
		void Delete(int frameIndex);
		void Duplicate(int frameIndex);

		void Draw(int frameIndex, int numFrames, int currentFrame, ImDrawList* drawList);

		void Begin();
		void End();

	private:
		ImSequencer::SequenceInterface* m_Sequencer;
		int m_CurrentFrame;
		int m_FirstFrameIndex;
		int m_SelectedEntry;
		int m_Options;
		bool m_Expanded = false;
	};
}

