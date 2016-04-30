#ifndef _LINECLASS_H_
#define _LINECLASS_H_

#include "stdafx.h"

class LineClass
{

public:
	LineClass();
	LineClass(const LineClass&);
	~LineClass();

	bool create_points_buffer(Vector3* buffer_, int buffer_size);
	void set_buffers();

private:

	Vector3 position;
	ID3D11Buffer *vertexBuffer;
};

#endif