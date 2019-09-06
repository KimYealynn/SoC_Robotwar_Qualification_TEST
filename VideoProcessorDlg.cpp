// Main file으로 
// 하단 함수 직접 구현하였습니다.
// _ViewHistogram(CByteImage& image);
// find_color_without_difference_image(CByteImage gImageBuf, CByteImage gBinImage);
// GetHisto(CByteImage& image);
// Detectcolor(CByteImage& image);
// Find_close_set_part(const CByteImage& imageIn, CByteImage& cannyimageIn);
// MeanShift(CByteImage& imageIn, CByteImage& Colored_canny, CByteImage& Original_image);
// ExecuteLynnHSV(CByteImage gImageBuf, int StartH, int EndH, int StartS, int EndS, int StartV, int EndV);
// CPErode(CByteImage& gBinImage);
// CPDilate(CByteImage& gBinImage);
// DoHoughparam(CByteImage& gBinImage);
//
// VideoProcessorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "VideoProcessor.h"
#include "VideoProcessorDlg.h"
#include "afxdialogex.h"

#include "imageSrc\ImageFrameWndManager.h"
#include "imageSrc\LoadImageFromFileDialog.h"
#include "imageSrc\Segmentation.h"

#include "imageSrc\Features.h"
#include "imageSrc\MyImageFunc.h"

#include "imageSrc\MorphologyFilter.h"


#include <cstdlib>

// 각 색깔의 H값 범위
#define H_R1_st 240
#define H_R1_ed 255
#define H_R2_st 0
#define H_R2_ed 10
#define S_R_st 0
#define S_R_ed 255
#define V_R_st 0
#define V_R_ed 255

#define H_Y_st 30
#define H_Y_ed 45
#define S_Y_st 0
#define S_Y_ed 255
#define V_Y_st 0
#define V_Y_ed 255


#define H_G_st 46
#define H_G_ed 75
#define S_G_st 100
#define S_G_ed 255
#define V_G_st 100
#define V_G_ed 200


#define H_B_st 140
#define H_B_ed 152
#define S_B_st 100
#define S_B_ed 255
#define V_B_st 0
#define V_B_ed 200
// 각 색깔에 대한 모드
#define RED 0
#define YELLOW 1
#define GREEN 2
#define BLUE 3
#define BACKGROUND 4
// 이전 영상 대비 최소 픽셀 변화량
#define MIN_GAP 4000

int MODE = BACKGROUND;
int temp_MODE = RED;
//과거(p), 현재(c)의 RYGB의 H값 누적
int P_RYGB_cnt[4];
int C_RYGB_cnt[4];

int GAP[4];
int C_ALL_RYGB;
int P_ALL_RYGB;
int KEYCNT;

int COLOR_num[5][3] = { { 255,0,0 },{ 255,230,0 } ,{ 0,255,0 },{ 0,0,255 },{ 255,255,255 } }; // RYGBB

bool FIRST = true;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 물체 검출 관련 전역 변수
CByteImage	gBinImage;

int prev_color_count[4]; //0:red, 1:yellow, 2:green, 3:blue
int prev_max_color = 0;


// 카메라 영상 및 정보를 저장하기 위한 전역 변수
CByteImage gImageBuf;
BITMAPINFO gBmpInfo;

// 버튼을 눌렀을 때 영상 활성화시킴
bool HSVImageOn;
bool HistrogramOn;
bool detectcolorOn;

bool is_Find_First_close_set_part = false;

static int StartH, EndH, StartS, EndS, StartV, EndV;
static int what_H;


//meanShift
int centerX = 0, centerY = 0;
int curr_max_color = 0;
int Lower_Focus_Range = -90, Upper_Focus_Range = 90;


//
int find_rectangle = 0;


void CVideoProcessorDlg::find_color_without_difference_image(CByteImage gImageBuf, CByteImage gBinImage)
{
	CDoubleImage	m_imageHSV;

	if (!gImageBuf.IsEmpty())
	{
		m_imageHSV = RGB2HSV(gImageBuf);
	}

	CByteImage* Four_binary = new CByteImage[4];//0:R,1:Y,2:G,3:B
	CByteImage* Four_canny = new CByteImage[4];//0:R,1:Y,2:G,3:B
	CByteImage* Four_gray2rgbimage = new CByteImage[4];//0:R,1:Y,2:G,3:B
	
	for (int i = 0; i < 4; i++)
	{
		Four_binary[i] = CByteImage(gImageBuf.GetWidth(), gImageBuf.GetHeight());
	}

	Binarization(m_imageHSV, Four_binary[0], H_R1_st, H_R1_ed, S_R_st, S_R_ed, V_R_st, V_R_ed);
	Binarization_For_Red(m_imageHSV, Four_binary[0], H_R2_st, H_R2_ed, S_R_st, S_R_ed, V_R_st, V_R_ed);
	Binarization(m_imageHSV, Four_binary[1], H_Y_st, H_Y_ed, S_Y_st, S_Y_ed, V_Y_st, V_Y_ed);
	Binarization(m_imageHSV, Four_binary[2], H_G_st, H_G_ed, S_G_st, S_G_ed, V_G_st, V_G_ed);
	Binarization(m_imageHSV, Four_binary[3], H_B_st, H_B_ed, S_B_st, S_B_ed, V_B_st, V_B_ed);


	for (int i = 0; i < 4; i++)
	{
		CPErode(Four_binary[i]);
		CPErode(Four_binary[i]);
		CPDilate(Four_binary[i]);
		CPDilate(Four_binary[i]);

		boldCannyEdge(Four_binary[i], Four_canny[i], 60, 30);

		switch (i) {
			case 0:
				Four_gray2rgbimage[i] = new_Gray2RGB(Four_canny[i], 237, 28, 36);//Red
				break;
			case 1:
				Four_gray2rgbimage[i] = new_Gray2RGB(Four_canny[i], 255, 255, 0);//Yellow
				break;
			case 2:
				Four_gray2rgbimage[i] = new_Gray2RGB(Four_canny[i], 0, 255, 0); //Green
				break;
			case 3:
				Four_gray2rgbimage[i] = new_Gray2RGB(Four_canny[i], 0, 255, 255);//Blue
				break;
		}
	}

	
	ShowImage(Four_canny[0], "red");
	ShowImage(Four_canny[1], "yellow");
	ShowImage(Four_canny[2], "green");
	ShowImage(Four_canny[3], "blue");

	ShowImage(Four_gray2rgbimage[0], "red_ca");
	ShowImage(Four_gray2rgbimage[1], "yellow_ca");
	ShowImage(Four_gray2rgbimage[2], "green_ca");
	ShowImage(Four_gray2rgbimage[3], "blue_ca");
	

}

