// CGPainterView.cpp : implementation of the CCGPainterView class
//

#include "stdafx.h"
#include "CGPainter.h"
#include "GlobalVariables.h"
#include "math.h"

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
	G_cLineColor = RGB(0,0,0);

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
	pen.CreatePen(PS_SOLID, 1, G_cLineColor);
	CPen* pOldPen=m_pMemDC->SelectObject(&pen);
	m_pMemDC->SelectObject(&pen);

	CBrush* pOldBrush=(CBrush*)m_pMemDC->SelectStockObject(NULL_BRUSH);
	m_pMemDC->SelectStockObject(NULL_BRUSH);

	//draw graph
	if(G_iDrawState == DRAW_LINE)
		DrawLine(m_pMemDC, m_ptStart, point, G_cLineColor);
	else if(G_iDrawState == DRAW_CIRCLE)
	{
		CPoint dist = point - m_ptStart;
		int radius = int(sqrt(float(dist.x*dist.x + dist.y*dist.y)) + 0.5);
		DrawCircle(m_pMemDC, m_ptStart, radius, G_cLineColor);
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
	
	if(G_iMouseState != MOUSE_LEFT_BUTTON)
		return;

	//set drawing context
	CDC* pDC=GetDC();
	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, G_cLineColor);
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
	if(G_iDrawState == DRAW_LINE)
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
	
/*************************************************************
编码直线生成算法，调用函数pDC->SetPixelV(point, cLineColor)画像素。
*************************************************************/
	//求起点到终点的横纵方向的距离
	long len_Y = ptEndPoint.y - ptStartPoint.y;
	long len_X = ptEndPoint.x - ptStartPoint.x;
	long len = max(len_X,len_Y);
	//设置基本变化的单位长度
	float fx_unit = 1.0f;
	float fy_unit = 1.0f;
	fy_unit = static_cast<float>(len_Y)/static_cast<float>(len);
	fx_unit = static_cast<float>(len_X)/static_cast<float>(len);
	//初始化
	pDC->SetPixelV(ptStartPoint,cLineColor);
	float x = static_cast<float>(ptStartPoint.x);
	float y = static_cast<float>(ptStartPoint.y);
	//累计画线
	for(long i = 1;i<=len;i++){
		//累积求和
		x = x + fx_unit;
		y = y + fy_unit;
		CPoint p(x,y);
		pDC->SetPixelV(p,cLineColor);
	}
}

int CCGPainterView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CDC* pDC=GetDC();
	m_pMemDC->CreateCompatibleDC(pDC);
	m_pBitmap->CreateCompatibleBitmap(pDC,m_nMaxX,m_nMaxY); 
	CBitmap* pOldBitmap=m_pMemDC->SelectObject(m_pBitmap);
	CBrush brush;
	brush.CreateStockObject(WHITE_BRUSH);
	CRect rect(-1,-1,m_nMaxX,m_nMaxY);
	m_pMemDC->FillRect(rect,&brush);
	
	m_pMemDC->SelectObject(pOldBitmap);
	ReleaseDC(pDC);
	
	return 0;
}

void CCGPainterView::DrawCircle(CDC *pDC, CPoint ptOrigin, int iRadius, COLORREF cLineColor)
{
/*************************************************************
write the circle algorithm for drawing the circle
use function: pDC->SetPixelV(point, cLineColor); to drawing a pixel
编码圆弧生成算法，调用函数pDC->SetPixelV(point, cLineColor)画像素。
*************************************************************/
	//画出上下左右四个顶点
	CPoint pt1(ptOrigin.x ,ptOrigin.y + iRadius);
	pDC->SetPixelV(pt1,cLineColor);
	CPoint pt2(ptOrigin.x + iRadius, ptOrigin.y);
	pDC->SetPixelV(pt2,cLineColor);
	CPoint pt3(ptOrigin.x, ptOrigin.y - iRadius);
	pDC->SetPixelV(pt3,cLineColor);
	CPoint pt4(ptOrigin.x - iRadius, ptOrigin.y);
	pDC->SetPixelV(pt4,cLineColor);
	long Diert,lamd1,lamd2;
	long Diert_,lamd1_,lamd2_;
	//以上面的顶点为起点做四分之一圆
	CPoint start(ptOrigin.x ,ptOrigin.y + iRadius);
	while (start.x <= (ptOrigin.x + iRadius) && start.y >= ptOrigin.y)
	{
		//求D点到圆弧的距离
		Diert = (start.x+1-ptOrigin.x)*(start.x+1-ptOrigin.x) +
				(start.y-1-ptOrigin.y)*(start.y-1-ptOrigin.y) - iRadius*iRadius;
		//Diert<0时，求H点到圆弧的差值
		lamd1 = Diert + 2*(start.y - ptOrigin.y) + 1;
		//Diert>0时，求V点到圆弧的差值
		lamd2 = Diert - 2*(start.x - ptOrigin.x) + 1;
		//求各自的绝对值
		Diert_ = fabs(double(Diert));
		lamd1_ = fabs(double(lamd1));
		lamd2_ = fabs(double(lamd2));
		if(Diert < 0){
			if(lamd1_ <= Diert_){
				//选择H点
				start.x = start.x+1;
				start.y = start.y;
			}
			else{
				//选择D点
				start.x = start.x+1;
				start.y = start.y-1;
			}
		}
		else if(Diert >0){
			if(lamd2_ <= Diert_){
				//选择V点
				start.x = start.x;
				start.y = start.y-1;
			}
			else{
				//选择D点
				start.x = start.x+1;
				start.y = start.y-1;
			}
		}
		else{
			//选择D点
			start.x = start.x+1;
			start.y = start.y-1;
		}
		//画对称点
		CPoint down(start.x, 2*ptOrigin.y-start.y);
		CPoint left(2*ptOrigin.x-start.x,start.y);
		CPoint sym(2*ptOrigin.x-start.x,2*ptOrigin.y-start.y);
		pDC->SetPixelV(start,cLineColor);
		pDC->SetPixelV(down,cLineColor);
		pDC->SetPixelV(left,cLineColor);
		pDC->SetPixelV(sym,cLineColor);
	}
}
