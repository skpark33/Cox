#pragma once

#include <windef.h>
#include <vector>


#define REAL_OBJ_NO			6		// 8 frames
#define LOCAL_REGION_NO		300		// 300 frames(10 secs)
#define BB_TEMP_DIFF		150		// 12000/80 = 150/1
#define SCALE_IMG			32
#define CENT_DIFF			1000	// celcius 6.67 degrees
#define CENT_PIX_COOR		16		// less then 32 pixel
#define UPDATE_OUT_NO		150		// Î∏îÎûôÎ∞îÎîîÍ∞Ä ?¥Îèô?òÍ≥† Î™?Ï∞æÏùÑ Í≤ΩÏö∞ ?§Ïùå ?§ÌÖù?¥Îèô???ÑÌïú Í∞?àò
#define UPDATE_KEEP_NO		30		// Î∏îÎûôÎ∞îÎîîÍ∞Ä ?¥Îèô?òÍ≥† ?ôÏùºÏ¢åÌëú ?†Ï? Í∞?àò
#define NUC_TEMP_CORRECT	20		// 22.5 = 0.15µµ, 20 = 0.13µµ

using namespace std;


class CBBTempCorrection
{
private:
	float		gain;			// gain 2-Point correction
	float		offset;			// offset 2-Point correction	

	int			err_arr[60];	// array param for moving average
	int			t1_moving_mean;	// before moving mean
	int			t1_err;			// t1 offset data
	int			t2_err;			// t2 offset data 
	int			t3_err;			// t3 offset data
	int			t1_bActive;		// t1 blackbody area status	

	int			det_obj_cx;		// black-body center-x detected in the whole image size
	int			det_obj_cy;		// black-body center-y detected in the whole image size
	int			det_obj_dx;		// black-body width detected in the whole image size
	int			det_obj_dy;		// black-body height detected in the whole image size
	int			update_obj_cx;
	int			update_obj_cy;
	int			update_obj_cnt;
	int			update_out_cnt;
	bool		bUpdate_obj;
	int			det_obj_data_arr_idx;
	int			det_obj_data_arr[60];
	int			det_obj_data;
	bool		bDetObj;		// whether or not black-body area is detected in the global region.	
	int			NoDetObj;
	bool		bLocalDetObj;	// whether or not black-body area is detected in the local region.
	int			NoLocalDetObj;
	int			NoWaitDet;

	int			success_obj_cx;
	int			success_obj_cy;
	int			success_obj_dx;
	int			success_obj_dy;

	int			*obj_lbp;		// created lbp pattern after detection
	int			lbp_idx;		// lbp code number

	RECT		bb_temp_rect;

	int			bb_size_1m;				// black-body pixel size at the 1m.
	int			bb_size_2m;				// black-body pixel size at the 2m.
	int			bb_size_3m;				// black-body pixel size at the 3m.
	int			bb_size_4m;				// black-body pixel size at the 4m.
	float		target_temp_1m;			// target temperature at the 4m when black-body distance is 1m.
	float		target_temp_2m;			// target temperature at the 4m when black-body distance is 2m.
	float		target_temp_3m;			// target temperature at the 4m when black-body distance is 3m.
	float		target_temp_4m;			// target temperature at the 4m when black-body distance is 4m.

	int			bb_size_ulimit;			// black-body distance correction param
	int			bb_size_mlimit;			// black-body distance correction param
	float		bb_size_corr_ugain;		// black-body distance correction param
	float		bb_size_corr_uoffset;	// black-body distance correction param
	float		bb_size_corr_mgain;		// black-body distance correction param
	float		bb_size_corr_moffset;	// black-body distance correction param
	float		bb_size_corr_lgain;		// black-body distance correction param
	float		bb_size_corr_loffset;	// black-body distance correction param

	int			old_detect_l;
	int			old_detect_r;
	int			old_detect_t;
	int			old_detect_b;
	int			false_detect_cnt;
	DWORD		m_dwTick;

public:
	CBBTempCorrection();
	~CBBTempCorrection();

public:
	/**
	* @brief		Temperature compensation with one black-body
	* @param[in]	tempLv		: temperature level
	* @param[in]	width		: image width
	* @param[in]	height		: image height
	* @param[in]	inspect_rect: inspection region ( ex: left(0), right(384), top(0), bottom(288)
	* @param[in]	min_temp	: reference minimum temperature (ex. normal temperature : -30)
	* @param[in]	max_temp	: reference maximum temperature (ex. normal temperature : 130)
	* @param[in]	base_temp	: black-body temperature
	* @param[in]	bb_draw_rect: black-body ROI coordinate
	* @param[out]	correctLv	: corrected temperature level
	* @rtn			bool		: true(detect), false(not detect)
	*/
	bool CompensateTempError(unsigned short *tempLv, int width, int height, RECT inspect_rect, float min_temp, float max_temp, float base_temp, RECT &bb_draw_rect, int* offset, bool bManual);

