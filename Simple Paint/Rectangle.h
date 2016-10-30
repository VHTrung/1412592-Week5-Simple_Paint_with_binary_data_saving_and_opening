#pragma once
#include "Shape.h"
#include "Paint.h"
int PaintRectangle(HDC * _hdc, const CPoint &, const CPoint &, const int &, void *);

class CRectangle :
	public CShape
{
public:
	CRectangle();
	CRectangle(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr);
	virtual ~CRectangle();

	virtual CPoint GetLeftTop() const;
	virtual CPoint GetRightBottom() const;
	virtual int GetMoreDataElementNum() const;
	virtual void* GetMoreData() const;
	virtual CShape* Create() const;
	virtual CShape* Create(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr) const;
	virtual CShape* Clone() const;
	virtual int Paint() const;
	virtual int GetType() const;

	virtual int SetData(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr);
};

