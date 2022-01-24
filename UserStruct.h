#pragma once
#include <vector>

// \brief
// ��Ī��ǥ ����ü (���󿡼� ��´�)
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:36:03		�����ۼ�
typedef struct _eeprom_matching_info_
{
	CPoint			m_ptFever[DEF_MAX_MATCHING_POINT];
	CPoint			m_ptReal[DEF_MAX_MATCHING_POINT];

} EEPROM_MATCHING_INFO;


// \brief
// �ǻ�ī�޶��� ���� �ػ� �� Dataformat
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:35:46		�����ۼ�
typedef struct _visable_support_resolution_
{
	VIDEOINFOHEADER					m_stHeader;
	BOOL							m_bSupport;
	DWORD							m_dwCompression;

	_visable_support_resolution_()
		: m_bSupport(FALSE)
		, m_dwCompression(0)
	{
	}

	_visable_support_resolution_(_In_ VIDEOINFOHEADER*	a_pVideoHeader)
		: m_bSupport(FALSE)
		, m_dwCompression(0)
	{
		::memcpy_s(&m_stHeader,
				   sizeof(VIDEOINFOHEADER),
				   a_pVideoHeader,
				   sizeof(VIDEOINFOHEADER));

		m_dwCompression = m_stHeader.bmiHeader.biCompression;

		DWORD dwFormat = MAKEFOURCC('Y','U','Y','2');
		m_bSupport = m_dwCompression == dwFormat;
	}

	CString		getCompression2String()
	{
		CString strFourCC;
		DWORD dwMJPG = MAKEFOURCC('M', 'J', 'P', 'G');
		DWORD dwYUY2 = MAKEFOURCC('Y', 'U', 'Y', '2');
		DWORD dwH263 = MAKEFOURCC('H', '2', '6', '3');
		DWORD dwH264 = MAKEFOURCC('H', '2', '6', '4');
		DWORD dwH265 = MAKEFOURCC('H', '2', '6', '5');
		switch( m_dwCompression ) {
			case MAKEFOURCC('M', 'J', 'P', 'G'):
				strFourCC.Format(_T("MJPG"));
				break;

			case MAKEFOURCC('Y', 'U', 'Y', '2'):
				strFourCC.Format(_T("YUY2"));
				break;

			case MAKEFOURCC('H', '2', '6', '3'):
				strFourCC.Format(_T("H263"));
				break;

			case MAKEFOURCC('H', '2', '6', '4'):
				strFourCC.Format(_T("H264"));
				break;

			case MAKEFOURCC('H', '2', '6', '5'):
				strFourCC.Format(_T("H265"));
				break;

			default:
				strFourCC.Format(_T("Unknown (%d)"), m_dwCompression);
				break;
		}

		return strFourCC;
	}

} VCAM_RES;


// \brief
// PC�� ����� �ǻ�ī�޶� ����
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:32:18		�����ۼ�
typedef struct _visable_camera_info_
{
	int										m_nDevIdx;
	CString									m_strDeviceName;
	CString									m_strDescription;
	CString									m_strDevicePath;
	std::vector<std::shared_ptr<VCAM_RES>>	m_vVideoInfoHDList;

	_visable_camera_info_()
		: m_nDevIdx(-1)
	{
	}

	_visable_camera_info_(_In_ int				a_nDevIdx,
						  _In_ const CString&	a_strName,
						  _In_ const CString&	a_strDesc,
						  _In_ const CString&	a_strPath)
		: m_nDevIdx(a_nDevIdx)
		, m_strDeviceName(a_strName)
		, m_strDescription(a_strDesc)
		, m_strDevicePath(a_strPath)
	{

	}
} VCAM_INFO;


// \brief
// ������ �������̽� ���ø� ����ü
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:31:45		�����ۼ�
template<typename T>
struct _cox_processing_buffer_
{
	T*						data;

	uint32_t				width;
	uint32_t				height;
	uint32_t				size;
	uint32_t				byte;

	_cox_processing_buffer_() = default;

	~_cox_processing_buffer_()
	{
		DeleteAry(data);
	}

	BOOL		Init(_In_ uint32_t		a_nWidth,
					 _In_ uint32_t		a_nHeight)
	{
		width	= a_nWidth;
		height	= a_nHeight;
		size	= width * height;
		byte	= (uint32_t)(sizeof(T) * size);

		data	= new T[size];
		::memset(data, 0x00, sizeof(T) * size);

		return TRUE;
	}


	BOOL		GetData(_Out_ T*		a_pBuff)
	{
		auto error = ::memcpy_s(a_pBuff, byte, data, byte);
		return error == 0 ? TRUE : FALSE;
	}


	BOOL		SetData(_In_ T*			a_pBuff)
	{
		auto error = ::memcpy_s(data, byte, a_pBuff, byte);
		return error == 0 ? TRUE : FALSE;
	}

	BOOL		SetData(_In_ T*			a_pBuff,
						_In_ int		w,
						_In_ int		h)
	{
		auto error = ::memcpy_s(data, w * h * sizeof(T), a_pBuff, w * h * sizeof(T));
		return error == 0  TRUE : FALSE;
	}

	void		Reset()
	{
		::memset(data, 0x00, sizeof(T) * size);
	}
};

template<typename T> using CPBUFF = _cox_processing_buffer_<T>;


// \brief
// �� Ư¡ ����ü
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-01-21 20:15:48		�����ۼ�

#include <FaceMeBaseDef.h>  //skpark in your area

