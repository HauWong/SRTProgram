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
	
	//相互校正
	CImageData MutualCorrect(const CImageData &img, double a, double b);

	//连续校正
	//bool ContinuousCorrect(const CImageData &fst_img, const CImageData &snd_img, CImageData &res_img);
	CImageData ContinuousCorrect(const CImageData &fst_img, const CImageData &snd_img);

	//不同年份校正
	CImageData ContinuousCorrect(const CImageData &pre_img, const CImageData &cur_img, const CImageData &nxt_img);

	//计算直方图
	double* CalcHistogram(const CImageData &img);
	double* CalcHistogram(const CImageData &img, char flg);

	//OTSU计算阈值
	int Otsu(const CImageData &img, int band_idx = 0, double rank = 1);
	
	//二值化
	CImageData Binary(const CImageData &img, int threshold);
	
	//重采样
	bool Subset(const CImageData &org_data, byte* res_data, int off_x, int off_y,
		int width, int height, int band_num, int band_order[]);

	//区域划分
	void ConnectedAreaMark(const CImageData &img, int &number,vector<int>&start,vector<int>&end,
		vector<int>&row,vector<int>& labels, vector<vector<int>>&overlap);
	
	//填充标记
	vector<int> Fill(const CImageData &img,byte *output, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap);

	vector<int> Fill(const CImageData &img, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap);


	//筛选
	CImageData Filtrate(const CImageData &img, vector<int>&area, int area_thres, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap);

	//分级
	CImageData Grade(const CImageData &img);

	//计算平均灰度
	int CalcMeanValue(const CImageData &img, int threshold, int num=0);
};