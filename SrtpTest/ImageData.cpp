#include "ImageData.h"


CImageData::CImageData(void)
	:m_band_num(0),m_raster_width(0),m_raster_height(0),
	m_file_path(""),m_file_name(""),m_file_type("")
{
	MetaData empty_metadata={
		{0,1,0,0,0,1},
		0,
		0,
		0,
		NULL,
		NULL,
		"Unknow",
		"Unknow",
		"Unknow",
		"Unknow",
		"Unknow",
		"Unkonw"
	};
	m_metadata=empty_metadata;
	m_gdal_dataset = NULL;
	m_org_data = NULL;
}

CImageData::CImageData(const char* path)
	:m_band_num(0),m_raster_width(0),m_raster_height(0),
	m_file_path(""),m_file_name(""),m_file_type("")
{
	MetaData empty_metadata={
		{0,1,0,0,0,1},
		0,
		0,
		0,
		NULL,
		NULL,
		"Unknow",
		"Unknow",
		"Unknow",
		"Unknow",
		"Unknow",
		"Unkonw"
	};
	m_metadata=empty_metadata;
	m_gdal_dataset = NULL;
	m_org_data = NULL;
	InitData(path);
}

CImageData::CImageData(const CImageData &another)
{
	m_file_path=another.m_file_path;
	m_file_name=another.m_file_name;
	m_file_type=another.m_file_type;
	int n = m_band_num=another.m_band_num;
	int w = m_raster_width=another.m_raster_width;
	int h = m_raster_height=another.m_raster_height;
	m_gdal_dataset=another.m_gdal_dataset;
	//m_org_data=new double[n*w*h];
	m_org_data=(byte*)CPLMalloc(sizeof(byte)*n*w*h);
	memset(m_org_data,0.0,sizeof(byte)*n*w*h);
	memcpy(m_org_data,another.m_org_data,sizeof(byte)*n*w*h);
	m_metadata=another.m_metadata;
	m_metadata.value_range=new double*[m_band_num];
	for(int i=0;i<m_band_num;i++)
	{
		m_metadata.value_range[i]=new double[2];
		memcpy(m_metadata.value_range[i],another.m_metadata.value_range[i],sizeof(double)*2);
	}
}

CImageData::~CImageData(void)
{
	if(m_org_data != NULL)
	{
		//delete []m_org_data;
		CPLFree(m_org_data);
		m_org_data = NULL;
	}
	if(m_metadata.value_range!=NULL)
	{
		for(int i=0;i<m_band_num;i++)
		{
			delete m_metadata.value_range[i];
		}
		delete[] m_metadata.value_range;
	}

}