void CVideoProcessorDlg::DoHoughparam(CByteImage& gBinImage)
{
	int nTVal = 254;//nTVal = 얼마 이상의 값을 가지는 픽셀을 경계선 픽셀로 인정할 것인가?(0~255 = 검은색부터 흰색까지 / 캐니엣지는 0 아니면 255니까 별 의미 없음 / 소벨엣지에서만 의미 있음)
	int nTNum = 90;//nTNum = 경계선 픽셀 수(캐니엣지된 영상의 흰 점)에 대한 문턱값으로, 몇 개 이상의 점을 지나는 직선을 검출할 것인가?
	static double ResTheta = 3.0;//ResTheta = 각도의 구간을 얼마나 정밀하게 나누어 조사할 것인가? = 값이 작을수록 정밀하게 조사

	double arrRho[100];//원점으로부터 직선에 수직으로 내린 수선의 길이
	double arrTheta[100];//y축과 수선(원점으로부터 직선에 수직으로 내린) 사이의 각도값

	int nLine = HoughLines(gBinImage, nTNum, nTVal, ResTheta, 100, arrRho, arrTheta);//nLine검출한 직선의 수

	int arrVer[20];//사각형의 세로평행을 세트로 저장
	int arrHori[20];//사각형의 가로를 저장

	int xIndex = 0;

	// 직선 그리기
	for (int i = 0; i<nLine; i++)
	{
		if (arrTheta[i] == 90) // 수직선(상하로 쭉 뻗는 직선), 이것만 따로 처리해준 이유는 수직선은 y축과 평행이 되어서 y1과 y2만으로 절대 만들어질 수 없음
		{
			DrawLine(gImageBuf, arrRho[i], 0, arrRho[i], gImageBuf.GetHeight() - 1, 255, 0, 0);
		}
		else//수직선 외의 모든 직선, y1과 y2만으로 직선을 만듦
		//else if (arrTheta[i]> 175 && arrTheta[i]<180)//사각형을 검출하고 싶으면 80<(현재 arrtheta - 맨 처음에 구한 arrtheta값)<100인 현재 arrtheta를 그리도록 하는 방향으로 하면??
		{
			int x1 = 0;
			int y1 = (int)(arrRho[i] / cos(arrTheta[i] * M_PI / 180) + 0.5);//C++에서 sin 함수와 cos 함수는 라디안을 받아 비율을 리턴한다. sin = 높이 / 사선
			int x2 = gImageBuf.GetWidth() - 1;
			int y2 = (int)( (arrRho[i] - x2 * sin(arrTheta[i] * M_PI / 180)) / cos(arrTheta[i] * M_PI / 180) + 0.5);//cos = 밑변 / 사선
			DrawLine(gImageBuf, x1, y1, x2, y2, 255, 0, 0);//R:255 G: 0 B: 0


		}
		arrVer[xIndex]= -1;//평행하는 선(수평선)
		for (int j = 0; j < nLine; j++) {
			int gap = arrTheta[i] - arrTheta[j];
			if (gap < 0) { gap *= -1; }
			if (gap > -5 && gap < 5) {
				if (arrRho[i] - arrRho[j] > 120 || arrRho[j] - arrRho[i] > 120) {
					arrVer[xIndex] = j;
					j = nLine;//break?
				}
			}
		}
		
		arrHori[xIndex]= -1;//수직선
		if (arrVer[xIndex] != -1) {
			for (int j = 0; j < nLine; j++) {
				int gap = arrTheta[i] - arrTheta[j];

				if ((arrTheta[i] * arrTheta[j]) < 0) {
					if (gap < 0) { gap *= -1; }
					if (gap>265 && gap<270) {
						arrHori[xIndex] = j;
						j = nLine;
					}
				}
				else {
					if (gap < 0) { gap *= -1; }
					if (gap > 85 && gap < 95) {
						arrHori[xIndex] = j;
						j = nLine;
					}
				}
			}
		}
		xIndex++;
	}

	double length = 0;
	double Rho_1 = 0,Rho_2=0;
	double theta_1 = 0, theta_2 = 0;

	for (int i = 0; i < nLine; i++) {
		if (arrVer[i] != -1 && arrHori[i] != -1) {
			int x1 = 0;
			int y1 = (int)(arrRho[i] / cos(arrTheta[i] * M_PI / 180) + 0.5);//C++에서 sin 함수와 cos 함수는 라디안을 받아 비율을 리턴한다. sin = 높이 / 사선
			int x2 = gImageBuf.GetWidth() - 1;
			int y2 = (int)((arrRho[i] - x2 * sin(arrTheta[i] * M_PI / 180)) / cos(arrTheta[i] * M_PI / 180) + 0.5);//cos = 밑변 / 사선
			DrawLine(gImageBuf, x1, y1, x2, y2, 0, 255, 255);
			
			x1 = 0;
			y1 = (int)(arrRho[arrVer[i]] / cos(arrTheta[arrVer[i]] * M_PI / 180) + 0.5);//C++에서 sin 함수와 cos 함수는 라디안을 받아 비율을 리턴한다. sin = 높이 / 사선
			x2 = gImageBuf.GetWidth() - 1;
			y2 = (int)((arrRho[arrVer[i]] - x2 * sin(arrTheta[arrVer[i]] * M_PI / 180)) / cos(arrTheta[arrVer[i]] * M_PI / 180) + 0.5);//cos = 밑변 / 사선
			DrawLine(gImageBuf, x1, y1, x2, y2, 0, 255, 0);

		  x1 = 0;
		 y1 = (int)(arrRho[arrHori[i]] / cos(arrTheta[arrHori[i]] * M_PI / 180) + 0.5);//C++에서 sin 함수와 cos 함수는 라디안을 받아 비율을 리턴한다. sin = 높이 / 사선
		 x2 = gImageBuf.GetWidth() - 1;
		 y2 = (int)((arrRho[arrHori[i]] - x2 * sin(arrTheta[arrHori[i]] * M_PI / 180)) / cos(arrTheta[arrHori[i]] * M_PI / 180) + 0.5);//cos = 밑변 / 사선
			DrawLine(gImageBuf, x1, y1, x2, y2, 0, 255, 0);


			//중심선분 2개 만드는곳
			length = arrRho[i] - arrRho[arrVer[i]];
			if (arrRho[i] * arrRho[arrVer[i]] < 0) {
				if (length > 0) { length *= -1; }
			}
			else {
				length *= -1;
			}

			Rho_1 = arrRho[i] + length / 2;
			Rho_2 = arrRho[arrHori[i]] + length / 2;
			
			theta_1 = arrTheta[i];
			theta_2 = arrTheta[arrHori[i]];

			x1 = 0;
			y1 = (int)(Rho_1 / cos(theta_1 * M_PI / 180) + 0.5);//C++에서 sin 함수와 cos 함수는 라디안을 받아 비율을 리턴한다. sin = 높이 / 사선
			x2 = gImageBuf.GetWidth() - 1;
			y2 = (int)((Rho_1 - x2 * sin(theta_1 * M_PI / 180)) / cos(theta_1 * M_PI / 180) + 0.5);//cos = 밑변 / 사선
			DrawLine(gImageBuf, x1, y1, x2, y2, 0, 0, 255);

			x1 = 0;
			y1 = (int)(Rho_2 / cos(theta_2 * M_PI / 180) + 0.5);//C++에서 sin 함수와 cos 함수는 라디안을 받아 비율을 리턴한다. sin = 높이 / 사선
			x2 = gImageBuf.GetWidth() - 1;
			y2 = (int)((Rho_2 - x2 * sin(theta_2 * M_PI / 180)) / cos(theta_2 * M_PI / 180) + 0.5);//cos = 밑변 / 사선
			DrawLine(gImageBuf, x1, y1, x2, y2, 0, 0, 255);

			i = xIndex;
		}
	}
	
	double x_1 = 0, x_2 = 0, x;
	double p_1 = 0, p_2 = 0, p;
	double X,Y;

	//교점
	if (length!=0) {
		x_1 = sin(theta_1* M_PI / 180) / cos(theta_1* M_PI / 180);
		x_2 = sin(theta_2* M_PI / 180) / cos(theta_2* M_PI / 180);
		x = x_1 - x_2;
		p_1 = Rho_1 / cos(theta_1* M_PI / 180);
		p_2 = Rho_2 / cos(theta_2* M_PI / 180);
		p = p_1 - p_2;

		X = p / x;
		Y = (Rho_1 - X*sin(theta_1* M_PI / 180)) / cos(theta_1* M_PI / 180);
	
		MODE = temp_MODE;

		centerX = X ;
		centerY = Y ;

		length = 0;
	}
	

	ShowImage(gImageBuf, "hough");
}






