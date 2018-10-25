// CGPainterView.cpp : implementation of the CCGPainterView class
//

#include "stdafx.h"
#include "CGPainter.h"
#include "GlobalVariables.h"
#include "math.h"
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stack>
using namespace std;

#include "CGPainterDoc.h"
#include "CGPainterView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView

IMPLEMENT_DYNCREATE(CCGPainterView, CView)

BEGIN_MESSAGE_MAP(CCGPainterView, CView)
	//{{AFX_MSG_MAP(CCGPainterView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_CLEARWINDOW, OnButtonClearwindow)
	ON_COMMAND(ID_BUTTON_POLYGON, OnButtonPolygon)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_POLYGON, OnUpdateButtonPolygon)
	ON_COMMAND(ID_BUTTON_POLYGON_FILL, OnButtonPolygonFill)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_POLYGON_FILL, OnUpdateButtonPolygonFill)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView construction/destruction


CCGPainterView::CCGPainterView()
{
	m_pMemDC=new CDC;
	m_pBitmap=new CBitmap;
	m_nMaxX=GetSystemMetrics(SM_CXSCREEN);
	m_nMaxY=GetSystemMetrics(SM_CYSCREEN);

	G_iDrawState = DRAW_NOTHING;
	G_cLineColor = RGB(0,1,0);

	G_iMouseState = NO_BUTTON;
}

CCGPainterView::~CCGPainterView()
{
	delete m_pMemDC;
	delete m_pBitmap;
}

BOOL CCGPainterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView drawing

void CCGPainterView::OnDraw(CDC* pDC)
{
	CCGPainterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	/////////////////////////////////////////
	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	pDC->BitBlt(0,0,m_nMaxX,m_nMaxY,m_pMemDC,0,0,SRCCOPY);
	m_pMemDC->SelectObject(pOldBitmap);
}

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView printing

BOOL CCGPainterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCGPainterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCGPainterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView diagnostics

#ifdef _DEBUG
void CCGPainterView::AssertValid() const
{
	CView::AssertValid();
}

void CCGPainterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGPainterDoc* CCGPainterView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGPainterDoc)));
	return (CCGPainterDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCGPainterView message handlers

void CCGPainterView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	G_iMouseState = MOUSE_LEFT_BUTTON;
	m_ptStart=point;
	m_ptOld=point;
}

void CCGPainterView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	G_iMouseState = NO_BUTTON;

	if(G_iDrawState == DRAW_NOTHING)
		return;

	//set the drawing context
	CDC* pDC=GetDC();
 	CBitmap* pOldBitmap=m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, G_cLineColor);

	CPen* pOldPen=m_pMemDC->SelectObject(&pen);
	m_pMemDC->SelectObject(&pen);
	pDC->SelectObject(&pen);

	CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
	m_pMemDC->SelectStockObject(NULL_BRUSH);
	pDC->SelectStockObject(NULL_BRUSH);
 
	//draw graph
	if(G_iDrawState == DRAW_LINE)
		DrawLine(m_pMemDC, m_ptStart, point, G_cLineColor);
	else if(G_iDrawState == DRAW_CIRCLE)
	{
		CPoint dist = point - m_ptStart;
		int radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);
		DrawCircle(m_pMemDC, m_ptStart, radius, G_cLineColor);
	}
	else if(G_iDrawState == DRAW_POLYGON)
	{
		if(ThePolygon.m_VerticeNumber>0)
			DrawLine(m_pMemDC, ThePolygon.m_Vertex[ThePolygon.m_VerticeNumber-1], point, G_cLineColor);
		
 		ThePolygon.InsertVertex(point);
	}
	else if(G_iDrawState == DRAW_POLYGON_FILL)
	{
		Scan_line_Fill(pDC,ThePolygon.GetFillColor(), G_cLineColor);
		if(ThePolygon.InsidePolygon(point)) 
		{
			Scan_line_Fill(pDC,ThePolygon.GetFillColor(), G_cLineColor);
			//BoundaryFill_Scan(pDC,point,ThePolygon.GetFillColor(),G_cLineColor);
// 			COLORREF pixelColor = pDC->GetPixel(point);
			//BoundaryFill_4Connection(pDC, point, ThePolygon.GetFillColor(), G_cLineColor);
//			pixelColor = m_pMemDC->GetPixel(point);
		//	BoundaryFill_4Connection(m_pMemDC, point, ThePolygon.GetFillColor(), G_cLineColor);
		}
		else
		{
			MessageBox("The Seed has to be inside the polygon.", "WARNING", MB_OK);
		
		}
		//Scan_line_Fill(pDC,ThePolygon.GetFillColor(), G_cLineColor);
		G_iDrawState = DRAW_NOTHING;
	}

	//retrieve the old contex
 	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	m_pMemDC->SelectObject(pOldBitmap);
	m_pMemDC->SelectObject(pOldPen);
	m_pMemDC->SelectObject(pOldBrush);
 	ReleaseDC(pDC);

}

