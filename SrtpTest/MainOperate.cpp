#include <afx.h>
#include <afxdlgs.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "ImageData.h"
#include "Processing.h"
using namespace std;

//const int NUM = 23;
struct Param
{
	int sens_num;	//可以不用传感器标号
	int year;
	double a;
	double b;
};

vector<vector<Param>> g_params;

void InitParameters();
void SetParameters();
bool Correct();
bool Extract();
bool Grade();
bool CalculateMeanValeu();
int main()
{
	InitParameters();
	char start_flg;
	cout << "Welcome!\n";
	cout << "1. Correction\n";
	cout << "2. Extraction\n";
	cout << "3. Grade\n";
	cout << "4. Get Mean Value\n";
	cout << "5. Out\n";
	cout << "Please Input the Operation Number You Want: ";
	while (cin >> start_flg)
	{
		switch(start_flg)
		{
		case '1':
			Correct();
			break;
		case '2':
			Extract();
			break;
		case '3':
			Grade();
			break;
		case '4':
			CalculateMeanValeu();
			break;
		default:
			return 0;
		}

		cout << "\n1. Correction";
		cout << "\n2. Extraction";
		cout << "\n3. Grade";
		cout << "\n4. Get Mean Value";
		cout << "\n5. Out";
		cout << "\nNext One: ";
	}

	return 0;
}

void InitParameters()
{
	Param ps[] = {
		{12, 1999, 0.7741, 1.2315},
		{14, 1999, 1.0912, 1.2263},
		{14, 2000, 0.9913, 1.1753},
		{14, 2001, 0.8686, 1.2266},
		{14, 2002, 0.9894, 1.1583},
		{14, 2003, 0.9003, 1.2019},
		{15, 2000, 0.8093, 1.1830},
		{15, 2001, 0.6655, 1.2646},
		{15, 2002, 0.5923, 1.2795},
		{15, 2003, 1.0439, 1.1822},
		{15, 2004, 1.1450, 1.0847},
		{15, 2005, 1.3548, 1.0228},
		{15, 2006, 1.2481, 1.0738},
		{15, 2007, 1.1421, 1.1297},
		{16, 2004, 0.7415, 1.1576},
		{16, 2005, 0.9048, 1.1776},
		{16, 2006, 0.8296, 1.1883},
		{16, 2007, 0.7314, 1.2132},
		{16, 2008, 0.7927, 1.1487},
		{16, 2009, 0.6051, 1.1525},
		{18, 2010, 0.3427, 1.2188},
		{18, 2011, 0.7035, 1.0872},
		{18, 2012, 0.4821, 1.1866},
		{18, 2013, 1.0000, 1.0000}
	};
	vector<Param> ps_12;
	vector<Param> ps_14;
	vector<Param> ps_15;
	vector<Param> ps_16;
	vector<Param> ps_18;

	ps_12.push_back(ps[0]);
	g_params.push_back(ps_12);
	int i=1;
	for(;i<6;i++)
	{
		ps_14.push_back(ps[i]);
	}
	g_params.push_back(ps_14);
	for(;i<14;i++)
	{
		ps_15.push_back(ps[i]);
	}
	g_params.push_back(ps_15);
	for(;i<20;i++)
	{
		ps_16.push_back(ps[i]);
	}
	g_params.push_back(ps_16);
	for(;i<24;i++)
	{
		ps_18.push_back(ps[i]);
	}
	g_params.push_back(ps_18);

}

Param SetParameters(int sens_num, int year)
{
	double a;
	double b;
	Param p;
	p.sens_num = sens_num;
	p.year = year;
	switch(sens_num)
	{
	case 12:
		a = g_params[0][year-1999].a;
		b = g_params[0][year-1999].b;
		break;
	case 14:
		a = g_params[1][year-1999].a;
		b = g_params[1][year-1999].b;
		break;
	case 15:
		a = g_params[2][year-2000].a;
		b = g_params[2][year-2000].b;
		break;
	case 16:
		a = g_params[3][year-2004].a;
		b = g_params[3][year-2004].b;
		break;
	case 18:
		a = g_params[4][year-2010].a;
		b = g_params[4][year-2010].b;
		break;
	default:
		break;
	}

	p.a = a;
	p.b = b;
	return p;
}


