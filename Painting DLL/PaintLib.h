#pragma once
#ifdef PAINTLIB_EXPORTS
#define PAINTLIB_API __declspec(dllexport) 
#else
#define PAINTLIB_API __declspec(dllimport) 

namespace PaintLib
{
	struct CPoint
	{
		int m_X;
		int m_Y;
	};
	//class PaintLib
	//{
	//public:
	//	static int PaintLine(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *);
	//	static int  PaintEllipse(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *);
	//	static int  PaintRectangle(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *);

	//};
}

extern "C" PAINTLIB_API int PaintLine(HDC * _hdc, const PaintLib::CPoint & _leftTop, const PaintLib::CPoint & _rightBottom, const int &, void *);
extern "C" PAINTLIB_API int  PaintEllipse(HDC * _hdc, const PaintLib::CPoint & _leftTop, const PaintLib::CPoint & _rightBottom, const int &, void *);
extern "C" PAINTLIB_API int  PaintRectangle(HDC * _hdc, const PaintLib::CPoint & _leftTop, const PaintLib::CPoint & _rightBottom, const int &, void *);
#endif