void CCGPainterView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(G_iDrawState == DRAW_NOTHING)
		return;
	
	if((G_iMouseState != MOUSE_LEFT_BUTTON) 
		&& (G_iDrawState!=DRAW_POLYGON || ThePolygon.m_VerticeNumber==0))
		return;

	//set drawing context
	CDC* pDC=GetDC();
 	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, G_cLineColor);

	CPen* pOldPen=m_pMemDC->SelectObject(&pen);
	pDC->SelectObject(&pen);
	
	CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
	pDC->SelectStockObject(NULL_BRUSH);
 
	//wipe the screen. This is used to realize the "elastic band" effect.
	CRect wipeRect(m_ptStart, m_ptOld);
	int radius;
	if(G_iDrawState == DRAW_CIRCLE)		//if drawing circle, the wipeRect is larger
	{
		CPoint dist = m_ptOld - m_ptStart;
		radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);

		CPoint ptLeftTop(m_ptStart.x - radius, m_ptStart.y - radius);
		CPoint ptRightBottom(m_ptStart.x + radius, m_ptStart.y + radius);
		wipeRect = CRect(ptLeftTop, ptRightBottom);
	}
	wipeRect.NormalizeRect();
	wipeRect.InflateRect(2, 2);
	pDC->BitBlt(wipeRect.left, wipeRect.top,
				wipeRect.Width(), wipeRect.Height(),
				m_pMemDC,
				wipeRect.left,wipeRect.top,
				SRCCOPY);

	//draw graph
	if(G_iDrawState == DRAW_LINE || G_iDrawState == DRAW_POLYGON)
		DrawLine(pDC, m_ptStart, point, G_cLineColor);
	else if(G_iDrawState == DRAW_CIRCLE)
	{
		CPoint dist = point - m_ptStart;
		int radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);
		DrawCircle(pDC, m_ptStart, radius, G_cLineColor);
	}
	
	//retrieve the old contex
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	m_pMemDC->SelectObject(pOldBitmap);
	m_pMemDC->SelectObject(pOldPen);
	m_pMemDC->SelectObject(pOldBrush);
	ReleaseDC(pDC);
	m_ptOld=point;
}



