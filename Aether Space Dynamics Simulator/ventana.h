#ifndef _VENTANAS_H_
#define _VENTANAS_H_

#include "stdafx.h"
#include "text_box.h"

class ventana_class
{
public:

	ventana_class()
	{
		dimmed = 0.0f;
		visible = false;
		previous_position_x = 0;
		previous_position_y = 0;
		total_messages_ = 0;
		total_text_boxes = 0;

		tb_max_size = nullptr;
		tb_pos_x = nullptr;
		tb_pos_y = nullptr;
		tb_box_color = nullptr;
		tb_text_color = nullptr;
		tb_width = nullptr;
		tb_heigth = nullptr;
		tb_type = nullptr;

		message_sentence_number = nullptr;
		mensaje_ = nullptr;
		message_size = nullptr;
		message_dynamic = nullptr;
		pos_x_message = nullptr;
		pos_y_message = nullptr;
		default_pos_x_message = nullptr;
		default_pos_y_message = nullptr;
		color_message = nullptr;
	};

	//ventana_class(const ventana_class&){};
	~ventana_class()
	{
		SAFE_DELETE_ARRAY(tb_pos_x);
		SAFE_DELETE_ARRAY(tb_max_size);
		SAFE_DELETE_ARRAY(tb_pos_y);
		SAFE_DELETE_ARRAY(tb_box_color);
		SAFE_DELETE_ARRAY(tb_text_color);
		SAFE_DELETE_ARRAY(tb_type);
		SAFE_DELETE_ARRAY(tb_width);
		SAFE_DELETE_ARRAY(tb_heigth);

		SAFE_DELETE_ARRAY(mensaje_);
		SAFE_DELETE_ARRAY(message_sentence_number);
		SAFE_DELETE_ARRAY(message_size);
		SAFE_DELETE_ARRAY(message_dynamic);
		SAFE_DELETE_ARRAY(pos_x_message);
		SAFE_DELETE_ARRAY(pos_y_message);
		SAFE_DELETE_ARRAY(default_pos_x_message);
		SAFE_DELETE_ARRAY(default_pos_y_message);
		SAFE_DELETE_ARRAY(color_message);
	};

	bool initialize(std::string title_, int type_, int total_messages_b, int total_t_boxes, int pos_x_, int pos_y_, int width_, int height_, Color _color_, Color color_window_)
	{
		total_text_boxes = total_t_boxes;
		tipo_ = type_;
		total_messages_ = total_messages_b;
		titulo_ = title_;
		posicion_x = pos_x_;
		posicion_y = pos_y_;
		ventana_x_original = 0;
		ventana_y_original = 0;
		largo_ = width_;
		alto_ = height_;
		color_ = _color_;
		color_window = color_window_;
		visible = false;

		trapped = false;

		tb_pos_x = new int[total_text_boxes];
		tb_max_size = new int[total_text_boxes];
		tb_pos_y = new int[total_text_boxes];
		tb_box_color = new Color[total_text_boxes];
		tb_text_color = new Color[total_text_boxes];
		tb_type = new int[total_text_boxes];
		tb_width = new int[total_text_boxes];
		tb_heigth = new int[total_text_boxes];
		mensaje_ = new std::string[total_messages_];
		message_sentence_number = new int[total_messages_];
		message_size = new int[total_messages_];
		message_dynamic = new bool[total_messages_];
		pos_x_message = new int[total_messages_];
		pos_y_message = new int[total_messages_];
		default_pos_x_message = new int[total_messages_];
		default_pos_y_message = new int[total_messages_];
		color_message = new Color[total_messages_];

		return true;
	};

	std::string getTitle(){ return titulo_; };
	int getType(){ return tipo_; };
	int getPositionX(){ return posicion_x; };
	int getPositionY(){ return posicion_y; };
	int getVentanaOriginalPosX(){ return ventana_x_original; };
	int getVentanaOriginalPosY(){ return ventana_y_original; };
	int getPreviousPositionX(){ return previous_position_x; };
	int getPreviousPositionY(){ return previous_position_y; };
	int getWidth(){ return largo_; };
	int getHeight(){ return alto_; };
	int getTotalMessages(){ return total_messages_; };
	int getTotalTextBoxes(){ return total_text_boxes; };
	bool getTrapped(){ return trapped; };
	bool getVisible(){ return visible; };
	Color getColor(){ return color_; };
	Color getWindowColor(){ return color_window; };
	float getDimmed(){ return dimmed; };
	std::string getMessage(int message_number) { return mensaje_[message_number]; };
	int get_message_pos_x(int message_number) { return pos_x_message[message_number]; };
	int get_message_pos_y(int message_number) { return pos_y_message[message_number]; };
	int get_default_message_pos_x(int message_number) { return default_pos_x_message[message_number]; };
	int get_default_message_pos_y(int message_number) { return default_pos_y_message[message_number]; };
	int get_message_size(int message_number){ return message_size[message_number]; };
	bool get_message_dynamic(int message_number){ return message_dynamic[message_number]; };
	Color get_message_color(int message_number){ return color_message[message_number]; };
	int get_title_sentence_number(){ return title_sentence_number; };
	int get_message_sentence_number(int message_number){ return message_sentence_number[message_number]; };

