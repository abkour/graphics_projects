#ifndef _FONT_ENGINE_HPP_
#define _FONT_ENINGE_HPP_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <array>
#include <map>
#include <string>

#include <glad/glad.h>
#include <shaderdirect.hpp>

#include <glm.hpp>

struct GlyphAtlas {
	std::map<unsigned char, std::array<float, 12>> texcoords;
};

class FontEngine {

	struct GlyphString {

		GlyphString(int pPposx, int pPposy, int pLength, GLuint pVao, GLuint pVbo, const glm::mat4& pModelTransform)
			: pposx(pPposx)
			, pposy(pPposy)
			, length(pLength)
			, vao(pVao)
			, vbo(pVbo)
			, model_transform(pModelTransform)
		{}

		int pposx, pposy;
		int length;
		GLuint vao, vbo;
		glm::mat4 model_transform;
	};

public:

	FontEngine(const glm::vec2 pWindowResolution);

	void InitializeFace(const char* PathToFont, const unsigned PixelSize);
	
	// Offsets in window space, angle in radians
	unsigned int MakeString(const std::string& input, const glm::vec2 LeftOffset, const glm::vec2 RightOffset, const float angle = 0.f);

	void AddSubstring(const std::string& substr, const glm::vec2 lOffset, const glm::vec2 rOffset, const unsigned int idx);

	void Render(unsigned int idx, glm::vec2 Offset, bool centered = false);
	
	void MakeAtlas(const int resx, const int resy);

	~FontEngine();

protected:

	std::vector<GlyphString> strings;

protected:

	glm::vec2 windowResolution;
	glm::mat4 ortho_projection;

	GLuint glyph_atlas;

protected:

	FT_Library library;
	FT_Face face;

	struct Glyph;
	std::map<FT_UInt, Glyph> glyphs;

	int maxGlyphHeight;

	GlyphAtlas glyphAtlas;

protected:

	ShaderWrapper fontshader;
};

#endif