void CCGPainterView::DrawLine(CDC *pDC, CPoint ptStartPoint, CPoint ptEndPoint, COLORREF cLineColor)
{
//	pDC->MoveTo(ptStartPoint);
//	pDC->LineTo(ptEndPoint);

// DDA 算法画直线 
	//use function:
	//pDC->SetPixelV(point, cLineColor); to draw a pixel

	//求出步数
	CPoint CurrentPoint;

	float deltax = fabs(float(ptEndPoint.x - ptStartPoint.x));
	float deltay = fabs(float(ptEndPoint.y - ptStartPoint.y));


	// caculate the length(step number)
	int Length=0;

	if(deltax>=deltay) Length = deltax;
	else			   Length = deltay;

    if(Length == 0) 		return;

	int myXsign =  -1;
	int myYsign = -1;
	if(ptEndPoint.x-ptStartPoint.x >= 0)
		myXsign = 1;
 
	if(ptEndPoint.y-ptStartPoint.y >= 0)
		myYsign = 1;

	//求步长
	float stepX= deltax /  Length;
	float stepY= deltay /  Length;

	// 初始点坐标
	float x, y;
	x = 0.5  +  ptStartPoint.x  ;
    y = 0.5  +  ptStartPoint.y  ;

	//begin main loop
	
	for(int i=1;i<=Length;i++)
	{
	//坐标值取整
		CurrentPoint.x = (int)x;
		CurrentPoint.y = (int)y;
	//画点		
		pDC->SetPixelV(CurrentPoint, cLineColor);
	//迭加更新点坐标值		
		x =   x + stepX * myXsign;
		y =   y + stepY * myYsign;	
	}

	//write the Bresenham' line algorithm for drawing the line
	//use function:
	//pDC->SetPixelV(point, cLineColor); to drawing a pixel
/*	
//my realization

	int deltax = abs(ptEndPoint.x - ptStartPoint.x);
	int deltay = abs(ptEndPoint.y - ptStartPoint.y);
	
	int s1, s2;
	if(ptEndPoint.x-ptStartPoint.x >= 0)
		s1 = 1;
	else
		s1 = -1;
	if(ptEndPoint.y-ptStartPoint.y >= 0)
		s2=1;
	else
		s2 = -1;
	
	int temp, interchange;
	if(deltay > deltax)
	{
		temp = deltax;
		deltax = deltay;
		deltay = temp;
		interchange = 1;
	}
	else
		interchange = 0;
	
	int x = ptStartPoint.x;
	int y = ptStartPoint.y;
	int f = 2*deltay - deltax;
	CPoint point;
	for(int i=1; i<=deltax; i++)
	{
		point.x = x;	point.y = y;
		pDC->SetPixelV(point, cLineColor);
		if (f >= 0)
		{
			if(interchange == 1)
				x = x + s1;
			else
				y = y + s2;
			f = f - 2*deltax;
		}

		if(interchange==1)
			y = y + s2;
		else
			x = x + s1;
		f = f + 2*deltay;
	}
*/
}

int CCGPainterView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC=GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	m_pBitmap->CreateCompatibleBitmap(pDC,m_nMaxX,m_nMaxY); 
	m_pMemDC->SelectObject(m_pBitmap);

	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	CRect rect(-1,-1,m_nMaxX,m_nMaxY);
	m_pMemDC->FillRect(rect,&brush);	

	ReleaseDC(pDC);
	
	return 0;
}

void CCGPainterView::DrawCircle(CDC *pDC, CPoint ptOrigin, int iRadius, COLORREF cLineColor)
{
/* 	CPoint ptLeftTop(ptOrigin.x - iRadius, ptOrigin.y - iRadius);
 	CPoint ptRightBottom(ptOrigin.x + iRadius, ptOrigin.y + iRadius);
 	CRect circleRect(ptLeftTop, ptRightBottom);
 	pDC->Ellipse(circleRect);
*/
	//write the Midpoint circle algorithm for drawing the circle
	//use function:
	//pDC->SetPixelV(point, cLineColor); to drawing a pixel

	//the iterate equation is:
	//D0 = 5/4 - R;
	//D(i+1) = D(i) + 2*xi + 3, if D(i) < 0;  (or D(i+1) = D(i) + 2*x(i+1) + 1)
	//D(i+1) = D(i) + 2*xi -2*yi + 5, if D(i) >= 0;  (or D(i+1) = D(i) + 2*x(i+1) -2*y(i+1) + 1)
//my realization
 
	int x = 0;
	int y = iRadius;
	float D = 5/4.0f - iRadius;
	CPoint point;
	while(x<y)
	{
		//once get one point, eight points will be filled
		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + y;
		point.y = ptOrigin.y + x;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - y;
		point.y = ptOrigin.y + x;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + y;
		point.y = ptOrigin.y - x;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - y;
		point.y = ptOrigin.y - x;
		pDC->SetPixelV(point, cLineColor);

		//compute the next step value (D and x, y)
		if(D<0)
			D = D + 2*x + 3;
		else
		{
			D = D + 2*x - 2*y + 5; 
			y = y-1;
		}
		x = x + 1;
    }

    if(x == y)
	{
		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y + y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x + x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);

		point.x = ptOrigin.x - x;
		point.y = ptOrigin.y - y;
		pDC->SetPixelV(point, cLineColor);
	}
 
}