	/**
	* @brief		Reset Black-body position
	* @param[in]	flag		: true(reset)
	*/
	void ResetBBPos() { bDetObj = false; bLocalDetObj = false; NoDetObj = 0; NoLocalDetObj = 0; }

	RECT GetBBTempRect() { return bb_temp_rect; }

	/**
	* @brief		Set Black-body Distance Temperature Correction Parameters
	* @param[in]	bb_1m		: black-body pixel width at the 1m.
	* @param[in]	bb_2m		: black-body pixel width at the 2m.
	* @param[in]	bb_3m		: black-body pixel width at the 3m.
	* @param[in]	bb_4m		: black-body pixel width at the 4m.
	* @param[in]	target_1m	: target(Black-body 36C) temperature at the 4m when black-body distance is 1m.
	* @param[in]	target_2m	: target(Black-body 36C) temperature at the 4m when black-body distance is 2m.
	* @param[in]	target_3m	: target(Black-body 36C) temperature at the 4m when black-body distance is 3m.
	* @param[in]	target_4m	: target(Black-body 36C) temperature at the 4m when black-body distance is 4m.
	**/
	void SetBBDistCorrectionParam(int bb_1m, int bb_2m, int bb_3m, int bb_4m, float target_1m, float target_2m, float target_3m, float target_4m);

private:
	/**
	* @brief		Temperature compensation with one black-body
	* @param[inout]	img			: temperature level
	* @param[in]	width		: image width
	* @param[in]	height		: image height
	* @param[in]	rect_sx		: start-x of black-body roi
	* @param[in]	rect_sy		: start-y of black-body roi
	* @param[in]	rect_ex		: end-x of black-body roi
	* @param[in]	rect_ey		: end-y of black-body roi
	* @param[in]	min_temp	: reference minimum temperature (ex. normal temperature : -30)
	* @param[in]	max_temp	: reference maximum temperature (ex. normal temperature : 130)
	* @param[in]	base_temp	: black-body temperature
	* @rtn			difference from base_temp level to current level in the roi.
	*/
	int CompensateTempError(unsigned short *img, int width, int height, int rect_sx, int rect_sy, int rect_ex, int rect_ey, float min_temp, float max_temp, float base_temp);

	/**
	* @brief		Temperature compensation with one black-body
	* @param[in]	tempLv		: temperature level
	* @param[in]	width		: image width
	* @param[in]	height		: image height
	* @param[in]	rect		: black-body ROI coordinate
	* @param[in]	min_temp	: reference minimum temperature (ex. normal temperature : -30)
	* @param[in]	max_temp	: reference maximum temperature (ex. normal temperature : 130)
	* @param[in]	base_temp	: black-body temperature
	* @param[out]	correctLv	: corrected temperature level
	* @rtn			difference from base_temp level to current level in the roi.
	*/
	int CompensateTempError(unsigned short *tempLv, int width, int height, RECT rect, float min_temp, float max_temp, float base_temp, bool &bActive);

	/**
	* @brief		Compute moving average
	* @param[in]	value		: offset value
	* @param[in]	bActive		: Applied moving average when bActive is true.
	* @rtn			int			: moving average result
	*/
	int GetMovingMean(int value, bool bActive);

	int GetMovingAverage(int *arr, int &arr_idx, int total_no, int value, bool bReal);

	/**
	* @brief		image resizing
	* @param[in]	*inImg		: pointer over input image
	* @param[in]	w			: input image width
	* @param[in]	h			: input image height
	* @param[out]	*outImg		: pointer over the output image
	* @param[in]	w2			: resize width
	* @param[in]	h2			: resize height
	*/
	void resizeBilinearGray(unsigned short *inImg, int w, int h, unsigned short *outImg, int w2, int h2);

	/**
	* @brief		Black-body Position Detection
	* @param[in]	*ir_image	: pointer over the ir temperature data
	* @param[in]	dx			: image width
	* @raram[in]	dy			: image height
	* @param[out]	obj_cx		: detected bb center-x
	* @param[out]	obj_cy		: detected bb center-y
	* @param[out]	obj_dx		: detected bb width
	* @param[out]	obj_dy		: detected bb_height
	* @param[out]	match_rate	: matching rate
	* @rtn			bool		: true(detect), false(not detect)
	*/
	bool SearchBBPos(unsigned short *ir_image, int dx, int dy, int sx, int ex, int sy, int ey, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate);

