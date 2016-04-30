////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "font.h"
#include "font_shader.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:

	struct SentenceType
	{
		SentenceType()
		{
			last_X = 0;
			visible = true;

			vertexBuffer = nullptr;
			indexBuffer = nullptr;
		};

		~SentenceType()
		{
			SAFE_RELEASE(vertexBuffer);
			SAFE_RELEASE(indexBuffer);
		};

		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		Color Dcolor;
		bool visible;
		int last_X;
	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool update_sentence(std::string texto,
		int pos_x, int pos_y, Color color_, int array_number);
	bool Initialize(Matrix ortho_, HWND hwnd, int screenWidth, int screenHeight);
	bool Render();

	int get_last_pixel_position(int array_number);

	bool InitializeSentence(int array_number, int maxLength, bool visible_);
	bool UpdateSentence(SentenceType* sentence, std::string text, int positionX, int positionY, Color color_);
	bool create_sentence_array(int array_number);
	void delete_m_sentence();
	void set_visible(int array_number, bool visible_);

private:
	bool RenderSentence(SentenceType&);

private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;
	int m_screenWidth, m_screenHeight;
	Matrix ortho_matrix;
	int total_sentences;

	SentenceType* m_sentence;
};

#endif