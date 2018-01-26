#include <iostream>
#include <fstream>
#include "Processing.h"
#include <list>
#include <cmath>

using std::cout;
using std::pow;

CProcessing::CProcessing()
	:m_max_icv_value(0)
{
	m_histogram = NULL;
}

CProcessing::~CProcessing()
{
	if(m_histogram!=NULL)
	{
		delete[] m_histogram;
		m_histogram = NULL;
	}

}


CImageData CProcessing::MutualCorrect(const CImageData &img, double a, double b)
{
	cout << "\nMutual Calibrating ...\n";
	int n = img.GetBandCount();
	int w = img.GetWidth();
	int h = img.GetHeight();
	byte* data = img.GetOriginData();

	byte* output_data = new byte[w*h];
	//for(int k=0;k<n;k++)
	//{
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				output_data[i*w+j] = (byte)(a*pow((double)data[i*w+j],b));
				//output_data[i*w+j] = data[i*w+j];
			}
		}
	//}

	CImageData output_img = img;
	output_img.SetOriginData(output_data);
	delete[] output_data;
	output_data = NULL;

	return output_img;
}

//bool CProcessing::ContinuousCorrect(const CImageData &fst_img, const CImageData &snd_img, CImageData &res_img)
//{
//	cout << "\nContinuous Correcting ...\n";
//	int n = fst_img.GetBandCount();
//	int w = fst_img.GetWidth();
//	int h = fst_img.GetHeight();
//	if((snd_img.GetWidth() != w) || (snd_img.GetHeight() != h))
//	{
//		cout << "These two images have different size!\n";
//		return false;
//	}
//	byte* fst_data = fst_img.GetOriginData();
//	byte* snd_data = snd_img.GetOriginData();
//
//	byte* output_data = new byte[w*h];
//	for(int i=0;i<h;i++)
//	{
//		for(int j=0;j<w;j++)
//		{
//			if((0 == fst_data[i*w+j]) && (0 == snd_data[i*w+j]))
//			{
//				output_data[i*w+j] = 0;
//			}
//			else
//			{
//				output_data[i*w+j] = (byte)(((int)fst_data[i*w+j]+(int)snd_data[i*w+j])/2);
//			}
//		}
//	}
//
//	res_img = fst_img;
//	res_img.SetOriginData(output_data);
//	delete []output_data;
//
//	return true;
//}

CImageData CProcessing::ContinuousCorrect(const CImageData &fst_img, const CImageData &snd_img)
{
	cout << "\nContinuous Correcting in Different Sensors ...\n";
	int n = fst_img.GetBandCount();
	int w = fst_img.GetWidth();
	int h = fst_img.GetHeight();
	if((snd_img.GetWidth() != w) || (snd_img.GetHeight() != h))
	{
		cout << "These two images have different size!\n";
	}
	byte* fst_data = fst_img.GetOriginData();
	byte* snd_data = snd_img.GetOriginData();

	byte* output_data = new byte[w*h];
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if((0 == fst_data[i*w+j]) && (0 == snd_data[i*w+j]))
			{
				output_data[i*w+j] = 0;
			}
			else
			{
				output_data[i*w+j] = (byte)(((int)fst_data[i*w+j]+(int)snd_data[i*w+j])/2);
			}
		}
	}

	CImageData output_img = fst_img;
	output_img.SetOriginData(output_data);
	delete []output_data;

	return output_img;
}

CImageData CProcessing::ContinuousCorrect(const CImageData &pre_img, const CImageData &crt_img, const CImageData &nxt_img)
{
	cout << "\nContinuous Correcting in Different Years ...\n";
	int n = pre_img.GetBandCount();
	int w = pre_img.GetWidth();
	int h = pre_img.GetHeight();
	if((crt_img.GetWidth() != w) || (crt_img.GetHeight() != h) ||
		(nxt_img.GetWidth() != w) || (nxt_img.GetHeight() != h))
	{
		cout << "These three images have different size!\n";
	}
	byte* pre_data = pre_img.GetOriginData();
	byte* crt_data = crt_img.GetOriginData();
	byte* nxt_data = nxt_img.GetOriginData();

	byte* res_data = new byte[w*h];
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if(0 == nxt_data[i*w+j])
				res_data[i*w+j] = 0;
			else if((nxt_data[i*w+j] > 0) && (pre_data[i*w+j] > crt_data[i*w+j]))
				res_data[i*w+j] = pre_data[i*w+j];
			else
				res_data[i*w+j] = crt_data[i*w+j];
		}
	}

	CImageData output_img = crt_img;
	output_img.SetOriginData(res_data);
	delete []res_data;

	return output_img;
}

