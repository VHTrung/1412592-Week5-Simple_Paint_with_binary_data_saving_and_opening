#include "stdafx.h"
#include "Ellipse.h"


CEllipse::CEllipse()
	: CShape()
{
}

CEllipse::CEllipse(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void* _moreData)
{
	m_leftTop = _leftTop;
	m_rightBottom = _rightBottom;
}


CEllipse::~CEllipse()
{
}

CPoint CEllipse::GetLeftTop() const
{
	return m_leftTop;
}

CPoint CEllipse::GetRightBottom() const
{
	return m_rightBottom;
}

int CEllipse::GetMoreDataElementNum() const
{
	return 0;
}

void * CEllipse::GetMoreData() const
{
	return nullptr;
}

CShape * CEllipse::Create() const
{
	return new CEllipse();
}

CShape * CEllipse::Create(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData) const
{
	return new CEllipse(_leftTop, _rightBottom);
}

CShape * CEllipse::Clone() const
{
	return new CEllipse(this->m_leftTop, this->m_rightBottom);
}

int CEllipse::Paint() const
{
	PaintFunction pfn = (PaintFunction)GetProcAddress(hinstLib, "PaintEllipse");
	if (!pfn)
		return -10;
	return CPaint::PaintMe((CShape*)this, pfn);
}

int CEllipse::GetType() const
{
	return ELLIPSESHAPECODE;
}

int CEllipse::SetData(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData)
{
	CShape::SetData(_leftTop, _rightBottom);
	if (_moreDataElemantNum != 0 || _moreData != nullptr)// parameters are ignored
		return 1;
	return 0;
}

//int PaintEllipse(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *)
//{
//	if (!_hdc)// invalid handle to divice context
//		return -1;
//
//	Ellipse(*_hdc, _leftTop.m_X, _leftTop.m_Y, _rightBottom.m_X, _rightBottom.m_Y);
//	return 0;
//}
