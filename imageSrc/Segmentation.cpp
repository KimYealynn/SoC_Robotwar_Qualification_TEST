#include "stdafx.h"
#include "Segmentation.h"
#include "MyImageFunc.h"


void Binarization(const CDoubleImage& imageIn, CByteImage& imageOut, int startH, int endH, int startS, int endS, int startV, int endV)
{
	ASSERT(imageIn.GetChannel() == 3);

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();

	CByteImage m_imageH = imageIn.GetChannelImg(2)*(255.0 / 360.0) + 0.5; //360뭐엿는지
	CByteImage m_imageS = imageIn.GetChannelImg(1)*(255.0) + 0.5;
	CByteImage m_imageV = imageIn.GetChannelImg(0)*(255.0) + 0.5;

	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pInH = m_imageH.GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		//H
		for (int c = 0; c<nWidth; c++)
		{
			if (pInH[c] > startH && pInH[c]<endH)
				pOut[c] = 255;//찾는 색이 흰색
			else
				pOut[c] = 0;//찾지 않는 색이 검은색
		}

		//S
		BYTE* pInS = m_imageS.GetPtr(r);

		for (int c = 0; c<nWidth; c++)
		{
			if (pOut[c] == 0 || (pInS[c] < startS || pInS[c]>endS))
				pOut[c] = 0;
			else
				pOut[c] = 255;
		}

		//V
		BYTE* pInV = m_imageV.GetPtr(r);

		for (int c = 0; c<nWidth; c++)
		{
			if (pOut[c] == 0 || (pInV[c] < startV || pInV[c]>endV))
				pOut[c] = 0;
			else
				pOut[c] = 255;
		}


	}
}

void Binarization_For_Red(const CDoubleImage& imageIn, CByteImage& imageOut, int startH, int endH, int startS, int endS, int startV, int endV)
{
	ASSERT(imageIn.GetChannel() == 3);

	int nWidth = imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();


	CByteImage m_imageH = imageIn.GetChannelImg(2)*(255.0 / 360.0) + 0.5;
	CByteImage m_imageS = imageIn.GetChannelImg(1)*(255.0) + 0.5;
	CByteImage m_imageV = imageIn.GetChannelImg(0)*(255.0) + 0.5;


	for (int r = 0; r<nHeight; r++)
	{
		BYTE* pInH = m_imageH.GetPtr(r);
		BYTE* pOut = imageOut.GetPtr(r);

		//H
		for (int c = 0; c<nWidth; c++)
		{
			if (pInH[c] > startH && pInH[c]<endH)
				pOut[c] = 255;//찾는 색이 흰색,H_R1을 이용해서 구한 게 지워지지 않게 0이 되는 부분을 없앰
		}

		//S

		BYTE* pInS = m_imageS.GetPtr(r);

		for (int c = 0; c<nWidth; c++)
		{
			if (pOut[c] == 0 || (pInS[c] < startS || pInS[c]>endS))
				pOut[c] = 0;
			else
				pOut[c] = 255;
		}


		//V

		BYTE* pInV = m_imageV.GetPtr(r);

		for (int c = 0; c<nWidth; c++)
		{
			if (pOut[c] == 0 || (pInV[c] < startV || pInV[c]>endV))
				pOut[c] = 0;
			else
				pOut[c] = 255;
		}


	}
}