	int get_tb_posx(int n){ return tb_pos_x[n]; };
	int get_tb_max_size(int n){ return tb_max_size[n]; };
	int get_tb_posy(int n){ return tb_pos_y[n]; };
	int get_tb_width(int n){ return tb_width[n]; };
	int get_tb_heigth(int n){ return tb_heigth[n]; };
	Color get_tb_box_color(int n){ return tb_box_color[n]; };
	Color get_tb_text_color(int n){ return tb_text_color[n]; };
	int get_tb_type(int n){ return tb_type[n]; };
	std::string getImage(){ return image_; };

	void set_message_(int message_number, std::string _message_){ mensaje_[message_number] = _message_; };
	void set_message_size(int message_number, int size_){ message_size[message_number] = size_; };
	void set_message_dynamic(int message_number, bool stat_){ message_dynamic[message_number] = stat_; };
	void set_position_x(int posx_){ posicion_x = posx_; };
	void set_position_y(int posy_){ posicion_y = posy_; };
	void set_tb_max_size(int n, int m_size_){ tb_max_size[n] = m_size_; };
	void set_ventana_original_posX(int pos_){ ventana_x_original = pos_; };
	void set_ventana_original_posY(int pos_){ ventana_y_original = pos_; };
	void set_previous_position_x(int posx_){ previous_position_x = posx_; };
	void set_previous_position_y(int posy_){ previous_position_y = posy_; };
	void set_message_pos_x(int message_number, int _pos_x_){ pos_x_message[message_number] = _pos_x_; };
	void set_message_pos_y(int message_number, int _pos_y_){ pos_y_message[message_number] = _pos_y_; };
	void set_default_message_pos_x(int message_number, int _pos_x_){ default_pos_x_message[message_number] = _pos_x_; };
	void set_default_message_pos_y(int message_number, int _pos_y_){ default_pos_y_message[message_number] = _pos_y_; };
	void set_message_color(int message_number, Color _color_){ color_message[message_number] = _color_; };
	void set_visible(bool visible_){ visible = visible_; };
	void setTrapped(bool trap_){ trapped = trap_; };
	void set_title_sentence_number(int sentence_number_){ title_sentence_number = sentence_number_; };
	void set_message_sentence_number(int message_n, int sentence_n){ message_sentence_number[message_n] = sentence_n; };
	void setDimmed(float dim){ dimmed = dim; };
	void setImage(std::string im_){ image_ = im_; };

	void set_tb_posx(int n, int posx){ tb_pos_x[n] = posx; };
	void set_tb_posy(int n, int posy){ tb_pos_y[n] = posy; };
	void set_tb_width(int n, int wid_){ tb_width[n] = wid_; };
	void set_tb_heigth(int n, int hei_){ tb_heigth[n] = hei_; };
	void set_tb_box_color(int n, Color col_){ tb_box_color[n] = col_; };
	void set_tb_text_color(int n, Color col_){ tb_text_color[n] = col_; };
	void set_tb_type(int n, int ty_){ tb_type[n] = ty_; };

private:

	std::string titulo_;
	int tipo_, posicion_x, posicion_y, ventana_x_original, ventana_y_original, previous_position_x, previous_position_y, largo_, alto_;
	int title_sentence_number;
	int total_messages_, total_text_boxes;
	bool trapped;
	Color color_;
	Color color_window;
	float dimmed;

	std::string* mensaje_;
	std::string image_;

	int* tb_pos_x;
	int* tb_pos_y;
	int* tb_width;
	int* tb_heigth;
	int* tb_max_size;
	int* message_size;
	int* message_sentence_number;
	bool* message_dynamic;
	int* pos_x_message;
	int* pos_y_message;
	int* default_pos_x_message;
	int* default_pos_y_message;
	Color* color_message;
	Color* tb_box_color;
	Color* tb_text_color;
	int* tb_type;

	bool visible;

};

#endif