/*주요함수*/
LRESULT ProcessCamFrame(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	CVideoProcessorDlg VideoProcessorDlg;
	CByteImage Past_histImage(256, 256 + 20);

	if (gBmpInfo.bmiHeader.biCompression == BI_RGB) // RGB 영상
	{
		memcpy(gImageBuf.GetPtr(), lpVHdr->lpData,
			gBmpInfo.bmiHeader.biHeight*gBmpInfo.bmiHeader.biWidth);
	}
	else if (gBmpInfo.bmiHeader.biCompression == MAKEFOURCC('Y', 'U', 'Y', '2')) // 16비트 영상
	{
		YUY2ToRGB24(gBmpInfo.bmiHeader.biWidth, gBmpInfo.bmiHeader.biHeight,
			lpVHdr->lpData, gImageBuf.GetPtr());
	}
	else
	{
		return FALSE;
	}


	if (HSVImageOn)
	{
		VideoProcessorDlg.ExecuteLynnHSV(gImageBuf, StartH, EndH, StartS, EndS, StartV, EndV);
	}

	if (HistrogramOn)
	{
		VideoProcessorDlg._ViewHistogram(gImageBuf);
	}

	if (detectcolorOn)
	{
		//VideoProcessorDlg.Detectcolor(gImageBuf);
		if (MODE == BACKGROUND) {
			temp_MODE = (temp_MODE + 1) % 4;
			if (temp_MODE == RED)
			{
				StartH = H_R1_st;
				EndH = H_R1_ed;
				StartS = S_R_st;
				EndS = S_R_ed;
				StartV = V_R_st;
				EndV = V_R_ed;
			}
			else if (temp_MODE == YELLOW)
			{
				StartH = H_Y_st;
				EndH = H_Y_ed;
				StartS = S_Y_st;
				EndS = S_Y_ed;
				StartV = V_Y_st;
				EndV = V_Y_ed;
			}
			else if (temp_MODE == GREEN)
			{
				StartH = H_G_st;
				EndH = H_G_ed;
				StartS = S_G_st;
				EndS = S_G_ed;
				StartV = V_G_st;
				EndV = V_G_ed;
			}
			else if (temp_MODE == BLUE)
			{
				StartH = H_B_st;
				EndH = H_B_ed;
				StartS = S_B_st;
				EndS = S_B_ed;
				StartV = V_B_st;
				EndV = V_B_ed;
			}

		}
	}

	return TRUE;
}//ProcessCamFrame


