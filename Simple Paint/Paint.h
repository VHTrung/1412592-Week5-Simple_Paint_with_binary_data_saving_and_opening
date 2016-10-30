#pragma once
//#include <windef.h>
#include "Point.h"
#include "Shape.h"

class CPaint
{
//public:
//	typedef int(*pfncPaint_Type)(const HDC &_hdc, const CPoint &, const CPoint &, const int &, void *);
//
private:
	static HDC * m_hdc;
	CPaint();
	
public:
	~CPaint();

	static HDC * SetHDC(HDC *_hdc);
	static int PaintMe(CShape * _pShape, int (*_pfn) (HDC *, const CPoint &, const CPoint &, const int &, void *));
};