double* CProcessing::CalcHistogram(const CImageData &img_data)
{
	cout << "\nCalculating Histogram ...\n";
	int band_num = img_data.GetBandCount();
	int w = img_data.GetWidth();
	int h = img_data.GetHeight();

	byte* byte_data=new byte[band_num*w*h];
	double Max=0,Min=0;
	//0-255灰度拉伸处理
	for(int k=0;k<band_num;k++)
	{
		//该波段的像素最大值与最小值
		Min=img_data.GetMetadata().value_range[k][0];
		Max=img_data.GetMetadata().value_range[k][1];
		//拉伸时所用的系数
		double scale=255/(Max-Min);
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				double org_data = img_data.GetOriginData()[k*w*h+i*w+j];
				int value=scale*(org_data-Min);
				if(value>255)
				{
					value=255;
				}
				if(value<0)
				{
					value=0;
				}
				byte_data[k*w*h+i*w+j]=(byte)value;
			}
		}
	}
	if(m_histogram!=NULL)
	{
		delete[] m_histogram;
		m_histogram = NULL;
	}
	m_histogram = new double[band_num*256];
	for(int k=0;k<band_num;k++)
	{
		for(int i=0;i<256;i++)
		{
			m_histogram[k*256+i]=0;
		}
	}
	
	for(int k=0;k<band_num;k++)
	{
		for(int i=0;i<h;i++)
			for(int j=0;j<w;j++)
			{
				int crt_value = (int)byte_data[k*w*h+i*w+j];
				m_histogram[k*256+crt_value]++;
			}
	}

	return m_histogram;
}

double* CProcessing::CalcHistogram(const CImageData &img_data, char flg)
{
	if(flg != 'b' && flg != 'g')
	{
		return NULL;
	}
	if(m_histogram!=NULL)
	{
		delete[] m_histogram;
		m_histogram = NULL;
	}
	m_histogram = new double[256];
	for(int i=0;i<256;i++)
	{
		m_histogram[i]=0;
	}

	int h = img_data.GetHeight();
	int w = img_data.GetWidth();
	int N = 0;
	
	byte* data = img_data.GetOriginData();
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if('g' == flg)
				if((int)data[i*w+j] < 38)
					continue;
			int index = (int)data[i*w+j];
			int num = m_histogram[index];
			m_histogram[(int)data[i*w+j]]++;
			N++;
		}
	}
	for(int i=0;i<256;i++)
	{
		m_histogram[i] = m_histogram[i]/N;
	}

	return m_histogram;
}


int CProcessing::Otsu(const CImageData &img, int band_idx, double rank)
{
	using std::ofstream;
	int b = band_idx;
	int w = img.GetWidth();
	int h = img.GetHeight();
	double* hist = CalcHistogram(img, 'b');
	
	/*T: 输出histogram测试-------------------------------------------------------
	ofstream hist_out;
	string out_file_name = "D:\\Documents\\Study\\2017SRTP\\DATA\\hist.txt";
	hist_out.open(out_file_name);
	for(int i=0;i<256;i++)
		hist_out << i << ',' << hist[i] <<'\n';
	hist_out.close(); 
	----------------------------------------------------------------------------*/

	double accu_hist[256];
	double mu[256];
	
	cout << "\nCalculating ICV ...\n";
	accu_hist[0]=hist[b*256];
	mu[0]=0;
	for(int i=1;i<256;i++)
	{
		accu_hist[i] = accu_hist[i-1] + hist[b*256+i];
		mu[i] = mu[i-1] + i*hist[b*256+i];
	}

	/* T: 输出histogram测试-------------------------------------------------------
	out_file_name = "D:\\Documents\\Study\\2017SRTP\\DATA\\icvs.txt";
	hist_out.open(out_file_name);
	------------------------------------------------------------------------------*/

	double mean = mu[255];	//灰度平均值
	double t_max = 0;
	for(int i=1;i<255;i++)
	{
		double PA = accu_hist[i];	//A类所占的比例
		double PB = 1 - accu_hist[i];	//B类所占的比例
		double icv = 0;
		if(fabs(PA) > 0.001 && fabs(PB) > 0.001)
		{
			double MA = mu[i]/PA;
			double MB = (mean-mu[i])/PB;
			icv = pow(PA, rank)*(MA-mean)*(MA-mean) + pow(PB, rank)*(MB-mean)*(MB-mean);
			//icv = PA*(MA-mean)*(MA-mean) + PB*(MB-mean)*(MB-mean);

			//hist_out << i << ',' << icv << '\n';		//T: 输出histogram测试

			if(icv>t_max)
			{
				t_max = icv;
				m_max_icv_value = i;
			}
		}
	}
	//hist_out.close();		//T: 输出histogram测试

	return m_max_icv_value;
}