bool Correct()
{
	//第一张影像
	char* filter = "TIFF (*.tif)|*.tif|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|BMP (*.bmp)|*.bmp||";
	string default_path = "D:\\Documents\\Study\\2017SRTP\\DATA\\roi_results\\";
	string file_path;
	AfxSetResourceHandle(GetModuleHandle(NULL));
	CFileDialog fst_dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "\nSelect the First File: ";
	if(IDOK == fst_dlg.DoModal())
	{
		file_path = fst_dlg.GetPathName();
		cout << file_path << endl;
	}
	else
		return false;
	CImageData img_data(file_path.c_str());

	CProcessing proc;

	//相互校正
	double t_a, t_b;	//相互校正参数
	cout << "The parameters of Mutual Correction for the first image:\na = ";
	int year = atoi(img_data.GetName().substr(0, 4).c_str());
	int sens_num = atoi(img_data.GetName().substr(5).c_str());
	t_a = SetParameters(sens_num, year).a;
	t_b = SetParameters(sens_num, year).b;
	cout << t_a << "\nb = ";
	cout << t_b << '\n';

	CImageData fst_corrected_img = proc.MutualCorrect(img_data, t_a, t_b);

	//输出测试
	//string  test_path = "D:\\Desktop\\test_1.jpg";
	//fst_corrected_img.SaveToFile(test_path.c_str());

	//第二张影像
	string snd_file_path;
	CFileDialog snd_dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "\nThen Select the Second File: ";
	if(IDOK == snd_dlg.DoModal())
	{
		snd_file_path = snd_dlg.GetPathName();
		cout << snd_file_path << endl;
	}
	else
		return false;
	CImageData snd_img_data(snd_file_path.c_str());

	//相互校正
	cout << "The parameters of Mutual Correction for the second image:\na = ";
	year = atoi(snd_img_data.GetName().substr(0, 4).c_str());
	sens_num = atoi(snd_img_data.GetName().substr(5).c_str());
	t_a = SetParameters(sens_num, year).a;
	t_b = SetParameters(sens_num, year).b;
	//cin >> t_a;
	cout << t_a << "\nb = ";
	cout << t_b << '\n';
	CImageData snd_corrected_img = proc.MutualCorrect(snd_img_data, t_a, t_b);

	//输出测试
	//string  test_path_2 = "D:\\Desktop\\test_2.jpg";
	//snd_corrected_img.SaveToFile(test_path_2.c_str());

	//连续校正
	CImageData corrected_img = proc.ContinuousCorrect(fst_corrected_img, snd_corrected_img);
	//if(!proc.ContinuousCorrect(fst_corrected_img, snd_corrected_img, corrected_img))
	//{
	//	cout << "\nNext One? (Y/N) ";
	//	continue;
	//}

	//保存
	string tmp_file_path = "D:\\Documents\\Study\\2017SRTP\\DATA\\tmp_output\\";
	CFileDialog save_dlg(FALSE, "jpg", tmp_file_path.c_str(), OFN_HIDEREADONLY, 
			"(*.*)|*.*|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif||");
	cout << "\nPlease Select the Path to save:";
	if(save_dlg.DoModal() != IDOK)
	{
		return false;
	}
	tmp_file_path = save_dlg.GetPathName();
	bool is_saved = corrected_img.SaveToFile(tmp_file_path.c_str());
	if(is_saved)
		cout << tmp_file_path << "\n";
	else
		cout << "\nTemporary File Save Failed!\n";

	return true;
}