void CVideoProcessorDlg::ExecuteLynnHSV(CByteImage gImageBuf, int StartH, int EndH, int StartS, int EndS, int StartV, int EndV)
{
	CDoubleImage	m_imageHSVAdj;
	CDoubleImage	m_imageHSV;
	CByteImage	m_imageH;
	CByteImage	m_imageS;
	CByteImage	m_imageV;
	CByteImage	m_imageOUT;
	CByteImage	gray2rgbimage;

	if (!gImageBuf.IsEmpty())
	{
		m_imageHSVAdj = m_imageHSV = RGB2HSV(gImageBuf);
	}
	m_imageH = m_imageHSVAdj.GetChannelImg(2)*(255.0 / 360.0) + 0.5;
	m_imageS = m_imageHSVAdj.GetChannelImg(1)*(255.0) + 0.5;
	m_imageV = m_imageHSVAdj.GetChannelImg(0)*(255.0) + 0.5;

	Binarization(m_imageHSV, gBinImage, StartH, EndH, StartS, EndS, StartV, EndV);
	if (temp_MODE == RED)
		Binarization_For_Red(m_imageHSV, gBinImage, H_R2_st, H_R2_ed, StartS, EndS, StartV, EndV);
	//원래 값 150,200,100,255,0,255

	CPErode(gBinImage); 
	CPErode(gBinImage);
	CPDilate(gBinImage);
	CPDilate(gBinImage);

	CByteImage CannyEdgedImage;

	//경계선출력하는부분
	if (temp_MODE == RED||MODE==RED) {
		boldCannyEdge(gBinImage, CannyEdgedImage, 60, 30);
		gray2rgbimage = new_Gray2RGB(CannyEdgedImage, 237, 28, 36);//Red
	}
	else if (temp_MODE == YELLOW || MODE == YELLOW) {
		boldCannyEdge(gBinImage, CannyEdgedImage, 60, 30);
		gray2rgbimage = new_Gray2RGB(CannyEdgedImage, 255, 255, 0);//Yellow
	}
	else if (temp_MODE == GREEN || MODE == GREEN) {
		boldCannyEdge(gBinImage, CannyEdgedImage, 60, 30);
		gray2rgbimage = new_Gray2RGB(CannyEdgedImage, 0, 255, 0); //Green
	}
	else if (temp_MODE == BLUE || MODE == BLUE) {
		boldCannyEdge(gBinImage, CannyEdgedImage, 60, 30);
		gray2rgbimage = new_Gray2RGB(CannyEdgedImage, 0, 255, 255);//Blue
	}

	//경계선출력하는부분
	//if (!is_Find_First_close_set_part)
	//	Find_close_set_part(gBinImage, CannyEdgedImage);
	//else //is_fid_first_close가 true일때

	//boldCannyEdge(gBinImage, CannyEdgedImage, 60, 30);
//	gray2rgbimage = new_Gray2RGB(CannyEdgedImage, 237, 28, 36);//Red


	if (MODE == BACKGROUND) {
		DoHoughparam(CannyEdgedImage);
	}

	//DoHoughparam(CannyEdgedImage);
	MeanShift(gBinImage, gray2rgbimage, gImageBuf); //첫번째인자 : 이진화영상, 두번째인자 : 테두리색깔넣는것
	//is_Find_First_close_set_part = false;//잘넣었군!
} //ExecuteLynnHSV


