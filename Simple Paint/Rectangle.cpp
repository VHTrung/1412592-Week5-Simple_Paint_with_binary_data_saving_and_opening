#include "stdafx.h"
#include "Rectangle.h"


CRectangle::CRectangle()
	: CShape()
{
}

CRectangle::CRectangle(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void* _moreData)
{
	m_leftTop = _leftTop;
	m_rightBottom = _rightBottom;
}


CRectangle::~CRectangle()
{
}

CPoint CRectangle::GetLeftTop() const
{
	return m_leftTop;
}

CPoint CRectangle::GetRightBottom() const
{
	return m_rightBottom;
}

int CRectangle::GetMoreDataElementNum() const
{
	return 0;
}

void * CRectangle::GetMoreData() const
{
	return nullptr;
}

CShape * CRectangle::Create() const
{
	return new CRectangle();
}

CShape * CRectangle::Create(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData) const
{
	return new CRectangle(_leftTop, _rightBottom);
}

CShape * CRectangle::Clone() const
{
	return new CRectangle(this->m_leftTop, this->m_rightBottom);
}

int CRectangle::Paint() const
{
	PaintFunction pfn = (PaintFunction)GetProcAddress(hinstLib, "PaintRectangle");
	if (!pfn)
		return -10;
	return CPaint::PaintMe((CShape*)this, pfn);
}

int CRectangle::GetType() const
{
	return RECTANGLESHAPECODE;
}

int CRectangle::SetData(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData)
{
	CShape::SetData(_leftTop, _rightBottom);
	if (_moreDataElemantNum != 0 || _moreData != nullptr)// parameters are ignored
		return 1;
	return 0;
}

//int PaintRectangle(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *)
//{
//	if (!_hdc)// invalid handle to divice context
//		return -1;
//
//	Rectangle(*_hdc, _leftTop.m_X, _leftTop.m_Y, _rightBottom.m_X, _rightBottom.m_Y);
//	return 0;
//}
