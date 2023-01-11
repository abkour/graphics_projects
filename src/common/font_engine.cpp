#include "font_engine.hpp"

#include <iostream> // For debugging messages
#include <utility>

#include <gtx/transform.hpp>
#include <gtc/type_ptr.hpp>

struct FontEngine::Glyph {
	Glyph(const unsigned pGlyphIndex, int pAdvance, glm::vec2 pBearing, glm::vec2 pDimension, unsigned char* pBuffer)
		: glyph_index(pGlyphIndex)
		, advance(pAdvance)
		, bearing(pBearing)
		, dimension(pDimension)
	{
		if(pBuffer != nullptr) {
			bufferSize = dimension.x * dimension.y;
			buffer = std::make_unique<unsigned char[]>(bufferSize);
			std::memcpy(buffer.get(), pBuffer, sizeof(unsigned char) * bufferSize);
		}
		bufferSize = 0;
	}

	Glyph(const Glyph& other)
		: advance(other.advance)
		, bearing(other.bearing)
		, dimension(other.dimension)
		, bufferSize(other.bufferSize)
	{
		buffer = std::make_unique<unsigned char[]>(other.bufferSize);
		std::memcpy(buffer.get(), other.buffer.get(), bufferSize);
	}

	Glyph(Glyph&& other) 
		: advance(other.advance)
		, bearing(other.bearing)
		, dimension(other.dimension)
		, bufferSize(other.bufferSize)
	{
		buffer = std::make_unique<unsigned char[]>(bufferSize);
		buffer.swap(other.buffer);
	}

	short bufferSize;
	std::unique_ptr<unsigned char[]> buffer;
	glm::vec2 bearing;
	glm::vec2 dimension;
	int advance;
	unsigned glyph_index;
};


FontEngine::FontEngine(const glm::vec2 pWindowResolution) 
	: windowResolution(pWindowResolution)
{
	// Enable Alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Error error = FT_Init_FreeType(&library);
	if (error) {
		std::cout << "Error after call to (FT_init_FreeType)" << '\n';
	}

	ShaderWrapper tmp_shader(
		false, 
		shader_p(GL_VERTEX_SHADER, ROOT_DIRECTORY + std::string("/src/common/font.glsl.vs")),
        shader_p(GL_FRAGMENT_SHADER, ROOT_DIRECTORY + std::string("/src/common/font.glsl.fs"))
	);

	fontshader = std::move(tmp_shader);

	ortho_projection = glm::ortho(0.f, pWindowResolution.x, 0.f, pWindowResolution.y);
}

FontEngine::~FontEngine() {
	for (int i = 0; i < strings.size(); ++i) {
		glDeleteVertexArrays(1, &strings[i].vao);
		glDeleteBuffers(1, &strings[i].vbo);
	}
	glDeleteTextures(1, &glyph_atlas);
	FT_Done_Face(face);
	FT_Done_FreeType(library);
}

void FontEngine::InitializeFace(const char* PathToFont, const unsigned PixelSize) {
	FT_Error error = FT_New_Face(library, PathToFont, 0, &face);
	if (error) {
		std::cout << "Error after call to (FT_New_Face) with path: " << PathToFont << "\n";
	}

	error = FT_Set_Pixel_Sizes(face, 0, PixelSize);
	if (error) {
		std::cout << "Could not set pixel size!\n";
	}

	if (FT_HAS_KERNING(face)) {
		std::cout << "Font supports kerning!\n";
	} else {
		std::cout << "No kerning support!\n";
	}
}