bool CImageData::InitData(const char * path)
{
	m_file_path = path;
	char dirve[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	if(_splitpath_s(path,dirve,dir,fname,ext))
	{
		//MessageBox(NULL,_T("Â·¾¶´íÎó£¡"),_T("´íÎó"),MB_ICONSTOP | MB_OK);
		//TRACE0("Path Not Available\n");
		return false;
	}
	m_file_name = fname;
	m_file_type = ext;

	if(!ReadToDataset())
	{
		//TRACE0("haven't read to dataset\n");
		return false;
	}

	if(!FetchBasicInformation())
	{
		//TRACE0("haven't fetched basic information\n");
		return false;
	}

	if(!ExtractImageData())
	{
	//	TRACE0("haven't extracted image data\n");
		return false;
	}

	GDALClose((GDALDatasetH)m_gdal_dataset);
	return true;
}

bool CImageData::ReadToDataset()
{
	if(""==m_file_path)
	{
		//TRACE0("file_path matter\n");
		return false;
	}
	if(m_gdal_dataset != NULL)
	{
		m_gdal_dataset = NULL;
	}
	GDALAllRegister();
	m_gdal_dataset = (GDALDataset*)GDALOpen(m_file_path.c_str(),GA_ReadOnly);

	return true;
}

bool CImageData::FetchBasicInformation()
{
	if(NULL==m_gdal_dataset)
	{
		return false;
	}
	m_band_num = m_gdal_dataset->GetRasterCount();
	m_raster_width = m_gdal_dataset->GetRasterXSize();
	m_raster_height = m_gdal_dataset->GetRasterYSize();

	//if(m_gdal_dataset->GetGeoTransform(m_metadata.geo_transform)!=CE_Failure)
	//{	
	//	m_metadata.resolution = atof(m_gdal_dataset->GetMetadataItem("TIFFTAG_RESOLUTIONUNIT",""));
	//	m_metadata.y_resolution = atof(m_gdal_dataset->GetMetadataItem("TIFFTAG_YRESOLUTION",""));
	//	m_metadata.x_resolution = atof(m_gdal_dataset->GetMetadataItem("TIFFTAG_XRESOLUTION",""));
	//	m_metadata.acquired_time = m_gdal_dataset->GetMetadataItem("TIFFTAG_DATETIME","");
	//}

	string proj_ref = m_gdal_dataset->GetProjectionRef();
	//projection hasn't been defined
	//sensor type hasn't been defined


	if(m_metadata.value_range!=NULL)
	{
		for(int i=0;i<m_band_num;i++)
		{
			delete m_metadata.value_range[i];
		}
		delete[] m_metadata.value_range;
	}
	m_metadata.value_range = new double*[m_band_num];
	GDALRasterBand* poBand = NULL;
	for(int k=0;k<m_band_num;k++)
	{
		m_metadata.value_range[k]=new double[2];
		poBand = m_gdal_dataset->GetRasterBand(k+1);
		m_metadata.value_range[k][0] = poBand->GetMinimum();
		m_metadata.value_range[k][1] = poBand->GetMaximum();
		switch(poBand->GetRasterDataType())
		{
		case GDT_Byte:m_metadata.data_type = "Byte";break;
		case GDT_UInt16:m_metadata.data_type = "UInt16";break;
		case GDT_UInt32:m_metadata.data_type = "UInt32";break;
		case GDT_Int16:m_metadata.data_type = "Int16";break;
		case GDT_Int32:m_metadata.data_type = "Int32";break;
		case GDT_Float32:m_metadata.data_type = "Float32";break;
		case GDT_Float64:m_metadata.data_type = "Float64";break;
		default:m_metadata.data_type = "Unknown";break;
		}
	}

	//didn't think about the case that no geo and proj reference
	return true;
}

bool CImageData::ExtractImageData()
{
	if(NULL==m_gdal_dataset)
	{
		return false;
	}

	int n = m_band_num;
	int w = m_raster_width;
	int h = m_raster_height;

	if(m_org_data != NULL)
	{
		//delete []m_org_data;
		CPLFree(m_org_data);
		m_org_data = NULL;
	}
	m_org_data = (byte*)CPLMalloc(sizeof(byte)*n*w*h);
	//m_org_data = new double[n*w*h];
	byte* band_data = new byte[w*h];
	GDALRasterBand* po_band = NULL;
	for(int k=0;k<n;k++)
	{
		po_band = m_gdal_dataset->GetRasterBand(k+1);
		CPLErr err = po_band->RasterIO(GF_Read,0,0,w,h,band_data,w,h,GDT_Byte,0,0);
		if(CE_Failure == err)
		{
			//TRACE0("Image Read Failed\n");
			return false;	
		}
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				m_org_data[k*w*h+i*w+j]=band_data[i*w+j];
			}
		}
	}
	delete[] band_data;

	return true;
}
//
//bool CImageData::ExtractImageData(int off_x, int off_y, int width, int height, int band_num)
//{
//	if(NULL==m_gdal_dataset)
//	{
//		return false;
//	}
//	int n;
//	int w = m_raster_width = width;
//	int h = m_raster_height = height;
//	if(band_num=0)
//		n = m_band_num;
//	else
//		n = band_num;
//
//	if(m_org_data != NULL)
//	{
//		//delete []m_org_data;
//		CPLFree(m_org_data);
//		m_org_data = NULL;
//	}
//	m_org_data = (byte*)CPLMalloc(sizeof(byte)*n*w*h);
//	//m_org_data = new double[n*w*h];
//	byte* band_data = new byte[w*h];
//	GDALRasterBand* po_band = NULL;
//	for(int k=0;k<n;k++)
//	{
//		po_band = m_gdal_dataset->GetRasterBand(k+1);
//		CPLErr err = po_band->RasterIO(GF_Read,off_x,off_y,w,h,band_data,w,h,GDT_Byte,0,0);
//		if(CE_Failure == err)
//		{
//			//TRACE0("Image Read Failed\n");
//			return false;	
//		}
//		for(int i=0;i<h;i++)
//		{
//			for(int j=0;j<w;j++)
//			{
//				m_org_data[k*w*h+i*w+j]=band_data[i*w+j];
//			}
//		}
//	}
//	delete[] band_data;
//
//	return true;
//}

