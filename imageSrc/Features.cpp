#include "stdafx.h"
#include "Features.h"
#include "MyImageFunc.h"
#include <math.h>

void DrawLine(CByteImage& canvas, int x1, int y1, int x2, int y2, BYTE val)
{
	ASSERT(canvas.GetChannel()==1);

	int xs, ys, xe, ye;
	if (x1 == x2) // ������
	{
		if (y1 < y2) {ys = y1; ye = y2;}
		else		 {ys = y2; ye = y1;}
		for (int r=ys ; r<=ye ; r++)
		{
			canvas.GetAt(x1, r) = val;
		}
		return;
	}

	double a = (double)(y2-y1)/(x2-x1); // ����
	int nHeight = canvas.GetHeight();

	if ((a>-1) && (a<1)) // �����࿡ �����
	{
		if (x1 < x2) {xs = x1; xe = x2; ys = y1; ye = y2;}
		else		 {xs = x2; xe = x1; ys = y2; ye = y1;}
		for (int c=xs ; c<=xe ; c++)
		{
			int r = (int)(a*(c-xs) + ys + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r) = val;
		}
	}
	else // �����࿡ �����
	{
		double invA = 1.0 / a;
		if (y1 < y2) {ys = y1; ye = y2; xs = x1; xe = x2; }
		else		 {ys = y2; ye = y1; xs = x2; xe = x1; }
		for (int r=ys ; r<=ye ; r++)
		{
			int c = (int)(invA*(r-ys) + xs + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r) = val;
		}
	}
}

void DrawLine(CByteImage& canvas, int x1, int y1, int x2, int y2, BYTE R, BYTE G, BYTE B)
{
	ASSERT(canvas.GetChannel()==3);

	int xs, ys, xe, ye;
	if (x1 == x2) // ������
	{
		if (y1 < y2) {ys = y1; ye = y2;}
		else		 {ys = y2; ye = y1;}
		for (int r=ys ; r<=ye ; r++)
		{
			canvas.GetAt(x1, r, 0) = B;
			canvas.GetAt(x1, r, 1) = G;
			canvas.GetAt(x1, r, 2) = R;
		}
		return;
	}

	double a = (double)(y2-y1)/(x2-x1); // ����
	int nHeight = canvas.GetHeight();

	if ((a>-1) && (a<1)) // �����࿡ �����
	{
		if (x1 < x2) {xs = x1; xe = x2; ys = y1; ye = y2;}
		else		 {xs = x2; xe = x1; ys = y2; ye = y1;}
		for (int c=xs ; c<=xe ; c++)
		{
			int r = (int)(a*(c-xs) + ys + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r, 0) = B;
			canvas.GetAt(c, r, 1) = G;
			canvas.GetAt(c, r, 2) = R;
		}
	}
	else // �����࿡ �����
	{
		double invA = 1.0 / a;
		if (y1 < y2) {ys = y1; ye = y2; xs = x1; xe = x2; }
		else		 {ys = y2; ye = y1; xs = x2; xe = x1; }
		for (int r=ys ; r<=ye ; r++)
		{
			int c = (int)(invA*(r-ys) + xs + 0.5);
			if (r<0 || r>=nHeight)
				continue;
			canvas.GetAt(c, r, 0) = B;
			canvas.GetAt(c, r, 1) = G;
			canvas.GetAt(c, r, 2) = R;
		}
	}
}