CImageData CProcessing::Binary(const CImageData &input_img, int threshold)
{
	cout << "\nBinarization processing ...\n";
	int n = input_img.GetBandCount();
	int w = input_img.GetWidth();
	int h = input_img.GetHeight();
	byte* input_data = input_img.GetOriginData();
	
	byte* output_data = new byte[w*h];
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if(input_data[i*w+j]>threshold)
				output_data[i*w+j] = 255;
			else
				output_data[i*w+j] = 0;

		}
	}

	CImageData output_img = input_img;
	output_img.SetOriginData(output_data);
	delete[] output_data;

	return output_img;
}

bool CProcessing::Subset(const CImageData &org_data, byte* res_data, int off_x, int off_y, int width, int height, int band_num, int band_order[])
{
	if(NULL==org_data.GetOriginData())
	{
		return false;
	}
	if(0==band_num)
	{
		return false;
	}

	int o_w = org_data.GetWidth();
	int o_h = org_data.GetHeight();
	int n = band_num;
	int w = width;
	int h = height;
	for(int k=0;k<n;k++)
	{
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				res_data[k*h*w+i*w+j] = org_data.GetOriginData()[band_order[k]*o_w*o_h+(i+off_y)*o_w+(j+off_x)];
			}
		}
	}

	return true;
}

void CProcessing::ConnectedAreaMark(const CImageData &img, int &number,vector<int>&start,vector<int>&end,
		vector<int>&row,vector<int>& labels, vector<vector<int>>&overlap)
{
	cout << "\nMarking connected areas ...\n";
	int n = img.GetBandCount();
	int w = img.GetWidth();
	int h = img.GetHeight();
	byte* data=img.GetOriginData();

	//获取连通区域
	//for(int k=0;k<n;k++)
	//{
		for(int i = 0; i<h;i++)
		{
			if(data[i*w]==255)
			{
				number++;
				start.push_back(0);
				row.push_back(i);
			}
			for(int j =1; j<w;j++)
			{
				if(data[i*w+j-1]==0 && data[i*w+j]==255)
				{
					number++;
					start.push_back(j);
					row.push_back(i);
				}
				else if(data[i*w+j-1]==255 && data[i*w+j]==0)
				{
					end.push_back(j-1);
				}
			}
			if(data[i*w+w-1]==255)
			{
				end.push_back(w-1);
			}
		}
	//}

	//提取等价对
	vector<pair<int,int>> equivalences;
	int offset = 0;
	labels.assign(number, -1);
	int idx_label = 0;
	int cur_row_idx = 0;
	int first_run_on_cur = 0;
	int first_run_on_pre = 0;
	int last_run_on_pre = -1;
	for(int i=0; i<number; i++)
	{
		if(row[i]!=cur_row_idx)
		{
			cur_row_idx = row[i];
			first_run_on_pre = first_run_on_cur;
			last_run_on_pre = i-1;
			first_run_on_cur = i;
		}
		for(int j = first_run_on_pre; j<=last_run_on_pre;j++)
		{
			if(start[i] <= end[j]+offset && end[i] >= start[j]-offset && row[i] == row[j]+1)
			{
				if(labels[i] == -1)
				{
					labels[i] = labels[j];
				}
				else if(labels[i] != labels[j])
				{
					equivalences.push_back(make_pair(labels[i], labels[j]));
				}
			}

		}
		if(labels[i]==-1)
		{
			labels[i]=idx_label++;
		}

	}

	//合并等价对
	int label_num = idx_label-1;
	if(equivalences.size() > 0)
	{
		for(int i=0; i<equivalences.size();i++)
		{
			vector<int>cur;		//存放当前正在合并的团

			//初始化当前合并的团（即把第一个未合并的的等价对放入其中）
			int first = equivalences[i].first;
			int second = equivalences[i].second;	
			cur.push_back(first);
			cur.push_back(second);

			//从第一个未合并的等价对之后开始对比判断
			for(int j=i+1;j<equivalences.size();j++)
			{
				int pre_size = cur.size();		//当前合并团在该次合并前的大小
				for(int k=0;k<cur.size();k++)
				{
					//判断当前合并团是否与当前等价对有重叠，
					//如果重叠，则将另一团标号放入当前合并团，并跳出循环
					if(cur[k] == equivalences[j].first)
					{
						cur.push_back(equivalences[j].second);
						vector<pair<int,int>>::iterator iter = equivalences.begin()+j;
						equivalences.erase(iter);
						iter = equivalences.begin();
						break;
					}
					else if(cur[k]==equivalences[j].second)
					{
						cur.push_back(equivalences[j].first);
						vector<pair<int,int>>::iterator iter = equivalences.begin()+j;
						equivalences.erase(iter);
						iter = equivalences.begin();
						break;
					}
				}

				//判断此次合并是否有新团标号加入，
				//如果是，则需从第一个未合并的等价对之后开始对比判断
				if(pre_size!=cur.size())
				{
					j=i;
				}
			}
			overlap.push_back(cur);		//将当前合并完成的团放入结果vector中
		}
		for(int i = 0;i<=label_num;i++)
		{
			for(int j=0;j<overlap.size();j++)
			{
				int k=0;
				for(;k<overlap[j].size();k++)
					if(overlap[j][k]==i)
						break;
				if(k<overlap[j].size())
					break;
				else if(j==overlap.size()-1)
				{
					vector<int>cur;
					cur.push_back(i);
					overlap.push_back(cur);
				}
			}
		}
	}
	else
	{
		for(int i=0;i<=label_num;i++)
		{
			vector<int> cur;
			cur.push_back(i);
			overlap.push_back(cur);
		}
	}
}