bool CImageData::Create(const byte* org_data,  int width, int height, int band_num, MetaData metadata)
{
	int n = m_band_num = band_num;
	int w = m_raster_width = width;
	int h = m_raster_height = height;
	if(m_org_data!=NULL)
	{
		CPLFree(m_org_data);
		m_org_data = NULL;
	}
	m_org_data=(byte*)CPLMalloc(sizeof(byte)*n*w*h);
	memset(m_org_data,0.0,sizeof(byte)*n*w*h);
	memcpy(m_org_data,org_data,sizeof(byte)*n*w*h);
	
	if(m_metadata.value_range!=NULL)
	{
		for(int i=0;i<m_band_num;i++)
		{
			delete m_metadata.value_range[i];
		}
		delete[] m_metadata.value_range;
	}
	m_metadata = metadata;
	m_metadata.value_range=new double*[n];
	for(int i=0;i<n;i++)
	{
		m_metadata.value_range[i]=new double[2];
		memset(m_metadata.value_range[i],0.0,sizeof(double)*2);
		memcpy(m_metadata.value_range[i],metadata.value_range[i],sizeof(double)*2);
	}

	return true;
}

bool CImageData::SaveToFile(const char* path)
{
	if(NULL==m_org_data)
	{
		//TRACE0("No Avaliable Data\n");
		return false;
	}

	m_file_path = path;
	char dirve[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	if(_splitpath_s(path,dirve,dir,fname,ext))
	{
		////MessageBox(NULL,_T("Â·¾¶´íÎó£¡"),_T("´íÎó"),MB_ICONSTOP | MB_OK);
		//TRACE0("Path Not Available\n");
		return false;
	}
	m_file_name = fname;
	m_file_type = ext;

	//string tmp_dirve = dirve;
	//string tmp_dir = dir;
	//string tmp_file_path = tmp_dirve+tmp_dir+m_file_name+"_t.tif";

	char* G_type = NULL;
	if(0 == strcmp(ext,".bmp"))
		G_type="BMP";
	else if(0 == strcmp(ext,".jpg"))
		G_type="JPEG";
	else if(0 == strcmp(ext,".png"))
		G_type="PNG";
	else if(0 == strcmp(ext,".tif"))
		G_type="GTiff";
	else if(0 == strcmp(ext,".gif"))
		G_type="GIF";
	else
	{
		m_file_path+=".tif";
		m_file_type=".tif";
		G_type="GTiff";
	}

	int n = m_band_num;
	int w = m_raster_width;
	int h = m_raster_height;

	GDALDriver *po_driver = GetGDALDriverManager()->GetDriverByName("MEM");
	GDALDataset *output_dataset = po_driver->Create("",w,h,n,GDT_Byte,NULL);

	GDALRasterBand* po_band = NULL;
	int line_count = w*n;
	byte* band_data = new byte[w*h];
	for(int k=0;k<n;k++)
	{
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				band_data[i*w+j]=m_org_data[k*w*h+i*w+j];
			}
		}
		po_band = output_dataset->GetRasterBand(n-k);
		CPLErr err = po_band->RasterIO(GF_Write,0,0,w,h,band_data,w,h,GDT_Byte,n,line_count);
		if(CE_Failure == err)
		{
			//TRACE0("Save Failed\n");
			return false;
		}
	}
	GDALDriver* p_dst_driver = (GDALDriver*)GDALGetDriverByName(G_type);
	if(NULL == p_dst_driver)
		return false;
	p_dst_driver->CreateCopy(m_file_path.c_str(),output_dataset,FALSE,NULL,NULL,NULL);

	delete[] band_data;
	band_data = NULL;
	return true;
}

bool CImageData::Colour(byte value)
{
	if(NULL == m_org_data)
	{
		return false;
	}

	int n = m_band_num;
	int w = m_raster_width;
	int h = m_raster_height;

	for(int k=0;k<n;k++)
	{
		for(int i=0;i<h;i++)
		{
			for(int j=0;j<w;j++)
			{
				if(m_org_data[k*w*h+i*w+j] != 0)
					m_org_data[k*w*h+i*w+j] == value;
			}
		}
	}

	return true;
}

void CImageData::SetPath(const char* path)
{
	m_file_path = path;
}

void CImageData::SetOriginData(const byte* data)
{
	int h = m_raster_height;
	int w = m_raster_width;
	for(int k=0;k<m_band_num;k++)
	{
		for(int i=0;i<m_raster_height;i++)
		{
			for(int j=0;j<m_raster_width;j++)
			{
				m_org_data[k*w*h+i*w+j]=data[k*w*h+i*w+j];
			}
		}
	}
}