void CVideoProcessorDlg::MeanShift(CByteImage& imageIn, CByteImage& Colored_canny, CByteImage& Original_image)
{
	int Local_centerX = centerX, Local_centerY = centerY;
	int nWidth = imageIn.GetWidth();//640
	int nHeight = imageIn.GetHeight();//480
	CByteImage TestImage = imageIn;
	int Move_Range = 45; //움직이는 길이
	int Area[11] = { 0,0,0,0,0,0,0,0,0,0,0 }; //중앙[0], 팔방[1,8], 크기크게[9], 크기작게[10]

	if (!(MODE == BACKGROUND)) { // 백그라운드 모드인 경우 추적 필요 arrVer
						//중앙, 크기 별 카운트  
		for (int x = Lower_Focus_Range - 5; x < Upper_Focus_Range + 5; x++) //중심점 중심으로 오른쪽왼쪽 주변을 얼마나 볼껀가
		{
			for (int y = Lower_Focus_Range - 5; y < Upper_Focus_Range + 5; y++) //중심점중심으로 상하를 얼마나관찰할것인가
			{
				if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight)) { //중심점이 화면이외로 벗어나지않게
					if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255) Area[9]++; // 크기크게
					if (x > Lower_Focus_Range && x<Upper_Focus_Range && y > Lower_Focus_Range && y < Upper_Focus_Range) {
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255) Area[0]++; //기존크기
					}
					if (x > Lower_Focus_Range + 5 && x<Upper_Focus_Range - 5 && y > Lower_Focus_Range + 5 && y < Upper_Focus_Range - 5) {
						Area[10]++; // 크기작게
					}
				}
			}
		}



		//To south(2)
		Local_centerY += Move_Range;
		for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
		{
			for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
			{
				if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
					if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
						Area[2]++;
			}
		}
		Local_centerY -= Move_Range;

		//To north east(6)
		Local_centerY -= Move_Range;
		Local_centerX += Move_Range;
		for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
		{
			for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
			{
				if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
					if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
						Area[6]++;
			}
		}
		Local_centerX -= Move_Range;
		Local_centerY += Move_Range;



		 

		/*크기별 밀도구하기*/
		int density[5]; int dens_index[3] = { 0,1,2 }; int tmp; int max = 0;
		density[0] = Area[0] / (Upper_Focus_Range*Upper_Focus_Range); // (색상 카운트 픽셀 수 / 크기 픽셀 수)
		density[1] = Area[9] / (Upper_Focus_Range + 10)*(Upper_Focus_Range + 10);
		density[2] = Area[10] / (Upper_Focus_Range - 10)*(Upper_Focus_Range - 10);
		density[3] = Area[2] / (Upper_Focus_Range*Upper_Focus_Range);
		density[4] = Area[6] / (Upper_Focus_Range*Upper_Focus_Range);

		Area[2] = 0;
		Area[6] = 0;

		if (density[0] < 0.2) { MODE = BACKGROUND; }
		else {

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 2; j++) {
					if (density[j] > density[j + 1]) {
						tmp = density[j];
						density[j] = density[j + 1];
						density[j + 1] = density[j];
						tmp = dens_index[j];
						dens_index[j] = dens_index[j + 1];
						dens_index[j + 1] = tmp;
					}
				}
			}

			if (dens_index[2] == 1) {
				Lower_Focus_Range -= 5;
				Upper_Focus_Range += 5;
			}
			else if (dens_index[2] == 2) {
				if (Upper_Focus_Range > 45) {
					Lower_Focus_Range += 5;
					Upper_Focus_Range -= 5;
				}
			}
			/*
			516
			304
			728
			*/

			//To north(1)
			Local_centerY -= Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[1]++;
				}
			}
			Local_centerY += Move_Range;

			//To south(2)
			Local_centerY += Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[2]++;
				}
			}
			Local_centerY -= Move_Range;

			//To west(3)
			Local_centerX -= Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[3]++;
				}
			}
			Local_centerX += Move_Range;

			//To east(4)
			Local_centerX += Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[4]++;
				}
			}
			Local_centerX -= Move_Range;

			//To north west(5)
			Local_centerY -= Move_Range;
			Local_centerX -= Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[5]++;
				}
			}
			Local_centerX += Move_Range;
			Local_centerY += Move_Range;

			//To north east(6)
			Local_centerY -= Move_Range;
			Local_centerX += Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[6]++;
				}
			}
			Local_centerX -= Move_Range;
			Local_centerY += Move_Range;

			//To south west(7)
			Local_centerY += Move_Range;
			Local_centerX -= Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[7]++;
				}
			}
			Local_centerX += Move_Range;
			Local_centerY -= Move_Range;

			//To south east(8)
			Local_centerY += Move_Range;
			Local_centerX += Move_Range;
			for (int x = Lower_Focus_Range; x < Upper_Focus_Range; x++)
			{
				for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
				{
					if ((Local_centerX + x > 0 && Local_centerX + x < nWidth) && (Local_centerY + y > 0 && Local_centerY + y < nHeight))
						if (TestImage.GetPtr(Local_centerY + y)[Local_centerX + x] == 255)
							Area[8]++;
				}
			}
			Local_centerX -= Move_Range;
			Local_centerY -= Move_Range;



			int Max_Area_count = 0;
			int Max_Area;

			for (int i = 0; i <= 8; i++)
			{
				if (Area[i] > Max_Area_count)
				{
					Max_Area_count = Area[i];
					Max_Area = i;
				}
			}

			switch (Max_Area)
			{
			case 0:
				break;
			case 1:
				centerY -= Move_Range;
				break;
			case 2:
				centerY += Move_Range;
				break;
			case 3:
				centerX -= Move_Range;
				break;
			case 4:
				centerX += Move_Range;
				break;
			case 5:
				centerY -= Move_Range;
				centerX -= Move_Range;
				break;
			case 6:
				centerY -= Move_Range;
				centerX += Move_Range;
				break;
			case 7:
				centerY += Move_Range;
				centerX -= Move_Range;
				break;
			case 8:
				centerY += Move_Range;
				centerX += Move_Range;
				break;
			}
		}
		//RGB는 채널이 3개니까 X에 관련된 것만*3
		int RGBcenterX = centerX * 3;
		int RGBWidth = nWidth * 3;
		int RGB_Lower_Focus_Range_X = Lower_Focus_Range * 3;
		int RGB_Upper_Focus_Range_X = Upper_Focus_Range * 3;

		for (int y = Lower_Focus_Range; y < Upper_Focus_Range; y++)
		{
			for (int x = RGB_Lower_Focus_Range_X; x < RGB_Upper_Focus_Range_X; x += 3)//RGB는 채널이 3개니까 +3
			{
				if ((RGBcenterX + x >= 0 && RGBcenterX + x < RGBWidth) && (centerY + y >= 0 && centerY + y < nHeight))
				{

					if ((y == Lower_Focus_Range || y == Upper_Focus_Range - 1)) {

						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x] = COLOR_num[MODE][2];//b
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 1] = COLOR_num[MODE][1];//g
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 2] = COLOR_num[MODE][0];//r
						Colored_canny.GetPtr(centerY + y + 1)[RGBcenterX + x] = COLOR_num[MODE][2];//b
						Colored_canny.GetPtr(centerY + y + 1)[RGBcenterX + x + 1] = COLOR_num[MODE][1];//g
						Colored_canny.GetPtr(centerY + y + 1)[RGBcenterX + x + 2] = COLOR_num[MODE][0];//r


						//원래 영상 위에 사각형
						Original_image.GetPtr(centerY + y)[RGBcenterX + x] = COLOR_num[MODE][2];//b
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 1] = COLOR_num[MODE][1];//g
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 2] = COLOR_num[MODE][0];//r
						Original_image.GetPtr(centerY + y + 1)[RGBcenterX + x] = COLOR_num[MODE][2];//b
						Original_image.GetPtr(centerY + y + 1)[RGBcenterX + x + 1] = COLOR_num[MODE][1];//g
						Original_image.GetPtr(centerY + y + 1)[RGBcenterX + x + 2] = COLOR_num[MODE][0];//r
						//원래 영상 위에 사각형
					}

					if (x == RGB_Lower_Focus_Range_X || x == RGB_Upper_Focus_Range_X - 3) {

						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x] = COLOR_num[MODE][2];//b
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 1] = COLOR_num[MODE][1];//g
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 2] = COLOR_num[MODE][0];//r
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 3] = COLOR_num[MODE][2];//b
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 1 + 3] = COLOR_num[MODE][1];//g
						Colored_canny.GetPtr(centerY + y)[RGBcenterX + x + 2 + 3] = COLOR_num[MODE][0];//r

						//원래 영상 위에 사각형
						Original_image.GetPtr(centerY + y)[RGBcenterX + x] = COLOR_num[MODE][2];//b
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 1] = COLOR_num[MODE][1];//g
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 2] = COLOR_num[MODE][0];//r
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 3] = COLOR_num[MODE][2];//b
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 1 + 3] = COLOR_num[MODE][1];//g
						Original_image.GetPtr(centerY + y)[RGBcenterX + x + 2 + 3] = COLOR_num[MODE][0];//r
						//원래 영상 위에 사각형
					}
				}
			}
		}
	}
	ShowImage(Colored_canny, "Result on canny");
	ShowImage(Original_image, "Result on original");
} //MeanShift


