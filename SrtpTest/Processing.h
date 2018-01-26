#pragma once
#include "ImageData.h"
#include <vector>
using std::vector;
using std::pair;
using std::make_pair;

class CProcessing
{
public:
	CProcessing();
	~CProcessing();
private:
	CImageData m_pro_reuslt;
	double* m_histogram;
	int m_max_icv_value;

public:
	
	//�໥У��
	CImageData MutualCorrect(const CImageData &img, double a, double b);

	//����У��
	//bool ContinuousCorrect(const CImageData &fst_img, const CImageData &snd_img, CImageData &res_img);
	CImageData ContinuousCorrect(const CImageData &fst_img, const CImageData &snd_img);

	//��ͬ���У��
	CImageData ContinuousCorrect(const CImageData &pre_img, const CImageData &cur_img, const CImageData &nxt_img);

	//����ֱ��ͼ
	double* CalcHistogram(const CImageData &img);
	double* CalcHistogram(const CImageData &img, char flg);

	//OTSU������ֵ
	int Otsu(const CImageData &img, int band_idx = 0, double rank = 1);
	
	//��ֵ��
	CImageData Binary(const CImageData &img, int threshold);
	
	//�ز���
	bool Subset(const CImageData &org_data, byte* res_data, int off_x, int off_y,
		int width, int height, int band_num, int band_order[]);

	//���򻮷�
	void ConnectedAreaMark(const CImageData &img, int &number,vector<int>&start,vector<int>&end,
		vector<int>&row,vector<int>& labels, vector<vector<int>>&overlap);
	
	//�����
	vector<int> Fill(const CImageData &img,byte *output, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap);

	vector<int> Fill(const CImageData &img, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap);


	//ɸѡ
	CImageData Filtrate(const CImageData &img, vector<int>&area, int area_thres, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap);

	//�ּ�
	CImageData Grade(const CImageData &img);

	//����ƽ���Ҷ�
	int CalcMeanValue(const CImageData &img, int threshold, int num=0);
};