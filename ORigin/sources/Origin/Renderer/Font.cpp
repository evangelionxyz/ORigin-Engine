// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin\Core\Application.h"
#include "Font.h"

#include "GlyphGeometry.h"
#include "FontGeometry.h"
#include "BitmapAtlasStorage.h"

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin {

	static std::vector<std::shared_ptr<Font> *> s_Fonts;
	static std::vector<std::future<void>> s_Futures;
	static std::vector<FontData *> s_FontData;
	static std::mutex s_FontMutex;

	template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
	static void CreateAndCacheAtlas(FontData *data)
	{
		OGN_PROFILER_RENDERING();

		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(data->Width, data->Height);
		generator.setAttributes(attributes);
		generator.setThreadCount(8);
		generator.generate(data->Glyphs.data(), static_cast<int>(data->Glyphs.size()));

		msdfgen::BitmapConstRef<T, N> bitmap = static_cast<msdfgen::BitmapConstRef<T, N>>(generator.atlasStorage());

		Buffer b = Buffer((void *)bitmap.pixels, bitmap.width * bitmap.height * 3);
		data->Buffer = Buffer::Copy(b);

		data->TexSpec.Format = ImageFormat::RGB8;
		data->TexSpec.Width = bitmap.width;
		data->TexSpec.Height = bitmap.height;
		data->TexSpec.GenerateMips = false;
	};

	static FontData *LoadFontData(const std::filesystem::path &filepath)
	{
		OGN_PROFILER_RENDERING();

		FontData *data = new FontData();
		msdfgen::FreetypeHandle *ft = msdfgen::initializeFreetype();
		OGN_CORE_ASSERT(ft, "[Font] FreeTypeHandle Error");

		std::string fileString = filepath.string();
		msdfgen::FontHandle *font = msdfgen::loadFont(ft, fileString.c_str());
		if (!font)
		{
			OGN_CORE_ERROR("[Font] Failed to load font {}", fileString);
			return nullptr;
		}

		struct CharsetRange
		{
			uint32_t Begin, End;
		};

		// From imgui_draw.cpp
		static const CharsetRange charsetRanges[] =
		{
			{0x0020, 0x00FF}
		};

		msdf_atlas::Charset charset;
		for (CharsetRange range : charsetRanges)
		{
			for (uint32_t c = range.Begin; c <= range.End; c++)
				charset.add(c);
		}
		double fontScale = 1.0;

		data->FontGeometry = msdf_atlas::FontGeometry(&data->Glyphs);
		int glyphsLoaded = data->FontGeometry.loadCharset(font, fontScale, charset);

		OGN_CORE_INFO("[Font] Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

		double emSize = 40.0;
		msdf_atlas::TightAtlasPacker atlasPacker;

		atlasPacker.setPixelRange(2.0);
		atlasPacker.setMiterLimit(1.0);
		atlasPacker.setPadding(0);
		atlasPacker.setScale(emSize);

		int remaining = atlasPacker.pack(data->Glyphs.data(), static_cast<int>(data->Glyphs.size()));
		OGN_CORE_ASSERT(remaining == 0, "");

		atlasPacker.getDimensions(data->Width, data->Height);

		emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8

		uint64_t coloringSeed = 0;
		bool expensiveColoring = false;
		if (expensiveColoring)
		{
			msdf_atlas::Workload([&glyphs = data->Glyphs, &coloringSeed](int i, int threadNo)->bool
			{
				unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT) * !!coloringSeed;
				glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
				return true;
			},
				data->Glyphs.size()).finish(THREAD_COUNT);
		}
		else
		{
			unsigned long long glyphSeed = coloringSeed;
			for (msdf_atlas::GlyphGeometry &glyph : data->Glyphs)
			{
				glyphSeed *= LCG_MULTIPLIER;
				glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
			}
		}

		CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>(data);

		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);

		return data;
	}

	static void LoadFont(std::vector<FontData *> *fontsData, std::filesystem::path filepath)
	{
		FontData *data = LoadFontData(filepath);
		std::unique_lock<std::mutex> lock(s_FontMutex);
		fontsData->push_back(data);

		OGN_CORE_WARN("Thread Id: {}", std::this_thread::get_id());
	}

	void FontLoader::LoadFontAsync(std::shared_ptr<Font> *font, std::filesystem::path filepath)
	{
		{
			std::unique_lock<std::mutex> lock(s_FontMutex);
			s_Fonts.push_back(font);
		}

		s_Futures.push_back(std::async(std::launch::async, LoadFont, &s_FontData, filepath));
	}

	Font::Font(FontData *data)
		: m_Data(data)
	{
		m_AtlasTexture = Texture2D::Create(m_Data->TexSpec);
		m_AtlasTexture->SetData(m_Data->Buffer);

		m_Loaded = true;
	}

	Font::~Font()
	{
		if (m_Data) delete m_Data;
	}

	void Font::CheckChanges()
	{
		OGN_CORE_TRACE("Total loaded fonts {}", s_Fonts.size());

		for (int i = 0; i < s_FontData.size(); i++)
		{
			const auto &font = s_Fonts[i];
			const auto &data = s_FontData[i];

			if (data)
			{
				*font = std::make_shared<Font>(data);

				s_FontData.erase(s_FontData.begin() + i);
				s_Fonts.erase(s_Fonts.begin() + i);
				s_Futures.erase(s_Futures.begin() + i);
				i--;
			}
		}

		OGN_CORE_TRACE("Cleared {}", s_Fonts.size());
	}

}