typedef struct _face_feature_info_
{
	uint32_t				sizeOfStructure;
	int32_t				featureType;
	int32_t				featureSubType;
	uint32_t				featureDataSize;
	float					featureData[2048];


	static _face_feature_info_* Create()
	{
		return new _face_feature_info_();
	}
	//skpark in your aree : to init
	_face_feature_info_()
	{
		uint32_t				sizeOfStructure = 0;
		uint32_t				featureDataSize = 0;
		float					featureData[2048];
		for (int i = 0; i<2048; i++)  featureData[i] = 0;
	}
	BOOL IsNotNull()
	{
		return (sizeOfStructure > 0 && featureDataSize > 0);
	}
	void Copy(const _face_feature_info_& that)  //skpark in your area :  copy �Լ� �߰�
	{
		sizeOfStructure = that.sizeOfStructure;
		featureType = that.featureSubType;
		featureSubType = that.featureSubType;
		featureDataSize = that.featureDataSize;
		memcpy(featureData, that.featureData, sizeof(float)* 2048);
	}
	void CopyTo(FaceMeSDK::FR_FaceFeature& that)  //skpark in your area :  copyTo �Լ� �߰�
	{
		that.sizeOfStructure = sizeOfStructure;
		that.featureType = featureSubType;
		that.featureSubType = featureSubType;
		that.featureData.size = featureDataSize;
		memcpy(that.featureData.data, featureData, sizeof(float)* 2048);
	}
	//skpark in your area end

} FACE_FEATURE_INFO;


// \brief
// �󱼰��� ����ü
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:31:33		�����ۼ�
typedef struct _face_info_
{
	// �ǻ� ����ǥ�� �� �ڽ� 
	CRect				facebox;

	// �ǻ� ��ũ����ǥ�� �����ϸ��� �󱼹ڽ�
	CRect				visable_facebox;

	// ���� ��ũ����ǥ�� �����ϸ��� �󱼹ڽ�
	CRect				thermal_facebox;
	
	// �ǻ� ����ǥ�迡�� ������ ���帶ũ ��ġ
	CPoint				facelandmark[5];

	// �ǻ� ��ũ����ǥ�� �����ϸ��� ���帶ũ ��ġ
	CPoint				visable_facelandmark[5];

	// ���� ��ũ����ǥ�� �����ϸ��� ���帶ũ ��ġ
	CPoint				thermal_facelandmark[5];

	// �� ����
	float				yaw;
	float				pitch;
	float				roll;

	// ���� Ȯ��
	float				confidence;

	// ����ũ ���� ����
	MASK_STATE			mask_state;

	// �� �µ�
	float				face_temp;

	// �� Ư¡
	FACE_FEATURE_INFO	facefeature;

} FACE_INFO;



// \brief
// �ǻ�� ���� ���� ��� ������ ��� ����ü
// �󱼰��� �������� ����ִ�.
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:30:05		�����ۼ�
typedef struct _camera_frame_info_
{
	CPBUFF<uint8_t>				visable_image;
	CPBUFF<uint8_t>				thermal_image;
	CPBUFF<float>				thermal_temp;
	CPBUFF<uint8_t>				thermal_gray;

	uint						face_count;
	FACE_INFO					face_detect[30];

	_camera_frame_info_()
	{
		visable_image.Init(DEF_MAX_VISABLE_CAM_W, DEF_MAX_VISABLE_CAM_H * 4);
		thermal_image.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H * 4);
		thermal_temp.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H);
		thermal_gray.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H);
	}

	~_camera_frame_info_() {}


	void	resetFace()
	{
		for( int i = 0 ; i < 30 ; ++i ) {
			::memset(&face_detect[i], 0x00, sizeof(FACE_INFO));
		}
		face_count = 0;
	}

	void	resetData()
	{
		visable_image.Reset();
		thermal_image.Reset();
		thermal_temp.Reset();
		thermal_gray.Reset();
		resetFace();
	}

} CAMF_INFO;


// \brief
// �ǻ� �̹��� ����ü
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:30:47		�����ۼ�
typedef struct _visable_frame_info_
{
	CPBUFF<uint32_t>			image;

	_visable_frame_info_()
	{
		image.Init(DEF_MAX_VISABLE_CAM_W, DEF_MAX_VISABLE_CAM_H);
	}

	~_visable_frame_info_()
	{
	}

	void		reset()
	{
		image.Reset();
	}
} VF_INFO;


// \brief
// ���� �̹��� ����ü
//
// \code
// - ���� �ڵ带 �־��ּ���.
// \encode
// \warning
// \sa
// \author	������
// \date		2021-12-28 11:31:01		�����ۼ�
typedef struct _thermal_frame_info_
{
	uint8_t						idx;
	CPBUFF<uint32_t>			image;		// �ȷ�Ʈ ����� �̹���
	CPBUFF<uint8_t>				gray;		// �׷��� �̹���
	CPBUFF<float>				temp;		// �µ� ������
	CPBUFF<uint16_t>			raw;		// raw ������

	_thermal_frame_info_()
	{
		image.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H);
		gray.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H);
		temp.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H);
		raw.Init(DEF_MAX_THERMAL_CAM_W, DEF_MAX_THERMAL_CAM_H);
	}

	~_thermal_frame_info_()
	{
	}

	void		reset()
	{
		image.Reset();
		gray.Reset();
		temp.Reset();
		raw.Reset();
	}

} TF_INFO;