int HoughLines(const CByteImage& imageIn, int nTNum, int nTVal, double resTheta, int numLine, double* pRho, double* pTheta)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep	= imageIn.GetWStep();

	int numRhoH = (int)(sqrt((double)(nWidth*nWidth + nHeight*nHeight))); // ���� �밢���� ����(���� �Ʒ� ������ ������ �� ��)
	int numRho = numRhoH*2; // rho�� ���� ������ ���� 2�� ����
	int numThe = 180 / resTheta;//numThe = �����ϴ� ���� ����,
								//180 => ������ 360������ ������ 0~180���� �����ϰ� ��ſ� ������ ����Ѵ�

	int numTrans = numRho*numThe; // rho�� theta ������ ���� Ƚ���� �����ϴ� ����(������ �迭 ũ��)

	double* sinLUT = new double[numThe]; // sin �Լ� ��� ���̺� ����(�ﰢ�Լ��� ������ ���)
	double* cosLUT = new double[numThe]; // cos �Լ� ��� ���̺� ����(�ﰢ�Լ��� ������ ���)
	double toRad = M_PI/numThe;//radian���� ��ȯ

	for (int theta=0 ; theta<numThe ; theta++)
	{
		sinLUT[theta] = (double)sin(theta*toRad);//��� ���̺� ���
		cosLUT[theta] = (double)cos(theta*toRad);//��� ���̺� ���
	}

	int* pCntTrans = new int[numTrans];
	memset(pCntTrans, 0, numTrans*sizeof(int));

	for (int r=1 ; r<nHeight ; r++)
	{
		BYTE* pIn = imageIn.GetPtr(r);
		for (int c=0 ; c<nWidth ; c++)
		{
			if (pIn[c] > nTVal )//�ȼ��� 0~255�� ���� ���� �� ������ �� �̻��� ������ �ȼ��� ��輱 �ȼ��� ����
			{					//�ٸ� ĳ�Ͽ����� 0 �ƴϸ� 255�� �����ϱ� �� if���� ��� ��� ���� ��
				for (int theta=0 ; theta<numThe ; theta++) 
				{
					int rho = (int)(c*sinLUT[theta] + r*cosLUT[theta] + numRhoH + 0.5);	//��輱 �ȼ��� ���Ͽ� �ȼ��� ������ ��� ������ ����Ͽ�
																						//c*sinLUT[theta] + r*cosLUT[theta] = ������ ������ = xsin + ycos
																						//numRhoH�� �� ��������?????
					pCntTrans[rho*numThe+theta]++;//�ش� �����鿡 ��ǥ, ��������� �� �迭���� �ش������� ������ ���� ������ �����
				}
			}
		}
	}

	// nThreshold���Ѵ� ��� ���� = ���ΰ� �̻��� ��ǥ�� ���� ������ ��� �迭�� ����
	int nLine = 0;

	for (int i=0 ; i<numTrans && nLine<numLine ; i++)//numLine = ����Ǵ� �ִ� ���� ��(pRho�� pTheta�� ����� ����Ǵ� �迭�� ũ��)
	{
		if (pCntTrans[i] > nTNum)//���� �ȼ� �� �̻� �ִ� ������ ���
		{
			pRho[nLine]	  = (int)(i/numThe); // rho�� �ε���, �Ÿ���
			pTheta[nLine] = (i - pRho[nLine] * numThe) * resTheta; //theta �� �ε���, ������
			pRho[nLine]	  -= numRhoH; // ���� ���� �����ϴ� ��ġ��ŭ ����
			nLine++;
		}
	}

	delete [] pCntTrans;
	delete [] sinLUT;
	delete [] cosLUT;

	return nLine;
}

CDoubleImage _Gaussian5x5(const CIntImage& imageIn)
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep();

	CDoubleImage imageGss(nWidth, nHeight);
	imageGss.SetConstValue(0);
	//int nWStepG = imageGss.GetWStep();

	int nWStep2 = 2*nWStep;

	for (int r=2 ; r<nHeight-2 ; r++)
	{
		double* pGss = imageGss.GetPtr(r);
		int*    pIn  = imageIn.GetPtr(r);
		for (int c=2 ; c<nWidth-2 ; c++)
		{
			pGss[c] = (2*(pIn[c-nWStep2-2] + pIn[c-nWStep2+2] + pIn[c+nWStep2-2] + pIn[c+nWStep2+2]) + 
					  4*(pIn[c-nWStep2-1] + pIn[c-nWStep2+1] + pIn[c-nWStep-2] + pIn[c-nWStep+2] +
						 pIn[c+nWStep-2] + pIn[c+nWStep+2] + pIn[c+nWStep2-1] + pIn[c+nWStep2+1]) + 
					  5*(pIn[c-nWStep2] + pIn[c-2] + pIn[c+2] + pIn[c+nWStep2]) + 
					  9*(pIn[c-nWStep-1] + pIn[c-nWStep+1] + pIn[c+nWStep-1] + pIn[c+nWStep+1]) + 
					  12*(pIn[c-nWStep] + pIn[c+nWStep] + pIn[c-1] + pIn[c+1]) + 
					  15*(pIn[c]))/159.0;
		}
	}
	
	return imageGss;
}

