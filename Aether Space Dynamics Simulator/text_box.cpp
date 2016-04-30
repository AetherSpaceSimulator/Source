#include "stdafx.h"
#include "text_box.h"

text_box_class::text_box_class()
{
	text_size_ = 0;
	enabled = true;
	has_focus = false;
	texto = "";
	texto_real = texto;
	depends_on = 0;
	type = 0;

	m_Text = nullptr;
}

text_box_class::text_box_class(const text_box_class& other)
{
}


text_box_class::~text_box_class()
{
	SAFE_DELETE(m_Text);
}

bool text_box_class::Initialize(Matrix ortho_, HWND hwnd, int screenWidth, int screenHeight,
	int pos_x, int pos_y, Color _color_, int depends_on_, int max_size_, int type_)
{
	m_Text = new TextClass;
	if (!m_Text) return false;

	if (!m_Text->Initialize(ortho_, hwnd, screenWidth, screenHeight)
		|| !m_Text->create_sentence_array(1)
		|| !m_Text->InitializeSentence(0, int(texto.size() + 1), true)) return false;

	depends_on = depends_on_;
	color_ = _color_;
	position_x = pos_x;
	position_y = pos_y;
	original_pos_X = pos_x;
	original_pos_Y = pos_y;
	text_size_ = int(texto.size() + 1);
	max_size = max_size_;
	type = type_;

	return true;
}

bool text_box_class::render()
{
	if (!m_Text->Render()) return false;

	return true;
}

bool text_box_class::update()
{
	if (int(texto.size() + 1) != text_size_)
	{
		if (!m_Text->InitializeSentence(0, int(texto.size() + 1), true)) return false;
		text_size_ = int(texto.size() + 1);
	}

	if (!m_Text->update_sentence(texto, position_x, position_y, color_, 0)) return false;

	return true;
}

int text_box_class::get_last_pixel_position(int array_number)
{
	return m_Text->get_last_pixel_position(array_number);
}