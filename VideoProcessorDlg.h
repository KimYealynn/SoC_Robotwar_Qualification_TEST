
// VideoProcessorDlg.h : ��� ����
//

#pragma once

#include "imageSrc/VideoProcessing.h"

// CVideoProcessorDlg ��ȭ ����
class CVideoProcessorDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CVideoProcessorDlg(CWnd* pParent = NULL);// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_VIDEOPROCESSOR_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
	// ī�޶� ĸ�� ���� ��� ����
	CWebCam		m_webCam;

	// AVI ������ �б� ���� ��� ����
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