void CCGPainterView::OnButtonClearwindow() 
{
	//清屏.
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	CRect rect(-1,-1,m_nMaxX,m_nMaxY);	//原图像并不清除.
	m_pMemDC->FillRect(rect,&brush);
	
	Invalidate(false);

	G_iDrawState = DRAW_NOTHING;	
	
}

void CCGPainterView::OnButtonPolygon() 
{
 	if(G_iDrawState != DRAW_POLYGON)
	{
		G_iDrawState = DRAW_POLYGON;
		ThePolygon.m_VerticeNumber = 0;

	}
	else
		G_iDrawState = DRAW_NOTHING;
	
}

void CCGPainterView::OnUpdateButtonPolygon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(G_iDrawState == DRAW_POLYGON);	
	
}

void CCGPainterView::OnButtonPolygonFill() 
{
 	if(G_iDrawState != DRAW_POLYGON_FILL)
	{
		G_iDrawState = DRAW_POLYGON_FILL;
		if (ThePolygon.m_VerticeNumber<3) 
		{
			MessageBox("You have to define a polygon firstly.", "WARNING", MB_OK);
			return;
		}   
	}
	else
		G_iDrawState = DRAW_NOTHING;
	
}

void CCGPainterView::OnUpdateButtonPolygonFill(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(G_iDrawState == DRAW_POLYGON_FILL);	
	
}

void CCGPainterView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int i=0;
	if(G_iDrawState == DRAW_POLYGON)
	{
		//set the drawing context
		CDC* pDC=GetDC();
		CBitmap* pOldBitmap=m_pMemDC->SelectObject(m_pBitmap);
		
		CPen pen;
		if(G_iDrawState == DRAW_POLYGON)
			pen.CreatePen(PS_SOLID, 3, ThePolygon.GetLineColor());
		else
			pen.CreatePen(PS_SOLID, 2, G_cLineColor);
		CPen* pOldPen=m_pMemDC->SelectObject(&pen);
		pDC->SelectObject(&pen);
		
		CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
		m_pMemDC->SelectStockObject(NULL_BRUSH);
		
		//Draw the last line
		if(ThePolygon.m_VerticeNumber> 2)
		{
			DrawLine(pDC, ThePolygon.m_Vertex[ThePolygon.m_VerticeNumber-1], ThePolygon.m_Vertex[0], G_cLineColor);
			DrawLine(m_pMemDC, ThePolygon.m_Vertex[ThePolygon.m_VerticeNumber-1], ThePolygon.m_Vertex[0], G_cLineColor);
		}
		G_iDrawState = DRAW_NOTHING;
		
		//retrieve the old contex
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		m_pMemDC->SelectObject(pOldBitmap);
		m_pMemDC->SelectObject(pOldPen);
		m_pMemDC->SelectObject(pOldBrush);
		ReleaseDC(pDC);
		Invalidate(false);

	}
	
	CView::OnRButtonUp(nFlags, point);
}

void CCGPainterView::BoundaryFill_4Connection(CDC *pDC, CPoint startPoint, COLORREF fillCol, COLORREF boundaryCol)
{
	//Write your boundary fill algorithm here.
	CPoint fillPoint = startPoint;
	COLORREF currentCol = pDC->GetPixel(fillPoint);
    if(currentCol != boundaryCol&&currentCol != fillCol)
	{
		pDC->SetPixelV(fillPoint, fillCol);
		fillPoint.y = startPoint.y+1;	//(x, y+1)
        BoundaryFill_4Connection(pDC, fillPoint, fillCol, boundaryCol);

		fillPoint.y = startPoint.y-1;	//(x, y-1)
        BoundaryFill_4Connection(pDC, fillPoint, fillCol, boundaryCol);

		fillPoint.x = startPoint.x-1;
		fillPoint.y = startPoint.y;		//(x-1, y)
        BoundaryFill_4Connection(pDC, fillPoint, fillCol, boundaryCol);
		
		fillPoint.x = startPoint.x+1;	//(x+1, y)
        BoundaryFill_4Connection(pDC, fillPoint, fillCol, boundaryCol);

	}
}

