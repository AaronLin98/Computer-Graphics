#define _MY_GLOBAL_VARIABLE_DEFINITION_FILE_

#include "stdafx.h"
#include "GlobalVariables.h"

// --------------------- Global variables ---------------------

int G_iDrawState;
int G_iMouseState;
COLORREF G_cLineColor;

void MyPolygon::SetTheSeed(CPoint point1)
{
	m_Seed = point1;
}

void MyPolygon::SetLineColor(COLORREF color1)
{
	m_LineColor = color1;
}

void MyPolygon::SetFillColor(COLORREF color1)
{
	m_FillColor = color1;
}

COLORREF MyPolygon::GetLineColor()
{
	return m_LineColor;
}

COLORREF MyPolygon::GetFillColor()
{
	return m_FillColor;
}

CPoint MyPolygon::GetTheSeed()
{
	return m_Seed;
}
void MyPolygon::FillThePolygon()
{ // my algorithm
	if (m_VerticeNumber<3) return;
}

void MyPolygon::InsertVertex(CPoint point1)
{
	m_Vertex[m_VerticeNumber] = point1;
	m_VerticeNumber ++;
}

bool MyPolygon::InsidePolygon(CPoint point1)
{
	CPoint thePoint = point1;
	CPoint m_TempVertex[50];

	for (int i = 0 ; i <m_VerticeNumber ; i ++ )  // 坐标平移 
	{
		   m_TempVertex[i].x = m_Vertex[i].x-thePoint.x;
		   m_TempVertex[i].y = m_Vertex[i].y-thePoint.y ; 
	}
	m_TempVertex[m_VerticeNumber].x = m_Vertex[0].x-thePoint.x;
	m_TempVertex[m_VerticeNumber].y = m_Vertex[0].y-thePoint.y ; 

	// 计算象限
	int f, t1, t2, sum=0;
	t1 = m_TempVertex[0].x>=0 ? ( m_TempVertex[0].y>=0?0:3 ) 
					: ( m_TempVertex[0].y>=0?1:2 ) ;       
	for (int i = 1 ; i <= m_VerticeNumber ; i ++ )
	{ 
		// 被测点为多边形顶点
		if ( !m_TempVertex[i].x && !m_TempVertex[i].y ) return false ; 

		f = m_TempVertex[i].y * m_TempVertex[i-1].x 
						- m_TempVertex[i].x * m_TempVertex[i-1].y ;         
		if ( !f && m_TempVertex[i-1].x*m_TempVertex[i].x <= 0 
			&& m_TempVertex[i-1].y*m_TempVertex[i].y <= 0 ) return false ;  // 点在边上
		t2 = m_TempVertex[i].x>=0 ? ( m_TempVertex[i].y>=0?0:3 ) 
			: ( m_TempVertex[i].y>=0?1:2 ) ;						// 计算象限

		if ( t2 == ( t1 + 1 ) % 4 )			sum += 1 ;              // 情况1
		else if ( t2 == ( t1 + 3 ) % 4 )	sum -= 1 ;              // 情况2
		else if ( t2 == ( t1 + 2 ) % 4 )                           // 情况3
		{          
			 if ( f > 0 )	sum += 2 ; 
			 else			sum -= 2 ;
		}
		t1 = t2 ;
	}

	if (sum == -4) return  true;
	else		  return  false;
}