#include "stdafx.h"
#include "colors.h"

color_class::color_class()
{
	//Basic colors
	White = Color{ 1.0f, 1.0f, 1.0f, 1.0f };
	Black = Color{ 0.0f, 0.0f, 0.0f, 1.0f };
	Gray = Color{ 0.5f, 0.5f, 0.5f, 1.0f };

	Red = Color{ 1.0f, 0.0f, 0.0f, 1.0f };
	Green = Color{ 0.0f, 1.0f, 0.0f, 1.0f };
	Blue = Color{ 0.0f, 0.0f, 1.0f, 1.0f };
	Yellow = Color{ 1.0f, 1.0f, 0.0f, 1.0f };
	Orange = Color{ 1.0f, 0.5f, 0.0f, 1.0f };

	Cyan = Color{ 0.0f, 1.0f, 1.0f, 1.0f };
	Magenta = Color{ 1.0f, 0.0f, 1.0f, 1.0f };

	Pink = Color{ 1.0f, 0.75f, 0.8f, 1.0f };
	Purple = Color{ 0.5f, 0.0f, 0.5f, 1.0f };

	Silver = Color{ 0.75f, 0.75f, 0.75f, 1.0f };

	// Fancy colors
	Steel = Color{ 0.7f, 0.77f, 0.87f, 1.0f };
	Light_steel = Color{ 0.8f, 0.88f, 1.0f, 1.0f };
	Royal_blue = Color{ 0.25f, 0.41f, 0.88f, 1.0f };
	Navy_blue = Color{ 0.0f, 0.0f, 0.5f, 1.0f };
	Sky_blue = Color{ 0.53f, 0.81f, 0.92f, 1.0f };
	Turquoise = Color{ 0.0f, 0.9f, 0.93f, 1.0f };
	Cadet_blue = Color{ 0.37f, 0.62f, 0.63f, 1.0f };

	Banana = Color{ 0.89f, 0.81f, 0.34f, 1.0f };
	Peacock = Color{ 0.2f, 0.63f, 0.8f, 1.0f };

	Sky_blue03 = Color{ 0.42f, 0.65f, 0.8f, 1.0f };
	Sky_blue04 = Color{ 0.3f, 0.65f, 0.8f, 1.0f };
	Dark_slate_gray02 = Color{ 0.55f, 0.93f, 0.93f, 1.0f };
	Dark_slate_gray03 = Color{ 0.47f, 0.8f, 0.8f, 1.0f };
	Dark_slate_gray04 = Color{ 0.32f, 0.54f, 0.54f, 1.0f };
	Light_golden_rod03 = Color{ 0.8f, 0.74f, 0.44f, 1.0f };
	Light_blue04 = Color{ 0.41f, 0.51f, 0.54f, 1.0f };
	Azure04 = Color{ 0.51f, 0.54f, 0.54f, 1.0f };
	Light_steel_blue04 = Color{ 0.43f, 0.48f, 0.54f, 1.0f };

	Gray20 = Color{ 0.2f, 0.2f, 0.2f, 1.0f };
	Gray20 = Color{ 0.1f, 0.1f, 0.1f, 1.0f };

	Nostalgic_blue = Color{ 0.2f, 0.60f, 0.64f, 1.0f };
}

color_class::color_class(const color_class& other)
{
}


color_class::~color_class()
{
}

Color color_class::dim_color(float dim_, Color& color_)
{
	color_.A(dim_);
	return color_;
}