void boldCannyEdge(const CByteImage& imageIn, CByteImage& imageOut, int nThresholdHi, int nThresholdLo )
{
	int nWidth	= imageIn.GetWidth();
	int nHeight = imageIn.GetHeight();
	int nWStep = imageIn.GetWStep(); 

	// ����þ� ����ũ
	CDoubleImage  imageGss(nWidth, nHeight);
	imageGss = _Gaussian5x5(imageIn);

	// �Һ� ����ũ
	int Gx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	int Gy[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

	CDoubleImage  imageMag(nWidth, nHeight);
	CByteImage imageAng(nWidth, nHeight);
	int nWStepG = imageGss.GetWStep();
	int nWStepA = imageAng.GetWStep();

	double* pGss = imageGss.GetPtr();

	// �̺� ���ϱ�
	for (int r=0 ; r<nHeight ; r++)
	{
	double* pMag = imageMag.GetPtr(r);
	BYTE*   pAng = imageAng.GetPtr(r);
	for (int c=0 ; c<nWidth ; c++)
	{
		double sumX = 0.0;
		double sumY = 0.0;
		for (int y=0 ; y<3 ; y++)
		{
			for (int x=0 ; x<3 ; x++)
			{
				int py = r-1+y;
				int px = c-1+x;
				if (px>=0 && px<nWidth && py>=0 && py<nHeight)
				{
					sumX += Gx[y*3+x]*pGss[py*nWStepG+px];
					sumY += Gy[y*3+x]*pGss[py*nWStepG+px];
				}
			}
		}

		pMag[c] = sqrt(sumX*sumX + sumY*sumY); // ��輱�� ����
		double theta;					 // ��輱�� ���� ����
		if (pMag[c] == 0)
		{
			if(sumY == 0)
			{
				theta = 0;
			}
			else
			{
				theta = 90;
			}
		}
		else
		{
			theta = atan2((float)sumY, (float)sumX)*180.0/M_PI;
		}

		if ((theta > -22.5 && theta < 22.5) || theta > 157.5 || theta < -157.5)
		{
			pAng[c] = 0;
		}
		else if ((theta >= 22.5 && theta < 67.5) || (theta >= -157.5 && theta < -112.5))
		{
			pAng[c] = 45;
		}
		else if ((theta >= 67.5 && theta <= 112.5) || (theta >= -112.5 && theta <= -67.5))
		{
			pAng[c] = 90;
		}
		else
		{
			pAng[c] = 135;
		}
	} // �� �̵� ��
	} // �� �̵� ��

	// �� �ִ밪 ����
	CByteImage imageCand(nWidth, nHeight);
	imageCand.SetConstValue(0);

	for (int r=1 ; r<nHeight-1 ; r++)
	{
	BYTE*	pCand = imageCand.GetPtr(r);
	double* pMag  = imageMag.GetPtr(r);
	BYTE*   pAng  = imageAng.GetPtr(r);
	for (int c=1 ; c<nWidth-1 ; c++)
	{
		switch (pAng[c])
		{
		case 0:      // 0�� ���� ��
			if (pMag[c] > pMag[c - 1] && pMag[c] > pMag[c + 1])
			{
				pCand[c] = 255;
				pCand[c + 1] = 255;
				pCand[c - 1] = 255;
				pCand[c + 2] = 255;
				pCand[c - 2] = 255;

				pCand[c + nWStepG] = 255;
				pCand[c - nWStepG] = 255;
				pCand[c + nWStepG * 2] = 255;
				pCand[c - nWStepG * 2] = 255;

				pCand[c + 1 + nWStepG] = 255;
				pCand[c + 1 - nWStepG] = 255;
				pCand[c + 1 +  nWStepG * 2] = 255;
				pCand[c + 1 - nWStepG * 2] = 255;

				pCand[c - 1 + nWStepG] = 255;
				pCand[c - 1 - nWStepG] = 255;
				pCand[c - 1 + nWStepG * 2] = 255;
				pCand[c - 1 - nWStepG * 2] = 255;

				pCand[c + 2 + nWStepG] = 255;
				pCand[c + 2 - nWStepG] = 255;
				pCand[c + 2 + nWStepG * 2] = 255;
				pCand[c + 2 - nWStepG * 2] = 255;

				pCand[c - 2 + nWStepG] = 255;
				pCand[c - 2 - nWStepG] = 255;
				pCand[c - 2 + nWStepG * 2] = 255;
				pCand[c - 2 - nWStepG * 2] = 255;

			}
			break;
		case 45:   // 45�� ���� ��
			if (pMag[c] > pMag[c - nWStepG + 1] && pMag[c] > pMag[c + nWStepG - 1])
			{
				pCand[c] = 255;
				pCand[c - nWStepG - 1] = 255;
				pCand[c + nWStepG + 1] = 255;
				pCand[c - 2 * nWStepG - 2] = 255;
				pCand[c + 2 * nWStepG + 2] = 255;
			}
			break;
		case 90:      // 90�� ���� ��
			if (pMag[c] > pMag[c - nWStepG] && pMag[c] > pMag[c + nWStepG])
			{
				pCand[c] = 255;
				pCand[c + nWStepG] = 255;
				pCand[c - nWStepG] = 255;
				pCand[c + nWStepG * 2] = 255;
				pCand[c - nWStepG * 2] = 255;
			}
			break;
		case 135:   // 135�� ���� ��
			if (pMag[c] > pMag[c - nWStepG - 1] && pMag[c] > pMag[c + nWStepG + 1])
			{
				pCand[c] = 255;
				pCand[c - nWStepG + 1] = 255;
				pCand[c + nWStepG - 1] = 255;
				pCand[c - 2 * nWStepG + 2] = 255;
				pCand[c + 2 * nWStepG - 2] = 255;
			}
			break;
		}
	} // �� �̵� ��
	} // �� �̵� ��

	imageCand.SaveImage("Cand.bmp");
	
	// ���ΰ� �˻�
	imageOut = CByteImage(nWidth, nHeight);
	imageOut.SetConstValue(0);
	for (int r=1 ; r<nHeight-1 ; r++)
	{
	BYTE*	pOut  = imageOut.GetPtr(r);
	BYTE*	pCand = imageCand.GetPtr(r);
	double* pMag  = imageMag.GetPtr(r);
	BYTE*   pAng  = imageAng.GetPtr(r);
	for (int c=1 ; c<nWidth-1 ; c++)
	{
		if (pCand[c])
		{
			if (pMag[c] > nThresholdHi)
			{
				pOut[c] = 255;
			}
			else if (pMag[c] > nThresholdLo) // ����� �ȼ� �˻�
			{
				bool bIsEdge = true;
				switch (pAng[c])
				{
				case 0:		// 90�� ���� �˻�
					if ((pMag[c-nWStepG] > nThresholdHi) || 
						(pMag[c+nWStepG] > nThresholdHi))
					{
						pOut[c] = 255;
					}
					break;
				case 45:	// 135�� ���� �˻�
					if ((pMag[c-nWStepG-1] > nThresholdHi) || 
						(pMag[c+nWStepG+1] > nThresholdHi))
					{
						pOut[c] = 255;
					}
					break;
				case 90:		// 0�� ���� �˻�
					if ((pMag[c-1] > nThresholdHi) || 
						(pMag[c+1] > nThresholdHi))
					{
						pOut[c] = 255;
					}
					break;
				case 135:	// 45�� ���� �˻�
					if ((pMag[c-nWStepG+1] > nThresholdHi) || 
						(pMag[c+nWStepG-1] > nThresholdHi))
					{
						pOut[c] = 255;
					}
					break;
				}
			}
		}
	} // �� �̵� ��
	} // �� �̵� ��
}