bool Extract()
{
	char* filter = "JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif||";
	string default_path = "D:\\Documents\\Study\\2017SRTP\\DATA\\tmp_output\\";
	string pre_file_path;
	AfxSetResourceHandle(GetModuleHandle(NULL));
	CFileDialog pre_dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "Select the Previous Year File: ";
	if(IDOK == pre_dlg.DoModal())
	{
		pre_file_path = pre_dlg.GetPathName();
		cout << pre_file_path << endl;
	}
	else
		return false;
	
	string cur_file_path;
	CFileDialog cur_dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "Select the Current Year File: ";
	if(IDOK == cur_dlg.DoModal())
	{
		cur_file_path = cur_dlg.GetPathName();
		cout << cur_file_path << endl;
	}
	else
		return false;
	
	string nxt_file_path;
	CFileDialog nxt_dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "Select the Next Year File: ";
	if(IDOK == nxt_dlg.DoModal())
	{
		nxt_file_path = nxt_dlg.GetPathName();
		cout << nxt_file_path << endl;
	}
	else
		return false;

	CImageData pre_img_data(pre_file_path.c_str());
	CImageData cur_img_data(cur_file_path.c_str());
	CImageData nxt_img_data(nxt_file_path.c_str());

	CProcessing proc;
	CImageData corrected_img = proc.ContinuousCorrect(pre_img_data, cur_img_data, nxt_img_data);
	corrected_img.SaveToFile((cur_file_path+".t.jpg").c_str());

	
	//CImageData corrected_img(pre_file_path.c_str());  //T: 输出histogram测试

	//OTSU方法计算二值化阈值
	int threshold = 32;
	//threshold = proc.Otsu(corrected_img);
	cout << "The Threshold Is " << threshold << endl;

	//二值化处理
	CImageData binary_img = proc.Binary(corrected_img, threshold);
	////binary_img.SaveToFile("D:\\Desktop\\tmp-t.jpg");	//T: 输出二值图像测试

	//连通区域标记
	int num=0;
	vector<int>row;
	vector<int>start;
	vector<int>end;
	vector<int>labels;
	vector<vector<int>>overlap;
	proc.ConnectedAreaMark(binary_img,num,start,end,row,labels,overlap);
	
	//填充标记区域

	int w = corrected_img.GetWidth();
	int h = corrected_img.GetHeight();

	//byte* test = new byte[w*h];						//T: 填充后的测试数据
	vector<int> area = proc.Fill(binary_img,row,start,end,num,labels,overlap);
	cout << "Blocks Count: " << area.size() << endl;
	for(int i=0;i<area.size();i++)
	{
		cout << "Block " << i << ": " << area[i] << " pixels"<<endl;
	}

	//for(int i=0;i<h;i++)								//T: 输出填充后的测试数据
	//{
	//	for(int j=0;j<w;j++)
	//	{
	//		cout << (int)test[i*w+j] << ' ';
	//	}
	//	cout << endl;
	//}


	//面积筛选
	int total_area=0;
	int area_thres = 2;
	CImageData res_img = proc.Filtrate(binary_img,area,area_thres,row,start,end,num,labels,overlap);
	//res_img.Colour();
	for(int i=0;i<area.size();i++)
	{
		total_area += area[i];
	}
	cout << "Total Area is " << total_area << " Pixels!\n";
		
	char save_flg;
	cout << "\nSave to File? (Y/N) ";
	cin >> save_flg;
	string output_file_path = "D:\\Documents\\Study\\2017SRTP\\DATA\\output\\";
	if('Y' == save_flg || 'y' == save_flg)
	{
		CFileDialog save_dlg(FALSE, "jpg", output_file_path.c_str(), OFN_HIDEREADONLY, 
			"(*.*)|*.*|JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif||");
		if(save_dlg.DoModal() != IDOK)
		{
			return false;
		}
		output_file_path = save_dlg.GetPathName();
		bool is_saved = res_img.SaveToFile(output_file_path.c_str());
		if(is_saved)
			cout << "File "<< output_file_path << " Has Been Saved!\n";
		else
			cout << "\nSave Failed!\n";
	}
	cout << "\nCompleted!"<<endl;

	cin.get();

	return true;
}

bool Grade()
{
	char* filter = "JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif||";
	string default_path = "D:\\Documents\\Study\\2017SRTP\\DATA\\tmp_output\\";
	string file_path;
	AfxSetResourceHandle(GetModuleHandle(NULL));
	CFileDialog dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "Select File: ";
	if(IDOK == dlg.DoModal())
	{
		file_path = dlg.GetPathName();
		cout << file_path << endl;
	}
	else
		return false;

	CImageData img_data(file_path.c_str());

	CProcessing proc;
	double *hist = proc.CalcHistogram(img_data, 'g');

	ofstream hist_out;
	string out_file_name = "D:\\Documents\\Study\\2017SRTP\\DATA\\hist.txt";
	hist_out.open(out_file_name);
	for(int i=0;i<256;i++)
		hist_out << i << ',' << hist[i] <<'\n';
	hist_out.close(); 

	return true;
}

bool CalculateMeanValeu()
{
	char* filter = "JPEG (*.jpg)|*.jpg|PNG (*.png)|*.png|BMP (*.bmp)|*.bmp|TIFF (*.tif)|*.tif||";
	string default_path = "D:\\Documents\\Study\\2017SRTP\\DATA\\tmp_output\\";
	string file_path;
	AfxSetResourceHandle(GetModuleHandle(NULL));
	CFileDialog dlg(TRUE, NULL, default_path.c_str(), OFN_HIDEREADONLY, filter);
	cout << "Select File: ";
	if(IDOK == dlg.DoModal())
	{
		file_path = dlg.GetPathName();
		cout << file_path << endl;
	}
	else
		return false;

	CImageData img_data(file_path.c_str());

	int threshold;
	int num;
	//cout << "\nPlease Enter the Threshold: ";
	//cin >> threshold;
	//cout << "\nPlease Enter the Pixels Number: ";
	//cin >> num;

	threshold = 21;
	cout << '\n' << threshold;

	CProcessing proc;
	int mean_value = proc.CalcMeanValue(img_data, threshold);

	cout << "\nThe Mean Value Is: " << mean_value;

	return true;
}