void CVideoProcessorDlg::Find_close_set_part(const CByteImage& imageIn, CByteImage& cannyimageIn)
{
	int nWidth = imageIn.GetWidth();//640
	int nHeight = imageIn.GetHeight();//480

	int* Xarray = new int[nWidth];
	memset(Xarray, 0, sizeof(int));

	int* Yarray = new int[nHeight];
	memset(Yarray, 0, sizeof(int));

	for (int r = 0; r < nHeight; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		for (int c = 0; c < nWidth; c++)
		{
			if (pIn[c] == 255)
			{
				Xarray[c]++;
				Yarray[r]++;
			}
		}
	}


	int left, right, up, down;

	for (int i = 0; i <nHeight; i++)
	{
		if (Yarray[i] > 80)
		{
			down = i;
			break;
		}
	}
	for (int i = nHeight - 1; i >0; i--)
	{
		if (Yarray[i] > 80)
		{
			up = i;
			break;
		}
	}
	for (int i = 0; i < nWidth; i++)
	{
		if (Xarray[i] > 80)
		{
			left = i;
			break;
		}
	}
	for (int i = nWidth - 1; i > 0; i--)
	{
		if (Xarray[i] > 80)
		{
			right = i;
			break;
		}
	}

	if (right - left > 0 && up - down > 0 && right - left < 300 && up - down < 300)
	{
		centerX = left + ((right - left) / 2);
		centerY = down + ((up - down) / 2);

	}

	if (centerX != 0 && centerY != 0)
		is_Find_First_close_set_part = true;

}//Find_close_set_part




