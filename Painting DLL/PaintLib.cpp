#include "stdafx.h"
#include "PaintLib.h"

int PaintLine(HDC * _hdc, const PaintLib::CPoint & _leftTop, const PaintLib::CPoint & _rightBottom, const int &, void *)
{
	if (!_hdc)// invalid handle to divice context
		return -1;

	MoveToEx(*_hdc, _leftTop.m_X, _leftTop.m_Y, NULL);
	LineTo(*_hdc, _rightBottom.m_X, _rightBottom.m_Y);
	return 0;
}

int PaintEllipse(HDC * _hdc, const PaintLib::CPoint & _leftTop, const PaintLib::CPoint & _rightBottom, const int &, void *)
{
	if (!_hdc)// invalid handle to divice context
		return -1;

	Ellipse(*_hdc, _leftTop.m_X, _leftTop.m_Y, _rightBottom.m_X, _rightBottom.m_Y);
	return 0;
}

int PaintRectangle(HDC * _hdc, const PaintLib::CPoint & _leftTop, const PaintLib::CPoint & _rightBottom, const int &, void *)
{
	if (!_hdc)// invalid handle to divice context
		return -1;

	Rectangle(*_hdc, _leftTop.m_X, _leftTop.m_Y, _rightBottom.m_X, _rightBottom.m_Y);
	return 0;
}

//int PaintLib::PaintLib::PaintLine(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *)
//{
//	if (!_hdc)// invalid handle to divice context
//		return -1;
//
//	MoveToEx(*_hdc, _leftTop.m_X, _leftTop.m_Y, NULL);
//	LineTo(*_hdc, _rightBottom.m_X, _rightBottom.m_Y);
//	return 0;
//}
//
//int PaintLib::PaintLib::PaintEllipse(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *)
//{
//	if (!_hdc)// invalid handle to divice context
//		return -1;
//
//	Ellipse(*_hdc, _leftTop.m_X, _leftTop.m_Y, _rightBottom.m_X, _rightBottom.m_Y);
//	return 0;
//}
//
//int PaintLib::PaintLib::PaintRectangle(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *)
//{
//	if (!_hdc)// invalid handle to divice context
//		return -1;
//
//	Rectangle(*_hdc, _leftTop.m_X, _leftTop.m_Y, _rightBottom.m_X, _rightBottom.m_Y);
//	return 0;
//}
