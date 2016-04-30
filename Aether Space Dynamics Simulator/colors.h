////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COLORS_H_
#define _COLORS_H_

#include "stdafx.h"

class color_class
{
public:
	color_class();
	color_class(const color_class&);
	~color_class();

	//Basic colors
	Color White, Black, Gray;

	Color Red, Green, Blue, Yellow, Orange;

	Color Cyan, Magenta, Pink, Purple;

	Color Silver;

	// Fancy colors
	Color Steel, Light_steel, Royal_blue, Navy_blue, Sky_blue, Turquoise, Cadet_blue;
	Color Banana, Peacock;
	Color Sky_blue03, Sky_blue04, Dark_slate_gray02, Dark_slate_gray03, Dark_slate_gray04, Light_golden_rod03, Light_blue04, Azure04, Light_steel_blue04;

	Color Gray10, Gray20;

	// Custom
	Color Nostalgic_blue;

	Color dim_color(float dim_, Color& color_);
};

#endif