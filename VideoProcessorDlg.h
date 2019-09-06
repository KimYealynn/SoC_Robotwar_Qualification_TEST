
// VideoProcessorDlg.h : 헤더 파일
//

#pragma once

#include "imageSrc/VideoProcessing.h"

// CVideoProcessorDlg 대화 상자
class CVideoProcessorDlg : public CDialogEx
{
// 생성입니다.
public:
	CVideoProcessorDlg(CWnd* pParent = NULL);// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VIDEOPROCESSOR_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonStartCam();

	afx_msg void _ViewHistogram(CByteImage& image);

	afx_msg void find_color_without_difference_image(CByteImage gImageBuf, CByteImage gBinImage);

	afx_msg void GetHisto(CByteImage& image);
	afx_msg void Detectcolor(CByteImage& image);

	afx_msg void Find_close_set_part(const CByteImage& imageIn, CByteImage& cannyimageIn);
	afx_msg void MeanShift(CByteImage& imageIn, CByteImage& Colored_canny, CByteImage& Original_image);

	afx_msg void ExecuteLynnHSV(CByteImage gImageBuf, int StartH, int EndH, int StartS, int EndS, int StartV, int EndV);
	afx_msg void CPErode(CByteImage& gBinImage);
	afx_msg void CPDilate(CByteImage& gBinImage);

	afx_msg void DoHoughparam(CByteImage& gBinImage);

protected:
	// 카메라 캡쳐 관련 멤버 변수
	CWebCam		m_webCam;

	// AVI 동영상 읽기 관련 멤버 변수
	CAviPlayer	m_aviPlayer;
	int			m_nFrame;
	int			m_idxFrame;
	bool		m_bPlay;

	int            m_histogram[256];
	double         m_histogramCdf[256];

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickeddetectcolor();
};
