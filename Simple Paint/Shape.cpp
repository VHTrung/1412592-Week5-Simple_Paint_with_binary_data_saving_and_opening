#include "stdafx.h"
#include "Shape.h"


CShape::CShape()
{
	hinstLib = LoadLibrary(L"Painting DLL.dll");

}

CShape::CShape(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData)
{
}

CShape::~CShape()
{
}

void CShape::Move(int _distanceX, int _distanceY)
{
	m_leftTop.m_X += _distanceX;
	m_rightBottom.m_X += _distanceX;

	m_leftTop.m_Y += _distanceY;
	m_rightBottom.m_Y += _distanceY;
}

int CShape::SetData(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData)
{
	m_leftTop = _leftTop;
	m_rightBottom = _rightBottom;
	if (_moreDataElemantNum != 0 || _moreData != nullptr)// parameters are ignored
		return 1;
	return 0;
}