	void SearchBBPosManual(unsigned short *ir_image, int dx, int dy, int sx, int sy, int ex, int ey, int &obj_cx, int &obj_cy);

	void GetReservaPosition(unsigned short* ir_image, int dx, int dy, vector<POINT> &point, int pos_x, int pos_y, int pos_depth);

	/**
	* @brief		Get Object's center value
	* @param[in]	*ir_image	: pointer over the ir temperature data
	* @param[in]	dx			: image width
	* @raram[in]	dy			: image height
	* @param[out]	cx			: detected bb center-x
	* @param[out]	cy			: detected bb center-y
	* @rtn			int			: object center's ir data
	*/
	int GetObjValue(unsigned short *ir_image, int dx, int dy, int cx, int cy);


	/**
	* @brief		Black-body Position Detection in the bb_pos vector
	* @param[in]	*ir_image	: pointer over the ir temperature data
	* @param[in]	dx			: image width
	* @raram[in]	dy			: image height
	* @param[in]	bb_pos		: reserved position
	* @param[out]	obj_cx		: detected bb center-x
	* @param[out]	obj_cy		: detected bb center-y
	* @param[out]	obj_dx		: detected bb width
	* @param[out]	obj_dy		: detected bb_height
	* @param[out]	match_rate	: matching rate
	* @rtn			bool		: true(detect), false(not detect)
	*/
	bool DetectBBPosition(unsigned short *ir_image, int dx, int dy, vector<POINT> &bb_pos, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate);

	/**
	* @brief		Created LBP(local binary pattern) after detection BB
	* @param[in]	*ir_image	: pointer over the ir temperature data
	* @param[in]	dx			: image width
	* @raram[in]	dy			: image height
	* @param[in]	obj_cx		: detected bb center-x
	* @param[in]	obj_cy		: detected bb center-y
	* @param[in]	obj_dx		: detected bb width
	* @param[in]	obj_dy		: detected bb_height
	*/
	void CreateLBP(unsigned short *ir_image, int dx, int dy, int obj_cx, int obj_cy, int obj_dx, int obj_dy);

	/**
	* @brief		Black-body Position Detection in the bb_pos vector
	* @param[in]	*ir_image	: pointer over the ir temperature data
	* @param[in]	dx			: image width
	* @raram[in]	dy			: image height
	* @param[in]	in_half_w	: inspection area's half length (obj_cx - in_half_w, obj_cx + in_half_w, obj_cy - in_half_w, obj_cy + in_half_w)
	* @param[out]	obj_cx		: detected bb center-x
	* @param[out]	obj_cy		: detected bb center-y
	* @param[out]	obj_dx		: detected bb width
	* @param[out]	obj_dy		: detected bb_height
	* @param[out]	match_rate	: matching rate
	* @rtn			bool		: true(detect), false(not detect)
	*/
	bool MatchingLBP(unsigned short *ir_image, int dx, int dy, int in_half_w, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate);

	/**
	* @brief		Compute gain and offset for 2-point correction
	* @param[in]	*dpcLv		: pointer of Camera DPC level data
	* @param[in]	width		: image width
	* @param[in]	height		: image height
	* @param[in]	rect1		: RECT coor structure of low-level blackbody
	* @param[in]	rect2		: RECT coor structure of high-level blackbldy
	* @param[in]	bb1			: first black-body temperature (unit : celcius)
	* @param[in]	bb2			: second black-body temperature (unit : celcius)
	* @rtn			error code
	*/
	const int Compute2PtCorrect(unsigned short *dpcLv, int width, int height, RECT low_rect, RECT high_rect, float low_bb, float high_bb);

	/**
	* @brief		Apply 2-point correction
	* @param[in]	*dpcLv			: pointer of Camera DPC level data
	* @param[in]	width			: image width
	* @param[in]	height			: image height
	* @param[in]	min_temp		: reference minimum temperature (ex. normal temperature : -30)
	* @param[in]	max_temp		: reference maximum temperature (ex. normal temperature : 130)
	* @param[out]	*correctLv		: pointer over the corrected temperature level
	* @rtn			error code
	*/
	const int Apply2PtCorrect(unsigned short *dpcLv, int width, int height, float min_temp, float max_temp, unsigned short *correctLv);

	/**
	* @brief		Apply 2-point correction
	* @param[in]	*img			: *img : Camera DPC level data
	* @param[in]	width			: image width
	* @param[in]	height			: image height
	* @param[in]	low_temp_limit	: low temperature limit
	* @param[in]	high_temp_limit : high temperature limit
	* @param[out]	temp			: celcius temperature
	* @rtn			error code
	*/
	const int Apply2PtCorrect(unsigned short *dpcLv, int width, int height, float min_temp, float max_temp, float *temp);
};

