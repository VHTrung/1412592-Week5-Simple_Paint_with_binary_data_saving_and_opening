#pragma once
#include "Point.h"

#define LINESHAPECODE 1
#define ELLIPSESHAPECODE 2
#define RECTANGLESHAPECODE 3

typedef int(*PaintFunction)(HDC * _hdc, const CPoint & _leftTop, const CPoint & _rightBottom, const int &, void *);

class CShape
{

protected:
	HINSTANCE hinstLib;
	CPoint m_leftTop;
	CPoint m_rightBottom;

public:
	CShape();
	CShape(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr);
	virtual ~CShape();

	virtual CPoint GetLeftTop() const = 0;
	virtual CPoint GetRightBottom() const = 0;
	virtual int GetMoreDataElementNum() const = 0;
	virtual void* GetMoreData() const = 0;
	virtual CShape* Create() const = 0;
	virtual CShape* Create(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr) const = 0;
	virtual CShape* Clone() const = 0;
	virtual int Paint() const = 0;
	virtual int GetType() const = 0;
	virtual void Move(int _distanceX, int _distanceY);

	virtual int SetData(const CPoint & _leftTop, const CPoint & _rightBottom, const int & _moreDataElemantNum = 0, void* _moreData = nullptr);
};