void CVideoProcessorDlg::Detectcolor(CByteImage& image)
{
	CDoubleImage m_imageHSVAdj = RGB2HSV(image);
	CByteImage m_imageH = m_imageHSVAdj.GetChannelImg(2)*(255.0 / 360.0) + 0.5;//HSV로 바꾼것에서 H만따낸것.
	CByteImage m_imageS = m_imageHSVAdj.GetChannelImg(1)*(255.0) + 0.5;
	CByteImage m_imageV = m_imageHSVAdj.GetChannelImg(0)*(255.0) + 0.5;	

	int nWidth = m_imageH.GetWidth();
	int nHeight = m_imageH.GetHeight();

	memset(m_histogram, 0, 256 * sizeof(int)); //배열, 0으로 초기화 : 히스토그램그래프 값 넣어주려고
	memset(C_RYGB_cnt, 0, 4 * sizeof(int));

	int r, c;
	//히스토그램계산
	for (r = 0; r < nHeight; r++)
	{
		BYTE* pInH = m_imageH.GetPtr(r);
		BYTE* pInS = m_imageS.GetPtr(r);
		BYTE* pInV = m_imageV.GetPtr(r);

		for (c = 0; c < nWidth; c++) {

			if(
				(((pInH[c]>H_R1_st && pInH[c]<H_R1_ed) || (pInH[c]>H_R2_st && pInH[c]<H_R2_ed)) && pInS[c]>50 && pInS[c]<255 && pInV[c]>142 && pInV[c]<255)
				| (pInH[c]>H_Y_st && pInH[c]<H_Y_ed && pInS[c]>0 && pInS[c]<255 && pInV[c]>0 && pInV[c]<255)
				| (pInH[c]>H_G_st && pInH[c]<H_G_ed && pInS[c]>100 && pInS[c]<255 && pInV[c]>100 && pInV[c]<200)
				| (pInH[c]>H_B_st && pInH[c]<H_B_ed && pInS[c]>100 && pInS[c]<255 && pInV[c]>0 && pInV[c]<255)
				)
				m_histogram[pInH[c]]++;
		}


	//C_RYGB 계산
	for (int i = H_R1_st; i <= H_R1_ed; i++) {
		C_RYGB_cnt[RED] += m_histogram[i];
	}
	for (int i = H_R2_st; i <= H_R2_ed; i++) {
		C_RYGB_cnt[RED] += m_histogram[i];
	}
	for (int i = H_Y_st; i <= H_Y_ed; i++) {
		C_RYGB_cnt[YELLOW] += m_histogram[i];
	}
	for (int i = H_G_st; i <= H_G_ed; i++) {
		C_RYGB_cnt[GREEN] += m_histogram[i];
	}
	for (int i = H_B_st; i <= H_B_ed; i++) {
		C_RYGB_cnt[BLUE] += m_histogram[i];
	}
	

	if (!FIRST) {
		GAP[RED] = C_RYGB_cnt[RED] - P_RYGB_cnt[RED];
		GAP[YELLOW] = C_RYGB_cnt[YELLOW] - P_RYGB_cnt[YELLOW];
		GAP[GREEN] = C_RYGB_cnt[GREEN] - P_RYGB_cnt[GREEN];
		GAP[BLUE] = C_RYGB_cnt[BLUE] - P_RYGB_cnt[BLUE];

		//MODE 0 = 빨강, 1= 노랑, 2=초록, 3=파랑, 4=아무것도없음
		if (MODE == BACKGROUND) {
			int max = 0;
			int mode = 4;
			for (int i = 0; i < 4; i++) {
				if (GAP[i] > max) {
					max = GAP[i];
					mode = i;
				}
			}
			if (max>3000) { //3000
				MODE = mode;
				KEYCNT = P_RYGB_cnt[MODE];//전프레임의 선택된색깔의count
				is_Find_First_close_set_part = false;
			}
		}

		else if (KEYCNT + 700 > C_RYGB_cnt[MODE]) { //700
			// 색종이가 들어오기 전 프레임에서의 색종이 색상의 카운트 값보다 현재 색종이 색상의 카운트 값이 작아지면 색종이가 나간거라고 판단
			MODE = BACKGROUND;
		}

		else {
			if (MODE == RED)
			{
				StartH = H_R1_st;
				EndH = H_R1_ed;
				StartS = S_R_st;
				EndS = S_R_ed;
				StartV = V_R_st;
				EndV = V_R_ed;
				prev_max_color = 0;

			}
			else if (MODE == YELLOW)
			{
				StartH = H_Y_st;
				EndH = H_Y_ed;
				StartS = S_Y_st;
				EndS = S_Y_ed;
				StartV = V_Y_st;
				EndV = V_Y_ed;
				prev_max_color = 1;
			}
			else if (MODE == GREEN)
			{
				StartH = H_G_st;
				EndH = H_G_ed;
				StartS = S_G_st;
				EndS = S_G_ed;
				StartV = V_G_st;
				EndV = V_G_ed;
				prev_max_color = 2;
			}
			else if (MODE == BLUE)
			{
				StartH = H_B_st;
				EndH = H_B_ed;
				StartS = S_B_st;
				EndS = S_B_ed;
				StartV = V_B_st;
				EndV = V_B_ed;
				prev_max_color = 3;
			}
		}
	}
	FIRST = false;
	//P_RYGB 설정
	P_RYGB_cnt[RED] = C_RYGB_cnt[RED];
	P_RYGB_cnt[YELLOW] = C_RYGB_cnt[YELLOW];
	P_RYGB_cnt[GREEN] = C_RYGB_cnt[GREEN];
	P_RYGB_cnt[BLUE] = C_RYGB_cnt[BLUE];

	//히스토그램 최댓값탐색
	int nMaxHist = INT_MIN;
	int i;

	for (i = 0; i < 256; i++)
		if (m_histogram[i] > nMaxHist) {
			nMaxHist = m_histogram[i];
			what_H = i;
		}

	//히스토그램 최댓값으로 정규화하고 누적합계산
	double dDivMax = 256.0 / nMaxHist;
	double dNormFactor = 255.0 / (nWidth * nHeight);
	m_histogramCdf[0] = m_histogram[0] * dNormFactor;
	m_histogram[0] *= dDivMax;
	for (i = 1; i < 256; i++)
	{
		m_histogramCdf[i] = m_histogramCdf[i - 1] + m_histogram[i] * dNormFactor; //히스토그램에 나오는 누적값
		m_histogram[i] *= dDivMax; //히스토그램에나오는값
	}

}//Detectcolor

/*주요함수 끝 부분 끝끝끝----------------------------------*/

void CVideoProcessorDlg::_ViewHistogram(CByteImage& image)
{
	CDoubleImage m_imageHSVAdj = RGB2HSV(image);
	CByteImage m_imageH = m_imageHSVAdj.GetChannelImg(2)*(255.0 / 360.0) + 0.5;//HSV로 바꾼것에서 H만따낸것.

	int nWidth = m_imageH.GetWidth();
	int nHeight = m_imageH.GetHeight();

	memset(m_histogram, 0, 256 * sizeof(int)); //배열, 0으로 초기화 : 히스토그램그래프 값넣어주려고

	int r, c;
	//히스토그램계산
	for (r = 0; r<nHeight; r++)
	{
		BYTE* pIn = m_imageH.GetPtr(r);
		for (c = 0; c<nWidth; c++)
			m_histogram[pIn[c]]++;
	}

	//히스토그램 최댓값탐색
	int nMaxHist = INT_MIN;
	int i;
	for (i = 0; i<256; i++)
		if (m_histogram[i] > nMaxHist)
			nMaxHist = m_histogram[i];

	//히스토그램 최댓값으로 정규화하고 누적합계산
	double dDivMax = 256.0 / nMaxHist;
	double dNormFactor = 255.0 / (nWidth * nHeight);
	m_histogramCdf[0] = m_histogram[0] * dNormFactor;
	m_histogram[0] *= dDivMax;
	for (i = 1; i<256; i++)
	{
		m_histogramCdf[i] = m_histogramCdf[i - 1] + m_histogram[i] * dNormFactor; //히스토그램에 나오는 누적값
		m_histogram[i] *= dDivMax; //히스토그램에나오는값
	}

	CByteImage histImage(256, 256 + 20); // 히스토그램 영상
	histImage.SetConstValue(255);

	//히스토그램 영상 만들기
	for (i = 0; i<256; i++)
	{
		for (int h = 0; h<m_histogram[i]; h++)
			histImage.GetAt(i, 255 - h) = 128; // 히스토그램

		histImage.GetAt(i, 255 - (BYTE)(m_histogramCdf[i] + 0.5)) = 0; // 누적합

		for (int h = 6; h<20; h++)
			histImage.GetAt(i, 256 + h) = i; // 인덱스 색
	}
	ShowImage(histImage, "Histogram");
}


void CVideoProcessorDlg::GetHisto(CByteImage& image)
{
	CDoubleImage m_imageHSVAdj = RGB2HSV(image);
	CByteImage m_imageH = m_imageHSVAdj.GetChannelImg(2)*(255.0 / 360.0) + 0.5;//HSV로 바꾼것에서 H만따낸것.

	int nWidth = m_imageH.GetWidth();
	int nHeight = m_imageH.GetHeight();

	memset(m_histogram, 0, 256 * sizeof(int)); //배열, 0으로 초기화 : 히스토그램그래프 값넣어주려고

	int r, c;
	//히스토그램계산
	for (r = 0; r<nHeight; r++)
	{
		BYTE* pIn = m_imageH.GetPtr(r);
		for (c = 0; c<nWidth; c++)
			m_histogram[pIn[c]]++;
	}

	//히스토그램 최댓값탐색
	int nMaxHist = INT_MIN;
	int i;
	for (i = 0; i<256; i++)
		if (m_histogram[i] > nMaxHist)
			nMaxHist = m_histogram[i];

	//히스토그램 최댓값으로 정규화하고 누적합계산
	double dDivMax = 256.0 / nMaxHist;
	double dNormFactor = 255.0 / (nWidth * nHeight);
	m_histogramCdf[0] = m_histogram[0] * dNormFactor;
	m_histogram[0] *= dDivMax;
	for (i = 1; i<256; i++)
	{
		m_histogramCdf[i] = m_histogramCdf[i - 1] + m_histogram[i] * dNormFactor; //히스토그램에 나오는 누적값
		m_histogram[i] *= dDivMax; //히스토그램에나오는값
	}
}



void CVideoProcessorDlg::CPErode(CByteImage& gBinImage) {
	CByteImage m_imageTmp;
	CByteImage maskErode(5, 5);		// 침식 마스크
	maskErode.SetConstValue(255);

	m_imageTmp = gBinImage;		// 임시 입력 영상
	gBinImage.SetConstValue(0);	// 결과 영상 초기화
	Erode(m_imageTmp, maskErode, gBinImage);
}

void CVideoProcessorDlg::CPDilate(CByteImage& gBinImage) {
	CByteImage m_imageTmp;
	CByteImage maskDilate(10, 10);	// 팽창 마스크
	maskDilate.SetConstValue(255);

	m_imageTmp = gBinImage;		// 임시 입력 영상
	gBinImage.SetConstValue(0);	// 결과 영상 초기화
	Dilate(m_imageTmp, maskDilate, gBinImage);
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();
	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
														// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVideoProcessorDlg 대화 상자

CVideoProcessorDlg::CVideoProcessorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoProcessorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nFrame = 0;
	m_idxFrame = 0;
	m_bPlay = true;
}

void CVideoProcessorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVideoProcessorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_CAM, &CVideoProcessorDlg::OnBnClickedButtonStartCam)
	
	ON_BN_CLICKED(IDOK, &CVideoProcessorDlg::OnBnClickedOk)
	
	ON_BN_CLICKED(IDC_detect_color, &CVideoProcessorDlg::OnBnClickeddetectcolor)
	
