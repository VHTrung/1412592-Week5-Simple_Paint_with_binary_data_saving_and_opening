#include "stdafx.h"
#include "Paint.h"

HDC * CPaint::m_hdc = nullptr;
CPaint::CPaint()
{
}



CPaint::~CPaint()
{
}

HDC *  CPaint::SetHDC(HDC * _hdc)
{
	HDC * old = m_hdc;
	m_hdc = _hdc;
	return old;
}

int CPaint::PaintMe(CShape * _shape, int(*_pfn) (HDC *, const CPoint &, const CPoint &, const int &, void *))
{

	return _pfn(m_hdc, _shape->GetLeftTop(), _shape->GetRightBottom(), _shape->GetMoreDataElementNum(), _shape->GetMoreData());
}
