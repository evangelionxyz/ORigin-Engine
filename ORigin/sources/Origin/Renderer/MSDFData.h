// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#undef INFINITE
#include "msdf-atlas-gen.h"

namespace origin {

	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};

}