typedef struct XET{
	double xi;
	double dx;
	double dy;
	bool operator < (const XET& edge) const
	{   
		return  xi < edge.xi; 
	} 
}AET,NET,Edge;


void CCGPainterView::Scan_line_Fill(CDC *pDC, COLORREF fillCol, COLORREF boundaryCol){
	//寻找扫描线起点和终点
	long ymax=ThePolygon.m_Vertex[0].y;
	long ymin=ThePolygon.m_Vertex[0].y;
	for(int i=0;i<ThePolygon.m_VerticeNumber;i++){
		if(ThePolygon.m_Vertex[i].y>ymax)
			ymax = ThePolygon.m_Vertex[i].y;
		if(ThePolygon.m_Vertex[i].y<ymin)
			ymin = ThePolygon.m_Vertex[i].y;
	}
	int pointnum = ThePolygon.m_VerticeNumber;
	int len = ymax - ymin + 1;
	//建立边表的向量
	vector<Edge> net[100000];
	for(long i=ymax;i>=ymin;i--)
	{
		for(int j=0;j<pointnum;j++)
		{
			//找到和多边形端点相交的情况
			if(i == ThePolygon.m_Vertex[j].y)
			{
				if(ThePolygon.m_Vertex[(j-1+pointnum)%pointnum].y <= i)
				{
					Edge *e = new Edge;
					e->xi = ThePolygon.m_Vertex[j].x;
					e->dy = ThePolygon.m_Vertex[j].y - ThePolygon.m_Vertex[(j-1+pointnum)%pointnum].y;
					e->dx = 1.0*(ThePolygon.m_Vertex[(j-1+pointnum)%pointnum].x - ThePolygon.m_Vertex[j].x)/
						(ThePolygon.m_Vertex[(j-1+pointnum)%pointnum].y - ThePolygon.m_Vertex[j].y);
					net[ymax-i].push_back(*e);
				}
				if(ThePolygon.m_Vertex[(j+1+pointnum)%pointnum].y <= i)
				{
					Edge *e = new Edge;
					e->xi = ThePolygon.m_Vertex[j].x;
					e->dy = ThePolygon.m_Vertex[j].y - ThePolygon.m_Vertex[(j+1+pointnum)%pointnum].y;
					e->dx = 1.0*(ThePolygon.m_Vertex[(j+1+pointnum)%pointnum].x - ThePolygon.m_Vertex[j].x)/
						(ThePolygon.m_Vertex[(j+1+pointnum)%pointnum].y - ThePolygon.m_Vertex[j].y);
					net[ymax-i].push_back(*e);
				}
			}
		}
	}
	//建立活性边表
	vector<Edge> aet;
	for(int i=0;i<len;i++)
	{
		//向活性边表里面添加新边
		if(!net[i].empty())
		{
			for(int j=0;j<net[i].size();j++)
			{
				aet.push_back(net[i][j]);
			}
			sort(aet.begin(),aet.end());
		}
		//遍历活性边表来画线
		for(int j=0;j<aet.size() && j+1<aet.size();j++)
		{
			CPoint p1(aet[j].xi,ymax-i);
			CPoint p2(aet[j+1].xi,ymax-i);
			DrawLine(pDC,p1,p2,fillCol);
			j++;
		}
		//删除掉活性边表中dy<=1的边
		for(int j=0;j<aet.size();j++)
		{
			if(aet[j].dy <= 1)
			{
				std::vector<Edge>::iterator it = aet.begin()+j;
				aet.erase(it);
				j--;
			}
		}
		//更新活性边表中的值
		for(int j=0;j<aet.size();j++)
		{
			aet[j].dy -= 1;
			aet[j].xi -= aet[j].dx;
		}
		//重新排序
		sort(aet.begin(),aet.end());
	}
}
