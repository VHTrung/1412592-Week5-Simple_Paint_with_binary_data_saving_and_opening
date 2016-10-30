#include "stdafx.h"
#include "Line.h"


CLine::CLine()
	:CShape()
{
}

CLine::CLine(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void* _moreData)
{
	m_leftTop = _leftTop;
	m_rightBottom = _rightBottom;
}


CLine::~CLine()
{
}

CPoint CLine::GetLeftTop() const
{
	return m_leftTop;
}

CPoint CLine::GetRightBottom() const
{
	return m_rightBottom;
}

int CLine::GetMoreDataElementNum() const
{
	return 0;
}

void * CLine::GetMoreData() const
{
	return nullptr;
}

CShape * CLine::Create() const
{
	return new CLine();
}

CShape * CLine::Create(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData) const
{
	return new CLine(_leftTop, _rightBottom);
}

CShape * CLine::Clone() const
{
	return new CLine(this->m_leftTop, this->m_rightBottom);
}

int CLine::Paint() const
{
	PaintFunction pfn = (PaintFunction)GetProcAddress(hinstLib, "PaintLine");
	if (!pfn)
		return -10;
	return CPaint::PaintMe((CShape*)this, pfn);
}

int CLine::GetType() const
{
	return LINESHAPECODE;
}

int CLine::SetData(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum, void * _moreData)
{
	CShape::SetData(_leftTop, _rightBottom);
	if (_moreDataElemantNum != 0 || _moreData != nullptr)// parameters are ignored
		return 1;

	return 0;
}

//int PaintLine(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *)
//{
//	if (!_hdc)// invalid handle to divice context
//		return -1;
//
//	MoveToEx(*_hdc, _leftTop.m_X, _leftTop.m_Y, NULL);
//	LineTo(*_hdc, _rightBottom.m_X, _rightBottom.m_Y);
//	return 0;
//}