END_MESSAGE_MAP()


// CVideoProcessorDlg 메시지 처리기

BOOL CVideoProcessorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

									// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CVideoProcessorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CVideoProcessorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		m_aviPlayer.ShowFrame(GetDlgItem(IDC_STATIC_PREVIEW), m_idxFrame);
		CDialogEx::OnPaint();
	}
}

HCURSOR CVideoProcessorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool IsChanged(CByteImage& binImage)
{
	int nSize = binImage.GetWidth()*binImage.GetHeight();
	int nSum = 0;

	BYTE* pData = binImage.GetPtr();
	for (int i = 0; i<nSize; i++)
	{
		if (pData[i])
			nSum++;
	}

	if (nSum > 0.5*nSize)
		return true;
}


/*주요버튼*/
void CVideoProcessorDlg::OnBnClickeddetectcolor()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	gBinImage = CByteImage(gImageBuf.GetWidth(), gImageBuf.GetHeight());
	HSVImageOn = true;
	detectcolorOn = true;
	HistrogramOn = true;
	memset(P_RYGB_cnt, 0, 4 * sizeof(int));
	MODE = BACKGROUND;
	FIRST = true;
}

void CVideoProcessorDlg::OnBnClickedButtonStartCam()
{
	GetDlgItem(IDC_STATIC_PREVIEW)->SetWindowPos(NULL, 0, 0, 640, 480, SWP_NOZORDER | SWP_NOMOVE);
	m_webCam.StartCam(GetDlgItem(IDC_STATIC_PREVIEW), 640, 480, gBmpInfo);
	gImageBuf = CByteImage(gBmpInfo.bmiHeader.biWidth, gBmpInfo.bmiHeader.biHeight, 3);
	m_webCam.SetCallBackOnFrame(ProcessCamFrame);
}

/*주요버튼 끝부분 끝끝끝*/

void CVideoProcessorDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CDialogEx::OnOK();
}