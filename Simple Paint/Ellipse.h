#pragma once
#include "Shape.h"
#include "Paint.h"
int PaintEllipse(HDC * _hdc, const CPoint &, const CPoint &, const int &, void *);

class CEllipse :
	public CShape
{
public:

	CEllipse();
	CEllipse(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr);
	virtual ~CEllipse();

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