void FontEngine::MakeAtlas(const int resx, const int resy) {
	std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(resx * resy);

	int budget_x = resx;
	int budget_y = resy;

	unsigned maxLineHeight = 0;

	for (int i = ' '; i < 'z'; ++i) {
		auto glyph_index = FT_Get_Char_Index(face, i);

		if (glyph_index != 0) {

			FT_Error error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
			if (error) {
				std::cout << "Error loading glyph!\n";
			}

			for (int j = 0; j < face->glyph->bitmap.rows; ++j) {
				std::memcpy(&data[((resy - budget_y) * resx) + resx - budget_x + resx * j],
							&face->glyph->bitmap.buffer[face->glyph->bitmap.width * j],
							face->glyph->bitmap.rows);
			}

			const float x00 = (resx - budget_x) / (float)resx;
			const float y00 = (resy - budget_y) / (float)resy;
			const float x01 = x00 + (face->glyph->bitmap.width / (float)resx);
			const float y01 = y00 + (face->glyph->bitmap.rows / (float)resy);
			std::array<float, 12> texcoords = 
			{
				x00, y01,
				x01, y01,
				x01, y00, 
				x00, y01,
				x01, y00,
				x00, y00
			};

			glyphAtlas.texcoords.emplace(i, texcoords);

			maxLineHeight = std::max(maxLineHeight, (unsigned)face->glyph->bitmap.rows);
			maxGlyphHeight = std::max(maxGlyphHeight, (int)face->glyph->bitmap.rows);

			auto retpair = glyphs.insert(std::make_pair(i, // Key
				// Value
				Glyph(glyph_index,
					face->glyph->advance.x / 64,
					glm::vec2(face->glyph->metrics.horiBearingX / 64, face->glyph->metrics.horiBearingY / 64),
					glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					nullptr)));
		
			budget_x -= face->glyph->bitmap.width;
			if (budget_x < 0) {
				budget_y -= maxLineHeight;
				if (budget_y >= maxLineHeight) {
					maxLineHeight = 0;
					budget_x = resx;
				} else {
					break;
				}
			}
		}
	}

	glGenTextures(1, &glyph_atlas);
	glBindTexture(GL_TEXTURE_2D, glyph_atlas);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resx, resy, 0, GL_RED, GL_UNSIGNED_BYTE, data.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

unsigned int FontEngine::MakeString(const std::string& input, 
									const glm::vec2 LeftOffset, 
									const glm::vec2 RightOffset,
									const float angle) 
{
	GLuint string_vao, string_vbo;
	glGenVertexArrays(1, &string_vao);
	glBindVertexArray(string_vao);
	glGenBuffers(1, &string_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, string_vbo);
	// Number 24 is derived from:
	// 6 vertices (2 floats each) + 6 uv coords (2 floats each)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * input.size() * 24, NULL, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

	float* vertexstore = reinterpret_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

	bool bFindWordLength = true;
	int wordlength = 0;
	int wordpos = LeftOffset.x;
	int ppos = 0;
	int pposy = 0;
	int stringsize = 0;
	for (int i = 0; i < input.size(); ++i) {
		auto it = glyphs.find(input[i]);

		// Find the length in font space of the current character string in order to determine 
		// whether we should render the character string on a new line or not.
		if (bFindWordLength && input[i] != ' ') {
			for (int j = i; j < input.size(); ++j) {
				if (input[j] == ' ') {
					break;
				} else {
					auto glyph_it = glyphs.find(input[j]);
					wordlength += glyph_it->second.advance;
				}
			}
			bFindWordLength = false;
		}
		if (input[i] == ' ') {
			bFindWordLength = true;
			wordlength = 0;
			wordpos = ppos + it->second.advance;
		}  else {
			if (wordpos + wordlength + LeftOffset.x > windowResolution.x - RightOffset.x) {
				pposy -= maxGlyphHeight * 1.3;
				wordpos = 0;
				ppos = 0;
			}
		}

		auto uvdata = glyphAtlas.texcoords.find(input[i])->second;
		
		vertexstore[i * 24] = ppos + it->second.bearing.x;
		vertexstore[i * 24 + 1] = pposy + it->second.bearing.y - it->second.dimension.y;
		vertexstore[i * 24 + 2] = uvdata[0];
		vertexstore[i * 24 + 3] = uvdata[1];
		vertexstore[i * 24 + 4] = ppos + it->second.bearing.x + it->second.dimension.x;
		vertexstore[i * 24 + 5] = pposy + it->second.bearing.y - it->second.dimension.y;
		vertexstore[i * 24 + 6] = uvdata[2];
		vertexstore[i * 24 + 7] = uvdata[3];
		vertexstore[i * 24 + 8] = ppos + it->second.bearing.x + it->second.dimension.x;
		vertexstore[i * 24 + 9] = pposy + it->second.bearing.y;
		vertexstore[i * 24 + 10] = uvdata[4];
		vertexstore[i * 24 + 11] = uvdata[5];
		vertexstore[i * 24 + 12] = ppos + it->second.bearing.x;
		vertexstore[i * 24 + 13] = pposy + it->second.bearing.y - it->second.dimension.y;
		vertexstore[i * 24 + 14] = uvdata[6];
		vertexstore[i * 24 + 15] = uvdata[7];
		vertexstore[i * 24 + 16] = ppos + it->second.bearing.x + it->second.dimension.x;
		vertexstore[i * 24 + 17] = pposy + it->second.bearing.y;
		vertexstore[i * 24 + 18] = uvdata[8];
		vertexstore[i * 24 + 19] = uvdata[9];
		vertexstore[i * 24 + 20] = ppos + it->second.bearing.x;
		vertexstore[i * 24 + 21] = pposy + it->second.bearing.y;
		vertexstore[i * 24 + 22] = uvdata[10];
		vertexstore[i * 24 + 23] = uvdata[11];

		ppos += it->second.advance;

		stringsize++;
	}
	
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glm::mat4 model_transform = glm::mat4(1.f);
	model_transform = glm::rotate(model_transform, angle, glm::vec3(0.f, 0.f, 1.f));

	strings.emplace_back(ppos, pposy, stringsize, string_vao, string_vbo, model_transform);
	return strings.size() - 1;
}

void FontEngine::AddSubstring(const std::string& substr, const glm::vec2 lOffset, const glm::vec2 rOffset, const unsigned int idx) {
	std::vector<float> vertexstore(substr.size() * 24);
	
	bool bFindWordLength = true;
	int wordlength = 0;
	int wordpos = strings[idx].pposx + lOffset.x;
	int ppos = strings[idx].pposx;
	int pposy = strings[idx].pposy;

	for (int i = 0; i < substr.size(); ++i) {
		auto it = glyphs.find(substr[i]);

		// Find the length in font space of the current character string in order to determine 
		// whether we should render the character string on a new line or not.
		if (bFindWordLength && substr[i] != ' ') {
			for (int j = i; j < substr.size(); ++j) {
				if (substr[j] == ' ') {
					break;
				} else {
					auto glyph_it = glyphs.find(substr[j]);
					wordlength += glyph_it->second.advance;
				}
			}
			bFindWordLength = false;
		}
		if (substr[i] == ' ') {
			bFindWordLength = true;
			wordlength = 0;
			wordpos = ppos + it->second.advance;
		} else {
			if (wordpos + wordlength + lOffset.x > windowResolution.x - rOffset.x) {
				pposy -= maxGlyphHeight * 1.3;
				wordpos = 0;
				ppos = 0;
			}
		}

		auto uvdata = glyphAtlas.texcoords.find(substr[i])->second;

		vertexstore[i * 24] = ppos + it->second.bearing.x;
		vertexstore[i * 24 + 1] = pposy + it->second.bearing.y - it->second.dimension.y;
		vertexstore[i * 24 + 2] = uvdata[0];
		vertexstore[i * 24 + 3] = uvdata[1];
		vertexstore[i * 24 + 4] = ppos + it->second.bearing.x + it->second.dimension.x;
		vertexstore[i * 24 + 5] = pposy + it->second.bearing.y - it->second.dimension.y;
		vertexstore[i * 24 + 6] = uvdata[2];
		vertexstore[i * 24 + 7] = uvdata[3];
		vertexstore[i * 24 + 8] = ppos + it->second.bearing.x + it->second.dimension.x;
		vertexstore[i * 24 + 9] = pposy + it->second.bearing.y;
		vertexstore[i * 24 + 10] = uvdata[4];
		vertexstore[i * 24 + 11] = uvdata[5];
		vertexstore[i * 24 + 12] = ppos + it->second.bearing.x;
		vertexstore[i * 24 + 13] = pposy + it->second.bearing.y - it->second.dimension.y;
		vertexstore[i * 24 + 14] = uvdata[6];
		vertexstore[i * 24 + 15] = uvdata[7];
		vertexstore[i * 24 + 16] = ppos + it->second.bearing.x + it->second.dimension.x;
		vertexstore[i * 24 + 17] = pposy + it->second.bearing.y;
		vertexstore[i * 24 + 18] = uvdata[8];
		vertexstore[i * 24 + 19] = uvdata[9];
		vertexstore[i * 24 + 20] = ppos + it->second.bearing.x;
		vertexstore[i * 24 + 21] = pposy + it->second.bearing.y;
		vertexstore[i * 24 + 22] = uvdata[10];
		vertexstore[i * 24 + 23] = uvdata[11];

		ppos += it->second.advance;
	}

	GLuint new_vao, new_vbo;
	glGenVertexArrays(1, &new_vao);
	glGenBuffers(1, &new_vbo);
	glBindVertexArray(new_vao);
	glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vertexstore.size() + strings[idx].length * 24), NULL, GL_STATIC_DRAW);
	
	glBindBuffer(GL_COPY_WRITE_BUFFER, new_vbo);
	glBindBuffer(GL_COPY_READ_BUFFER, strings[idx].vbo);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(float) * strings[idx].length * 24);

	// Copy new contents into new buffer
	glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * (strings[idx].length * 24), vertexstore.size() * sizeof(float), vertexstore.data());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

	glDeleteVertexArrays(1, &strings[idx].vao);
	glDeleteBuffers(1, &strings[idx].vbo);
	
	strings[idx].vao = new_vao;
	strings[idx].vbo = new_vbo;;
	strings[idx].length += substr.size();
	strings[idx].pposx = ppos;
	strings[idx].pposy = pposy;
}

void FontEngine::Render(unsigned int idx, glm::vec2 Offset, bool centered) {
	if(centered) {
		Offset.x -= strings[idx].pposx / 2.f;
	}

	fontshader.bind();
	fontshader.upload2fv(&Offset.x, "offset");
	fontshader.upload44fm(glm::value_ptr(strings[idx].model_transform), "model");
	fontshader.upload44fm(glm::value_ptr(ortho_projection), "projection");
	glBindTexture(GL_TEXTURE_2D, glyph_atlas);
	glBindTextureUnit(0, glyph_atlas);
	glBindVertexArray(strings[idx].vao);
	glDrawArrays(GL_TRIANGLES, 0, strings[idx].length * 6);
}