vector<int> CProcessing::Fill(const CImageData &img,byte *output, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap)
{
	cout << "\nFilling ...\n";
	int w = img.GetWidth();
	int block_num = overlap.size();
	vector<int>area;
	area.assign(block_num, 0);
	for(int k=0;k<number;k++)
	{
		for(int j=0;j<block_num;j++)
		{
			for(int t=0;t<overlap[j].size();t++)
			{
				if(labels[k]==overlap[j][t])
				{
					int r = row[k];
					for(int i=start[k];i<=end[k];i++)
					{
						output[r*w+i]=j+1;
						area[j]++;
					}
				}
			}
		}
	}

	return area;
}

vector<int> CProcessing::Fill(const CImageData &img, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap)
{
	cout << "\nFilling ...\n";
	int w = img.GetWidth();
	int block_num = overlap.size();
	vector<int>area;
	area.assign(block_num, 0);
	for(int k=0;k<number;k++)
	{
		for(int j=0;j<block_num;j++)
		{
			for(int t=0;t<overlap[j].size();t++)
			{
				if(labels[k]==overlap[j][t])
				{
					int r = row[k];
					for(int i=start[k];i<=end[k];i++)
					{
						area[j]++;
					}
				}
			}
		}
	}

	return area;
}

CImageData CProcessing::Filtrate(const CImageData &img, vector<int>&area, int area_thres, const vector<int>&row, const vector<int>&start, 
		  const vector<int>&end, int number, const vector<int>&labels, const vector<vector<int>>&overlap)
{
	cout << "\nFiltrating ...\n";
	int h = img.GetHeight();
	int w = img.GetWidth();
	byte *res = new byte[w*h];
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			res[i*w+j]=0;
		}
	}

	int block_num = overlap.size();

	for(int i=0;i<number;i++)
	{
		for(int j=0;j<block_num;j++)
		{
			int r = row[i];
			if(area[j]<area_thres)
			{
				for(int t=0;t<overlap[j].size();t++)
				{
					if(labels[i]==overlap[j][t])
					{		
						for(int k=start[i];k<=end[i];k++)
						{
							res[r*w+k]=0;
						}
					}
				}
				area[j]=0;
			}
			else
			{
				for(int t=0;t<overlap[j].size();t++)
				{
					if(labels[i]==overlap[j][t])
					{		
						for(int k=start[i];k<=end[i];k++)
						{
							res[r*w+k]=255;
						}
					}
				}
			}

		}
	}

	CImageData res_img = img;
	res_img.SetOriginData(res);

	delete []res;
	return res_img;
}

int CProcessing::CalcMeanValue(const CImageData &img, int threshold, int num)
{
	int n = img.GetBandCount();
	int w = img.GetWidth();
	int h = img.GetHeight();
	
	int sum = 0;
	int mean = 0;
	byte* input_data = img.GetOriginData();
	
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++)
		{
			if(input_data[i*w+j]>threshold)
			{
				sum += (int)input_data[i*w+j];
				num++;
			}
		}
	}

	if(num != 0)
		mean = sum/num;
	return mean;
}
