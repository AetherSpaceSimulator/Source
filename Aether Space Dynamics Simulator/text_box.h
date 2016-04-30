#ifndef _TEXTBOX_H_
#define _TEXTBOX_H_

#include "stdafx.h"
#include "text.h"
#include "colors.h"

class text_box_class
{
public:

	text_box_class();
	text_box_class(const text_box_class&);
	~text_box_class();

	bool Initialize(Matrix ortho_, HWND hwnd, int screenWidth, int screenHeight,
		int pos_x, int pos_y, Color _color_, int depends_on_, int max_size_, int type_);
	bool render();
	bool update();

	bool getEnabled(){ return enabled; };
	std::string& getText(){ return texto; };
	std::string& getRealText(){ return texto_real; };
	Color getColor(){ return color_; };
	int getDependsOn(){ return depends_on; };
	int getPos_X(){ return position_x; };
	int getPos_Y(){ return position_y; };
	int getOrigX(){ return original_pos_X; };
	int getOrigY(){ return original_pos_Y; };
	bool getHasFocus(){ return has_focus; };
	int getMaxSize(){ return max_size; };
	int get_last_pixel_position(int array_number);
	int getType(){ return type; };

	void setPosX(int posX){ position_x = posX; };
	void setPosY(int posY){ position_y = posY; };
	void setType(int ty_){ type = ty_; };
	void setHasFocus(bool has_it){ has_focus = has_it; };
	void setText(std::string texto_){ texto = texto_; };
	void setRealText(std::string texto_){ texto_real = texto_; };
	void setEnabled(bool enab_){ enabled = enab_; };

private:

	int original_pos_X, original_pos_Y;
	int position_x, position_y, depends_on, text_size_, max_size;
	bool enabled, has_focus;
	int type; // tipo 0 es alfanumerico, tipo 1 es numerico
	Color color_;

	std::string texto;
	std::string texto_real;
	TextClass* m_Text;
};

#endif