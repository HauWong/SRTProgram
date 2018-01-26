#pragma once
#pragma comment(lib,"..\\lib\\gdal_i.lib")
#include "gdal_priv.h"

typedef unsigned char byte;

using std::string;

typedef struct ImageBasicInformation
{
	double		geo_transform[6];
	double		resolution;
	double		x_resolution;
	double		y_resolution;
	double*		centr_wave_lth;
	double**	value_range;
	string		acquired_time;
	string		geo_coord_sys;
	string		pro_coord_sys;
	string		data_type;		//���ݴ洢����
	string		store_method;	//���ݴ洢��ʽ
	string		sensor;
}MetaData;


class CImageData
{
public:
	CImageData(void);
	CImageData(const char*);
	CImageData(const CImageData &another);
	~CImageData(void);
private:
	GDALDataset* m_gdal_dataset;

	byte*		m_org_data;		//ԭʼ����
	int			m_band_num;
	int			m_raster_width;
	int			m_raster_height;
	string		m_file_path;
	string		m_file_name;
	string		m_file_type;	//�ļ�����

	MetaData	m_metadata;
public:
	bool InitData(const char* file_path);
	bool ReadToDataset();
	bool FetchBasicInformation();
	bool ExtractImageData();
  //bool ExtractImageData(int off_x, int off_y, int width, int height, int band_num=0);
	bool Create(const byte* org_data,  int width, int height, int band_num, MetaData metadata); 
	bool SaveToFile(const char* file_path);
	bool Colour(byte value = 255);	//��ɫ

	void				SetPath(const char* file_path);
	void				SetOriginData(const byte* data);
	inline byte*		GetOriginData()const{return m_org_data;}
	inline const int	GetWidth()const{return m_raster_width;}
	inline int			GetHeight()const{return m_raster_height;}
	inline int			GetBandCount()const{return m_band_num;}
	inline string		GetPath()const{return m_file_path;}
	inline string		GetName()const{return m_file_name;}
	inline string		GetFileType()const{return m_file_type;}
	inline MetaData		GetMetadata()const{return m_metadata;}
};

