#include "stdafx.h"
#include "font.h"

FontClass::FontClass()
{
	m_Font = nullptr;
	m_Texture = nullptr;
}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
	SAFE_DELETE(m_Texture);
	SAFE_DELETE_ARRAY(m_Font);
}

bool FontClass::Initialize(char* fontFilename, std::string textureFilename)
{
	bool result;


	// Load in the text file containing the font data.
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// Load the texture that has the font characters on it.
	result = LoadTexture(textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool FontClass::LoadFontData(char* filename)
{
	std::ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (i = 0; i<95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}

bool FontClass::LoadTexture(std::string filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new textureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->load_texture(filename);
	if (!result)
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->get_Texture();
}

void FontClass::BuildVertexArray(void* vertices, std::string sentence, float drawX_, float drawY)
{
	drawX = drawX_;
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = int(sentence.size());

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for (i = 0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			drawX = drawX + 5.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = Vector3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = Vector2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = Vector3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = Vector2(m_Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = Vector3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = Vector2(m_Font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = Vector3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = Vector2(m_Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = Vector3(drawX + m_Font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = Vector2(m_Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = Vector3((drawX + m_Font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = Vector2(m_Font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + m_Font[letter].size + 2.0f;
		}
	}

	last_X = int(drawX);

	return;
}

int FontClass::get_last_pixel_position()
{
	return last_X;
}