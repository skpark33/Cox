#include "stdafx.h"
#include "CBBTempCorrection.h"
#include <math.h>

CBBTempCorrection::CBBTempCorrection()
	: t1_err(0), t2_err(0), t1_bActive(false), t1_moving_mean(0), NoDetObj(0), bLocalDetObj(false), bDetObj(false), NoLocalDetObj(0), NoWaitDet(0)
{
	memset(err_arr, 0, sizeof(int) * 60);
	obj_lbp = NULL;
	bb_temp_rect = { 0, 0, 0, 0 };
	det_obj_data = 0;
	
	det_obj_data_arr_idx = 0;
	memset(det_obj_data_arr, 0, sizeof(int) * 60);
	
	bb_size_ulimit = 17;
	bb_size_mlimit = 11;
	bb_size_corr_ugain = 0.038125f;
	bb_size_corr_uoffset = -1.508125f;
	bb_size_corr_mgain = 0.1066667f;
	bb_size_corr_moffset = -2.67333f;
	bb_size_corr_lgain = 0.26667f;
	bb_size_corr_loffset = -4.43333f;
}

CBBTempCorrection::~CBBTempCorrection()
{
	if( obj_lbp != NULL )
		delete[] obj_lbp;
}

/**
* @brief		Set Black-body Distance Temperature Correction Parameters
* @param[in]	bb_1m		: black-body pixel width at the 1m.
* @param[in]	bb_2m		: black-body pixel width at the 2m.
* @param[in]	bb_3m		: black-body pixel width at the 3m.
* @param[in]	bb_4m		: black-body pixel width at the 4m.
* @param[in]	target_1m	: target temperature at the 4m when black-body distance is 1m.
* @param[in]	target_2m	: target temperature at the 4m when black-body distance is 2m.
* @param[in]	target_3m	: target temperature at the 4m when black-body distance is 3m.
* @param[in]	target_4m	: target temperature at the 4m when black-body distance is 4m.
**/
void CBBTempCorrection::SetBBDistCorrectionParam(int bb_1m, int bb_2m, int bb_3m, int bb_4m, float target_1m, float target_2m, float target_3m, float target_4m)
{
	bb_size_ulimit = bb_2m;
	bb_size_mlimit = bb_3m;
	bb_size_corr_ugain = (float)((target_2m - target_1m) / (bb_1m - bb_2m));
	bb_size_corr_uoffset = (float)((36 - target_1m) - bb_size_corr_ugain * bb_1m);
	bb_size_corr_mgain = (float)((target_3m - target_2m) / (bb_2m - bb_3m));
	bb_size_corr_moffset = (float)((36 - target_2m) - bb_size_corr_mgain * bb_2m);
	bb_size_corr_lgain = (float)((target_4m - target_3m) / (bb_3m - bb_4m));
	bb_size_corr_loffset = (float)((36 - target_3m) - bb_size_corr_lgain * bb_3m);
}

/**
* @brief		Temperature compensation with one black-body
* @param[in]	tempLv		: temperature level
* @param[in]	width		: image width
* @param[in]	height		: image height
* @param[in]	inspect_rect: inspection region ( ex: left(0), right(384), top(0), bottom(288)
* @param[in]	min_temp	: reference minimum temperature (ex. normal temperature : -30)
* @param[in]	max_temp	: reference maximum temperature (ex. normal temperature : 130)
* @param[in]	base_temp	: black-body temperature
* @param[in]	bb_rect		: black-body ROI coordinate
* @param[out]	correctLv	: corrected temperature level
* @rtn			bool		: true(detect), false(not detect)
*/
bool CBBTempCorrection::CompensateTempError(unsigned short *tempLv, int width, int height, RECT inspect_rect, float min_temp, float max_temp, float base_temp, RECT &bb_draw_rect, int* offset, bool bManual)
{
	int sx = inspect_rect.left;
	int ex = inspect_rect.right;
	int sy = inspect_rect.top;
	int ey = inspect_rect.bottom;

	int obj_half_dx = 0;
	int obj_half_dy = 0;

	int obj_cx = 0;
	int obj_cy = 0;
	int obj_dx = 0;
	int obj_dy = 0;
	float rate = 0;
	float bb_temp = base_temp;

	bool detect = false;

	if( bManual ) {
		SearchBBPosManual(tempLv, width, height, sx, sy, ex, ey, obj_cx, obj_cy);
		bb_draw_rect = inspect_rect;
		detect = true;
	} else {
		detect = SearchBBPos(tempLv, width, height, sx, ex, sy, ey, obj_cx, obj_cy, obj_dx, obj_dy, rate);

		if( detect ) {
			obj_half_dx = (int)(obj_dx * 0.5f + 0.5f);
			obj_half_dy = (int)(obj_dy * 0.5f + 0.5f);
			bb_draw_rect.left = obj_cx - obj_half_dx + 1;
			bb_draw_rect.top = obj_cy - obj_half_dy + 1;
			bb_draw_rect.right = obj_cx + obj_half_dx + 1;
			bb_draw_rect.bottom = obj_cy + obj_half_dy + 1;

			old_detect_t = bb_draw_rect.top;
			old_detect_b = bb_draw_rect.bottom;
			old_detect_l = bb_draw_rect.left;
			old_detect_r = bb_draw_rect.right;
			false_detect_cnt = 0;
		}
	}

	if( detect ) {
		bb_temp_rect.left = obj_cx - 1;
		bb_temp_rect.right = obj_cx + 1;
		bb_temp_rect.top = obj_cy - 1;
		bb_temp_rect.bottom = obj_cy + 1;

		int grid_dx = (int)(width / 3.0f);
		int grid_dy = (int)(height / 3.0f);
		//float temp_offset[9] = { 0.5f, 0.1f, -0.5f, 0.3f, 0, -0.5f, 0.1f, -0.1f, -0.8f };
		float temp_offset[9] = { 0.f };
		int grid_x = (int)floor((obj_cx * 3.0f) / width);
		int grid_y = (int)floor((obj_cy * 3.0f) / height);
		bb_temp += temp_offset[grid_y * 3 + grid_x];

		float offset_2nd = 0;

		if( !bManual ) {
			/***************************************
			obj_dx   temp
			30      37.7
			17      38.2
			9      39
			8      40.1
			****************************************/
			if( obj_dx >= bb_size_ulimit )
				offset_2nd = bb_size_corr_ugain * obj_dx + bb_size_corr_uoffset;
			else if( obj_dx >= bb_size_mlimit )
				offset_2nd = bb_size_corr_mgain * obj_dx + bb_size_corr_moffset;
			else
				offset_2nd = bb_size_corr_lgain * obj_dx + bb_size_corr_loffset;

			bb_temp += offset_2nd;
		}
	}

	*offset = CompensateTempError(tempLv, width, height, bb_temp_rect, min_temp, max_temp, bb_temp, detect);

	if( !detect ) {
		if( false_detect_cnt < 2 ) {
			false_detect_cnt++;

			bb_draw_rect.top = old_detect_t;
			bb_draw_rect.bottom = old_detect_b;
			bb_draw_rect.left = old_detect_l;
			bb_draw_rect.right = old_detect_r;

			if( !bUpdate_obj )
				detect = true;
		}
	}

	return detect;
}


/**
* @brief		Temperature compensation with one black-body
* @param[inout]	img : temperature level
* @param[in]	width : image width
* @param[in]	height : image height
* @param[in]	rect_sx : roi start-x
* @param[in]	rect_sy : roi start-y
* @param[in]	rect_ex : roi end-x
* @param[in]	rect_ey : roi end-y
* @param[in]	min_temp : reference minimum temperature
* @param[in]	max_temp : reference maximum temperature
* @param[in]	base_temp : black-body temperature (unit : celcius)
* @rtn			difference from base_temp level to current level in the roi.
*/
int CBBTempCorrection::CompensateTempError(unsigned short *img, int width, int height, int rect_sx, int rect_sy, int rect_ex, int rect_ey, float min_temp, float max_temp, float base_temp)
{
	int sum = 0;
	int roi_mean = 0;
	int temp_err = 0;

	unsigned short baseLv = (unsigned short)((base_temp - min_temp) * (12000.0 / (max_temp - min_temp)) + 2000);

	for( int y = rect_sy; y < rect_ey; y++ )
		for( int x = rect_sx; x < rect_ex; x++ )
			sum += img[y*width + x];

	// roi average
	roi_mean = (int)(sum / (float)((rect_ex - rect_sx)*(rect_ey - rect_sy)));
	temp_err = baseLv - roi_mean;

	// update temperature level
	for( int k = 0; k < (width*height); k++ )
		img[k] = img[k] + temp_err;

	return temp_err;
}

/**
* @brief		Temperature compensation with one black-body
* @param[in]	*tempLv		: pointer of the temperature level
* @param[in]	width		: image width
* @param[in]	height		: image height
* @param[in]	rect		: black-body ROI coordinate
* @param[in]	min_temp	: reference minimum temperature (ex. normal temperature : -30)
* @param[in]	max_temp	: reference maximum temperature (ex. normal temperature : 130)
* @param[in]	base_temp	: black-body temperature (unit : celcius)
* @param[out]	*correctLv	: pointer of the corrected temperature level
* @rtn			difference from base_temp level to current level in the roi.
*/
int CBBTempCorrection::CompensateTempError(unsigned short *tempLv, int width, int height, RECT rect, float min_temp, float max_temp, float base_temp, bool &bActive)
{
	int sum = 0;
	int roi_mean = 0;
	int temp_err = 0;

	// convert from celcius temperature to level temperature
	unsigned short baseLv = (unsigned short)((base_temp - min_temp) * (12000.0 / (max_temp - min_temp)) + 2000);

	for( int y = rect.top; y <= rect.bottom; y++ )
		for( int x = rect.left; x <= rect.right; x++ )
			sum += tempLv[y*width + x];

	// roi average
	roi_mean = (int)(sum / (float)((rect.right - rect.left + 1)*(rect.bottom - rect.top + 1)));

	temp_err = baseLv - roi_mean;

	int mean_err = GetMovingMean(temp_err, bActive);

	// update temperature level
	for( int k = 0; k < (width*height); k++ )
		tempLv[k] = __max(2000, __min(14000, tempLv[k] + mean_err));

	return mean_err;
}

/**
* @brief		Bilinear resize grayscale image.
*				pixels is an array of size w * h.
*				Target dimension is w2 * h2.
*				w2 * h2 cannot be zero.
*
* @param[in]	*inImg : pixels Image pixels.
* @param[in[]	w : Image width.
* @param[in]	h : Image height.
* @param[out]	*outImg : New array with size w2 * h2.
* @param[in]	w2 : New width.
* @param[in]	h2 : New height.
*/
void CBBTempCorrection::resizeBilinearGray(unsigned short *inImg, int w, int h, unsigned short *outImg, int w2, int h2)
{
	int A, B, C, D, x, y, index, gray;
	float x_ratio = ((float)(w - 1)) / (w2 - 1);
	float y_ratio = ((float)(h - 1)) / (h2 - 1);

	float x_diff, y_diff;

	for( int i = 0; i < h2; i++ ) {
		for( int j = 0; j < w2; j++ ) {
			x = (int)(x_ratio * j);
			y = (int)(y_ratio * i);
			x_diff = (x_ratio * j) - x;
			y_diff = (y_ratio * i) - y;
			index = y * w + x;

			// range is 0 to 255 thus bitwise AND with 0xff
			A = inImg[index] & 0xffff;

			if( (x + 1) >= w )
				B = inImg[index] & 0xffff;
			else
				B = inImg[index + 1] & 0xffff;

			if( (y + 1) >= h )
				C = inImg[index] & 0xffff;
			else
				C = inImg[index + w] & 0xffff;

			if( (y + 1) >= h || (x + 1) >= w )
				D = inImg[index] & 0xffff;
			else
				D = inImg[index + w + 1] & 0xffff;

			// Y = A(1-w)(1-h) + B(w)(1-h) + C(h)(1-w) + Dwh
			gray = (int)(
				A*(1 - x_diff)*(1 - y_diff) + B * (x_diff)*(1 - y_diff) +
				C * (y_diff)*(1 - x_diff) + D * (x_diff*y_diff)
				);

			outImg[i*w2 + j] = (unsigned short)min(16383, max(0, gray));
		}
	}
}

/*************************************************************************************************
bool CBBTempCorrection::SearchBBPos(unsigned short *ir_image, int dx, int dy, int sx, int ex, int sy, int ey, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate)
{
	bool bLDetObj = false;

	if( bDetObj ) {
		int old_obj_cx = det_obj_cx;
		int old_obj_cy = det_obj_cy;

		NoLocalDetObj = 0;
		// local area detection
		bLDetObj = MatchingLBP(ir_image, dx, dy, 10, obj_cx, obj_cy, obj_dx, obj_dy, match_rate);

		int obj_value = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);

		if( bLDetObj && abs(det_obj_data - obj_value) < BB_TEMP_DIFF )
			det_obj_data = obj_value;
		else
			bLDetObj = false;

		if( bLDetObj ) {
			bLocalDetObj = true;
			return true;
		} else {
			bDetObj = false;
			NoDetObj = 0;
			return false;
		}
	} else {
		if( bLocalDetObj ) {
			bLDetObj = MatchingLBP(ir_image, dx, dy, 20, obj_cx, obj_cy, obj_dx, obj_dy, match_rate);

			int obj_value = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);

			if( bLDetObj && abs(det_obj_data - obj_value) < BB_TEMP_DIFF )
				det_obj_data = obj_value;
			else
				bLDetObj = false;

			if( !bLDetObj )
				NoLocalDetObj++;

			if( NoLocalDetObj > 10 ) {
				NoLocalDetObj = 0;
				bLocalDetObj = false;
			}

			if( bLDetObj ) {
				bDetObj = true;
				bLocalDetObj = false;

				int half_dx = (int)(obj_dx * 0.5f) + 10;
				int half_dy = (int)(obj_dy * 0.5f) + 10;
				int sum_x = 0;
				int sum_y = 0;
				int max_x = 0;
				int min_x = 1000;
				int max_y = 0;
				int min_y = 1000;
				int sum_idx = 0;
				unsigned short ct_pix = ir_image[obj_cy * dx + obj_cx];

				if( (obj_cx - half_dx) < 0 || (obj_cx + half_dx) >= dx || (obj_cy - half_dy) < 0 || (obj_cy + half_dy) >= dy ) {
					bDetObj = false;
					return false;
				}

				for( int y = obj_cy - half_dy; y < obj_cy + half_dy; y++ ) {
					for( int x = obj_cx - half_dx; x < obj_cx + half_dx; x++ ) {
						if( abs(ir_image[y*dx + x] - ct_pix) < 500 ) {
							sum_x += x;
							sum_y += y;
							sum_idx++;

							if( max_x < x ) max_x = x;
							if( min_x > x ) min_x = x;
							if( max_y < y ) max_y = y;
							if( min_y > y ) min_y = y;
						}
					}
				}

				obj_cx = (int)(sum_x / sum_idx + 0.5f);
				obj_cy = (int)(sum_y / sum_idx + 0.5f);
				int w = (int)((max_x - min_x)*1.6f);// 6;
				int h = (int)((max_y - min_y)*1.6f);// 6;
				obj_dx = (w > h) ? w : h;
				obj_dy = obj_dx;

				det_obj_cx = obj_cx;
				det_obj_cy = obj_cy;
				det_obj_dx = obj_dx;
				det_obj_dy = obj_dy;



				// check range
				half_dx = (int)(obj_dx * 0.5f);
				half_dy = (int)(obj_dy * 0.5f);
				if( (obj_cx - half_dx) < 0 || (obj_cx + half_dx) >= dx || (obj_cy - half_dy) < 0 || (obj_cy + half_dy) >= dy ) {
					bDetObj = false;
					return false;
				} else {
					CreateLBP(ir_image, dx, dy, obj_cx, obj_cy, obj_dx, obj_dy);
					det_obj_data = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);
					return true;
				}
			} else {
				bDetObj = false;
				NoDetObj = 0;
				return false;
			}
		}
	}

	// searched the whole image area

	unsigned short *tmp_img = new unsigned short[(dx)*(dy)];

	// computed 4-neighbor average
	for( int y = 0; y < dy - 1; y++ ) {
		for( int x = 0; x < dx - 1; x++ ) {
			tmp_img[y*dx + x] = (unsigned short)((ir_image[y*dx + x] + ir_image[y*dx + (x + 1)] + ir_image[(y + 1)*dx + x] + ir_image[(y + 1)*dx + (x + 1)]) / 4.0);
		}
	}

	vector<POINT> bb_pos_1;
	vector<POINT> bb_pos_2;
	vector<POINT> bb_pos_3;
	vector<POINT> bb_pos_4;
	vector<POINT> bb_pos_5;
	vector<POINT> bb_pos_6;
	vector<POINT> bb_pos_7;
	vector<POINT> bb_pos_8;

	for( int y = sy; y < ey - 32; y++ ) {
		for( int x = sx; x < ex - 32; x++ ) {
			// max pixel : 32
			int m0 = tmp_img[(y + 16)*dx + (x + 16)];

			int m1 = tmp_img[(y + 8)*dx + (x + 16)];
			int m2 = tmp_img[(y + 16)*dx + (x + 8)];
			int m3 = tmp_img[(y + 24)*dx + (x + 16)];
			int m4 = tmp_img[(y + 16)*dx + (x + 24)];

			int m5 = tmp_img[(y + 10)*dx + (x + 16)];
			int m6 = tmp_img[(y + 16)*dx + (x + 10)];
			int m7 = tmp_img[(y + 22)*dx + (x + 16)];
			int m8 = tmp_img[(y + 16)*dx + (x + 22)];

			int m9 = tmp_img[(y + 12)*dx + (x + 16)];
			int m10 = tmp_img[(y + 16)*dx + (x + 12)];
			int m11 = tmp_img[(y + 20)*dx + (x + 16)];
			int m12 = tmp_img[(y + 16)*dx + (x + 20)];

			//////////////////////////////////////
			int m13 = tmp_img[(y + 6)*dx + (x + 16)];
			int m14 = tmp_img[(y + 16)*dx + (x + 6)];
			int m15 = tmp_img[(y + 26)*dx + (x + 16)];
			int m16 = tmp_img[(y + 16)*dx + (x + 26)];

			int m17 = tmp_img[(y + 4)*dx + (x + 16)];
			int m18 = tmp_img[(y + 16)*dx + (x + 4)];
			int m19 = tmp_img[(y + 28)*dx + (x + 16)];
			int m20 = tmp_img[(y + 16)*dx + (x + 28)];

			///////////////////////////////////
			int m21 = tmp_img[(y + 2)*dx + (x + 16)];
			int m22 = tmp_img[(y + 16)*dx + (x + 2)];
			int m23 = tmp_img[(y + 28)*dx + (x + 16)];
			int m24 = tmp_img[(y + 16)*dx + (x + 28)];

			int m25 = tmp_img[(y + 0)*dx + (x + 16)];
			int m26 = tmp_img[(y + 16)*dx + (x + 0)];
			int m27 = tmp_img[(y + 30)*dx + (x + 16)];
			int m28 = tmp_img[(y + 16)*dx + (x + 30)];

			int diff_01 = m0 - m1;
			int diff_02 = m0 - m2;
			int diff_03 = m0 - m3;
			int diff_04 = m0 - m4;
			int diff_11 = m0 - m5;
			int diff_12 = m0 - m6;
			int diff_13 = m0 - m7;
			int diff_14 = m0 - m8;
			int diff_21 = m0 - m9;
			int diff_22 = m0 - m10;
			int diff_23 = m0 - m11;
			int diff_24 = m0 - m12;

			int diff_31 = m0 - m13;
			int diff_32 = m0 - m14;
			int diff_33 = m0 - m15;
			int diff_34 = m0 - m16;

			int diff_41 = m0 - m17;
			int diff_42 = m0 - m18;
			int diff_43 = m0 - m19;
			int diff_44 = m0 - m20;

			int diff_51 = m0 - m21;
			int diff_52 = m0 - m22;
			int diff_53 = m0 - m23;
			int diff_54 = m0 - m24;

			int diff_61 = m0 - m25;
			int diff_62 = m0 - m26;
			int diff_63 = m0 - m27;
			int diff_64 = m0 - m28;

			int thresh = CENT_DIFF;
			if( diff_01 > thresh && diff_02 > thresh && diff_03 > thresh && diff_04 > thresh ) {
				if( diff_11 > thresh && diff_12 > thresh && diff_13 > thresh && diff_14 > thresh ) {
					if( diff_21 > thresh && diff_22 > thresh && diff_23 > thresh && diff_24 > thresh )
						GetReservaPosition(ir_image, dx, dy, bb_pos_1, x, y, 4);
					else
						GetReservaPosition(ir_image, dx, dy, bb_pos_2, x, y, 6);
				} else
					GetReservaPosition(ir_image, dx, dy, bb_pos_3, x, y, 8);
			} else if( diff_21 > thresh && diff_22 > thresh && diff_23 > thresh && diff_24 > thresh )
				GetReservaPosition(ir_image, dx, dy, bb_pos_4, x, y, 8);
			else if( diff_31 > thresh && diff_32 > thresh && diff_33 > thresh && diff_34 > thresh )
				GetReservaPosition(ir_image, dx, dy, bb_pos_5, x, y, 10);
			else if( diff_41 > thresh && diff_42 > thresh && diff_43 > thresh && diff_44 > thresh )
				GetReservaPosition(ir_image, dx, dy, bb_pos_6, x, y, 12);
			else if( diff_51 > thresh && diff_52 > thresh && diff_53 > thresh && diff_54 > thresh )
				GetReservaPosition(ir_image, dx, dy, bb_pos_7, x, y, 14);
			else if( diff_61 > thresh && diff_62 > thresh && diff_63 > thresh && diff_64 > thresh )
				GetReservaPosition(ir_image, dx, dy, bb_pos_8, x, y, 16);
		}
	}

	delete[] tmp_img;

	bool bObj = false;
	int local_obj_cx = 0;
	int local_obj_cy = 0;
	int local_obj_dx = 0;
	int local_obj_dy = 0;
	float local_match_rate = 0;

	bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_1, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_2, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_3, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_4, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_5, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_6, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_7, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( !bObj )
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_8, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if( bObj ) {
		if( NoDetObj >= REAL_OBJ_NO )
			NoDetObj = REAL_OBJ_NO;
		else
			NoDetObj++;
	} else
		NoDetObj = 0;

	bDetObj = NoDetObj >= REAL_OBJ_NO ? true : false;

	if( bDetObj ) {
		int half_dx = (int)(local_obj_dx * 0.5f) + 10;
		int half_dy = (int)(local_obj_dy * 0.5f) + 10;
		int sum_x = 0;
		int sum_y = 0;
		int max_x = 0;
		int min_x = 1000;
		int max_y = 0;
		int min_y = 1000;
		int sum_idx = 0;
		unsigned short ct_pix = ir_image[local_obj_cy * dx + local_obj_cx];

		for( int y = local_obj_cy - half_dy; y < local_obj_cy + half_dy; y++ ) {
			for( int x = local_obj_cx - half_dx; x < local_obj_cx + half_dx; x++ ) {
				if( abs(ir_image[y*dx + x] - ct_pix) < 500 ) {
					sum_x += x;
					sum_y += y;
					sum_idx++;

					if( max_x < x ) max_x = x;
					if( min_x > x ) min_x = x;
					if( max_y < y ) max_y = y;
					if( min_y > y ) min_y = y;
				}
			}
		}

		obj_cx = (int)(sum_x / sum_idx + 0.5f);
		obj_cy = (int)(sum_y / sum_idx + 0.5f);
		int w = (int)((max_x - min_x)*1.6f);// 6;
		int h = (int)((max_y - min_y)*1.6f);// 6;
		obj_dx = (w > h) ? w : h;
		obj_dy = obj_dx;
		match_rate = local_match_rate;


		det_obj_cx = obj_cx;
		det_obj_cy = obj_cy;
		det_obj_dx = obj_dx;
		det_obj_dy = obj_dy;

		// check range
		half_dx = (int)(obj_dx * 0.5f);
		half_dy = (int)(obj_dy * 0.5f);
		if( (obj_cx - half_dx) < 0 || (obj_cx + half_dx) >= dx || (obj_cy - half_dy) < 0 || (obj_cy + half_dy) >= dy )
			bDetObj = false;
		else {
			CreateLBP(ir_image, dx, dy, obj_cx, obj_cy, obj_dx, obj_dy);
			det_obj_data = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);
		}
	}

	return bDetObj;
}
*************************************************************************************************/

bool CBBTempCorrection::SearchBBPos(unsigned short *ir_image, int dx, int dy, int sx, int ex, int sy, int ey, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate)
{
	bool bLDetObj = false;

	/*******************************************************************************/
	if (bDetObj)
	{
		NoLocalDetObj = 0;
		NoWaitDet = 0;

		int old_obj_cx = det_obj_cx;
		int old_obj_cy = det_obj_cy;

		// local area detection
		bLDetObj = MatchingLBP(ir_image, dx, dy, 10, obj_cx, obj_cy, obj_dx, obj_dy, match_rate);
				
		int obj_value = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);

		if (bLDetObj && abs(det_obj_data - obj_value) < BB_TEMP_DIFF)
		{
			det_obj_data = GetMovingAverage(det_obj_data_arr, det_obj_data_arr_idx, 60, obj_value, true);

			success_obj_cx = obj_cx;
			success_obj_cy = obj_cy;
			success_obj_dx = obj_dx;
			success_obj_dy = obj_dy;

			bUpdate_obj = false;
		}
		else if (!bLDetObj && abs(det_obj_data - obj_value) < BB_TEMP_DIFF)
		{
			bUpdate_obj = true;
			update_out_cnt = 0;
		}
		else
		{
			bLDetObj = false;

			det_obj_cx = success_obj_cx;
			det_obj_cy = success_obj_cy;
		}

		if (bLDetObj)
		{
			bLocalDetObj = true;
			int obj_sx = obj_cx - (int)(obj_dx * 0.5f);
			int obj_ex = obj_cx + (int)(obj_dx * 0.5f);
			int obj_sy = obj_cy - (int)(obj_dy * 0.5f);
			int obj_ey = obj_cy + (int)(obj_dy * 0.5f);

			if( obj_sx < sx || obj_ex > ex || obj_sy < sy || obj_ey > ey )
				bDetObj = false;
			else
				bDetObj = true;

			return bDetObj;
			//return true;
		}
		else
		{
			bDetObj = false;
			NoDetObj = 0;
			return false;
		}
	}
	else if (bUpdate_obj)
	{
		// local area detection
		bLDetObj = MatchingLBP(ir_image, dx, dy, 10, obj_cx, obj_cy, obj_dx, obj_dy, match_rate);

		int obj_value = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);

		if (abs(det_obj_data - obj_value) < BB_TEMP_DIFF)
		{
			if (bLDetObj)
			{
				det_obj_data = GetMovingAverage(det_obj_data_arr, det_obj_data_arr_idx, 60, obj_value, true);
				bUpdate_obj = false;

				int obj_sx = obj_cx - (int)(obj_dx * 0.5f);
				int obj_ex = obj_cx + (int)(obj_dx * 0.5f);
				int obj_sy = obj_cy - (int)(obj_dy * 0.5f);
				int obj_ey = obj_cy + (int)(obj_dy * 0.5f);

				if( obj_sx < sx || obj_ex > ex || obj_sy < sy || obj_ey > ey )
					bDetObj = false;
				else
					bDetObj = true;

				return bDetObj;
				//return true;
			}
			else
			{
				if(update_out_cnt > UPDATE_OUT_NO)
					bUpdate_obj = false;

				return false;
			}
		}
		else
		{
			if (update_out_cnt++ > UPDATE_OUT_NO)
				bUpdate_obj = false;
			
			return false;
		}
	}
	else
	{
		if (bLocalDetObj)
		{
			 obj_cx = success_obj_cx;
			 obj_cy = success_obj_cy;
			 obj_dx = success_obj_dx;
			 obj_dy = success_obj_dy;

			bLDetObj = MatchingLBP(ir_image, dx, dy, 20, obj_cx, obj_cy, obj_dx, obj_dy, match_rate);
			
			int obj_value = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);
			
			int diff = abs(det_obj_data - obj_value);
			CString msg = _T("");
			msg.Format(_T("\n [%d %d %d %d] - %d : %d - %f"), obj_cx, obj_cy, obj_dx, obj_dy, det_obj_data, obj_value, match_rate);
			//AfxOutputDebugString(msg);

			if (bLDetObj && abs(det_obj_data - obj_value) < BB_TEMP_DIFF)
			{
				det_obj_data = obj_value;
			}
			else
			{
				bLDetObj = false;

				det_obj_cx = success_obj_cx;
				det_obj_cy = success_obj_cy;
			}

			if (!bLDetObj)
				NoLocalDetObj++;

			if (NoLocalDetObj > LOCAL_REGION_NO)
			{
				NoLocalDetObj = 0;
				bLocalDetObj = false;
			}

			if (bLDetObj)
			{
				//bDetObj = true;
				bLocalDetObj = false;

				int obj_sx = obj_cx - (int)(obj_dx * 0.5f);
				int obj_ex = obj_cx + (int)(obj_dx * 0.5f);
				int obj_sy = obj_cy - (int)(obj_dy * 0.5f);
				int obj_ey = obj_cy + (int)(obj_dy * 0.5f);

				if( obj_sx < sx || obj_ex > ex || obj_sy < sy || obj_ey > ey )
					bDetObj = false;
				else
					bDetObj = true;

				if( bDetObj ) {
					det_obj_cx = obj_cx;
					det_obj_cy = obj_cy;
					det_obj_dx = obj_dx;
					det_obj_dy = obj_dy;

					//AfxOutputDebugString(_T("CreateLBP"));

					CreateLBP(ir_image, dx, dy, obj_cx, obj_cy, obj_dx, obj_dy);

					det_obj_data = GetMovingAverage(det_obj_data_arr, det_obj_data_arr_idx, 60, obj_value, true);
				}
				return bDetObj;
				//return true;
			}
			else
			{
				bDetObj = false;
				NoDetObj = 0;
				return false;
			}
		}
	}
	/*******************************************************************************/

	// searched the whole image area

	unsigned short *tmp_img = new unsigned short[(dx)*(dy)];

	// computed 4-neighbor average
	for (int y = 0; y < dy - 1; y++)
	{
		for (int x = 0; x < dx - 1; x++)
		{
			tmp_img[y*dx + x] = (unsigned short)((ir_image[y*dx + x] + ir_image[y*dx + (x + 1)] + ir_image[(y + 1)*dx + x] + ir_image[(y + 1)*dx + (x + 1)]) / 4.0);
		}
	}

	vector<POINT> bb_pos_1;
	vector<POINT> bb_pos_2;
	vector<POINT> bb_pos_3;
	vector<POINT> bb_pos_4;
	vector<POINT> bb_pos_5;
	vector<POINT> bb_pos_6;
	vector<POINT> bb_pos_7;
	vector<POINT> bb_pos_8;

	int inner_sx = (sx - 16) <= 0 ? 0 : (sx - 16);
	int inner_ex = (ex + 16) > dx - 1 ? dx - 1 : (ex + 16);
	int inner_sy = (sy - 16) <= 0 ? 0 : (sy - 16);
	int inner_ey = (ey + 16) > dy - 1 ? dy - 1 : (ey + 16);

	for (int y = inner_sy; y < inner_ey - 32; y++)
	{
		for (int x = inner_sx; x < inner_ex - 32; x++)
		{
			// max pixel : 32
			int m0 = tmp_img[(y + 16)*dx + (x + 16)];

			int m1 = tmp_img[(y + 8)*dx + (x + 16)];
			int m2 = tmp_img[(y + 16)*dx + (x + 8)];
			int m3 = tmp_img[(y + 24)*dx + (x + 16)];
			int m4 = tmp_img[(y + 16)*dx + (x + 24)];

			int m5 = tmp_img[(y + 10)*dx + (x + 16)];
			int m6 = tmp_img[(y + 16)*dx + (x + 10)];
			int m7 = tmp_img[(y + 22)*dx + (x + 16)];
			int m8 = tmp_img[(y + 16)*dx + (x + 22)];

			int m9 = tmp_img[(y + 12)*dx + (x + 16)];
			int m10 = tmp_img[(y + 16)*dx + (x + 12)];
			int m11 = tmp_img[(y + 20)*dx + (x + 16)];
			int m12 = tmp_img[(y + 16)*dx + (x + 20)];

			//////////////////////////////////////
			int m13 = tmp_img[(y + 6)*dx + (x + 16)];
			int m14 = tmp_img[(y + 16)*dx + (x + 6)];
			int m15 = tmp_img[(y + 26)*dx + (x + 16)];
			int m16 = tmp_img[(y + 16)*dx + (x + 26)];

			int m17 = tmp_img[(y + 4)*dx + (x + 16)];
			int m18 = tmp_img[(y + 16)*dx + (x + 4)];
			int m19 = tmp_img[(y + 28)*dx + (x + 16)];
			int m20 = tmp_img[(y + 16)*dx + (x + 28)];

			///////////////////////////////////
			int m21 = tmp_img[(y + 2)*dx + (x + 16)];
			int m22 = tmp_img[(y + 16)*dx + (x + 2)];
			int m23 = tmp_img[(y + 28)*dx + (x + 16)];
			int m24 = tmp_img[(y + 16)*dx + (x + 28)];

			int m25 = tmp_img[(y + 0)*dx + (x + 16)];
			int m26 = tmp_img[(y + 16)*dx + (x + 0)];
			int m27 = tmp_img[(y + 30)*dx + (x + 16)];
			int m28 = tmp_img[(y + 16)*dx + (x + 30)];

			int diff_01 = m0 - m1;
			int diff_02 = m0 - m2;
			int diff_03 = m0 - m3;
			int diff_04 = m0 - m4;
			int diff_11 = m0 - m5;
			int diff_12 = m0 - m6;
			int diff_13 = m0 - m7;
			int diff_14 = m0 - m8;
			int diff_21 = m0 - m9;
			int diff_22 = m0 - m10;
			int diff_23 = m0 - m11;
			int diff_24 = m0 - m12;

			int diff_31 = m0 - m13;
			int diff_32 = m0 - m14;
			int diff_33 = m0 - m15;
			int diff_34 = m0 - m16;

			int diff_41 = m0 - m17;
			int diff_42 = m0 - m18;
			int diff_43 = m0 - m19;
			int diff_44 = m0 - m20;

			int diff_51 = m0 - m21;
			int diff_52 = m0 - m22;
			int diff_53 = m0 - m23;
			int diff_54 = m0 - m24;

			int diff_61 = m0 - m25;
			int diff_62 = m0 - m26;
			int diff_63 = m0 - m27;
			int diff_64 = m0 - m28;

			int thresh = CENT_DIFF;
			if (diff_01 > thresh && diff_02 > thresh && diff_03 > thresh && diff_04 > thresh)
			{
				if (diff_11 > thresh && diff_12 > thresh && diff_13 > thresh && diff_14 > thresh)
				{
					if (diff_21 > thresh && diff_22 > thresh && diff_23 > thresh && diff_24 > thresh)
						GetReservaPosition(ir_image, dx, dy, bb_pos_1, x, y, 2);
					else
						GetReservaPosition(ir_image, dx, dy, bb_pos_2, x, y, 3);
				}
				else
					GetReservaPosition(ir_image, dx, dy, bb_pos_3, x, y, 4);
			}
			else if (diff_21 > thresh && diff_22 > thresh && diff_23 > thresh && diff_24 > thresh)
				GetReservaPosition(ir_image, dx, dy, bb_pos_4, x, y, 2);
			else if (diff_31 > thresh && diff_32 > thresh && diff_33 > thresh && diff_34 > thresh)
				GetReservaPosition(ir_image, dx, dy, bb_pos_5, x, y, 5);
			else if (diff_41 > thresh && diff_42 > thresh && diff_43 > thresh && diff_44 > thresh)
				GetReservaPosition(ir_image, dx, dy, bb_pos_6, x, y, 6);
			else if (diff_51 > thresh && diff_52 > thresh && diff_53 > thresh && diff_54 > thresh)
				GetReservaPosition(ir_image, dx, dy, bb_pos_7, x, y, 6);
			else if (diff_61 > thresh && diff_62 > thresh && diff_63 > thresh && diff_64 > thresh)
				GetReservaPosition(ir_image, dx, dy, bb_pos_8, x, y, 7);
		}
	}

	delete[] tmp_img;

	bool bObj = false;
	int local_obj_cx = 0;
	int local_obj_cy = 0;
	int local_obj_dx = 0;
	int local_obj_dy = 0;
	float local_match_rate = 0;

	bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_1, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_2, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_3, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_4, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_5, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_6, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_7, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (!bObj)
		bObj = DetectBBPosition(ir_image, dx, dy, bb_pos_8, local_obj_cx, local_obj_cy, local_obj_dx, local_obj_dy, local_match_rate);

	if (bObj)
	{
		if (NoDetObj >= REAL_OBJ_NO)
			NoDetObj = REAL_OBJ_NO;
		else
			NoDetObj++;
	}
	else
		NoDetObj = 0;

	bDetObj = NoDetObj >= REAL_OBJ_NO ? true : false;

	if (bDetObj)
	{
		int half_dx = (int)(local_obj_dx * 0.5f) + 10;
		int half_dy = (int)(local_obj_dy * 0.5f) + 10;
		int sum_x = 0;
		int sum_y = 0;
		int max_x = 0;
		int min_x = 1000;
		int max_y = 0;
		int min_y = 1000;
		int sum_idx = 0;
		//unsigned short ct_pix = ir_image[local_obj_cy * dx + local_obj_cx];

		int sum = ir_image[(local_obj_cy - 1)*dx + (local_obj_cx - 1)] + ir_image[(local_obj_cy - 0)*dx + (local_obj_cx - 1)] + ir_image[(local_obj_cy + 1)*dx + (local_obj_cx - 1)] +
			ir_image[(local_obj_cy - 1)*dx + (local_obj_cx - 0)] + ir_image[(local_obj_cy - 0)*dx + (local_obj_cx + 0)] + ir_image[(local_obj_cy + 1)*dx + (local_obj_cx + 0)] +
			ir_image[(local_obj_cy - 1)*dx + (local_obj_cx + 1)] + ir_image[(local_obj_cy - 0)*dx + (local_obj_cx + 1)] + ir_image[(local_obj_cy + 1)*dx + (local_obj_cx + 1)];

		unsigned short ct_pix = (unsigned short)(sum / 9.0f);

		for (int y = local_obj_cy - half_dy; y < local_obj_cy + half_dy; y++)
		{
			for (int x = local_obj_cx - half_dx; x < local_obj_cx + half_dx; x++)
			{
				int sum =	ir_image[(y - 1)*dx + (x - 1)] + ir_image[(y - 0)*dx + (x - 1)] + ir_image[(y + 1)*dx + (x - 1)] +
							ir_image[(y - 1)*dx + (x - 0)] + ir_image[(y - 0)*dx + (x + 0)] + ir_image[(y + 1)*dx + (x + 0)] +
							ir_image[(y - 1)*dx + (x + 1)] + ir_image[(y - 0)*dx + (x + 1)] + ir_image[(y + 1)*dx + (x + 1)];

				unsigned short target_pix = (unsigned short)(sum / 9.0f);

				if (abs(target_pix - ct_pix) < 200)
				{
					sum_x += x;
					sum_y += y;
					sum_idx++;

					if (max_x < x) max_x = x;
					if (min_x > x) min_x = x;
					if (max_y < y) max_y = y;
					if (min_y > y) min_y = y;
				}
			}
		}

		obj_cx = (int)(sum_x / sum_idx + 0.5f);
		obj_cy = (int)(sum_y / sum_idx + 0.5f);

		float rate = 1.6f;

		if (sum_idx > 100) rate = 1.6f;
		else if (sum_idx > 20) rate = 1.8f;
		else rate = 2.2f;

		int w = (int)((max_x - min_x)*rate + 0.5f);// 6;
		int h = (int)((max_y - min_y)*rate + 0.5f);// 6;
		obj_dx = (w > h) ? w : h;
		obj_dy = obj_dx;
		match_rate = local_match_rate;


		det_obj_cx = obj_cx;
		det_obj_cy = obj_cy;
		det_obj_dx = obj_dx;
		det_obj_dy = obj_dy;

		CString msg = _T("");
		msg.Format(_T("D : [%d %d %d]\n"), w, h, sum_idx);
		//AfxOutputDebugString(msg);

		// check range
		half_dx = (int)(obj_dx * 0.5f);
		half_dy = (int)(obj_dy * 0.5f);
		if ((obj_cx - half_dx) < 0 || (obj_cx + half_dx) >= dx || (obj_cy - half_dy) < 0 || (obj_cy + half_dy) >= dy)
			bDetObj = false;
		else
		{
			int obj_sx = obj_cx - (int)(obj_dx * 0.5f);
			int obj_ex = obj_cx + (int)(obj_dx * 0.5f);
			int obj_sy = obj_cy - (int)(obj_dy * 0.5f);
			int obj_ey = obj_cy + (int)(obj_dy * 0.5f);

			if( obj_sx < sx || obj_ex > ex || obj_sy < sy || obj_ey > ey )
				bDetObj = false;
			else
				bDetObj = true;

			if( bDetObj ) {

				CreateLBP(ir_image, dx, dy, obj_cx, obj_cy, obj_dx, obj_dy);
				det_obj_data = GetObjValue(ir_image, dx, dy, obj_cx, obj_cy);

				det_obj_data_arr_idx = 0;
				memset(det_obj_data_arr, 0, sizeof(int) * 60);
			}
		}
	}
	   
	return bDetObj;
}


void CBBTempCorrection::GetReservaPosition(unsigned short* ir_image, int dx, int dy, vector<POINT> &point, int pos_x, int pos_y, int pos_depth)
{
	POINT pt;
	pt.x = pos_x + CENT_PIX_COOR;
	pt.y = pos_y + CENT_PIX_COOR;

	int min = 10000;
	int max = 0;

	for( int y = pt.y - pos_depth; y <= pt.y + pos_depth; y++ ) {
		for( int x = pt.x - pos_depth; x <= pt.x + pos_depth; x++ ) {
			if( y >= 0 && x >= 0 && y < dy && x < dx ) {
				if( min > ir_image[y * dx + x] ) min = ir_image[y * dx + x];
				if( max < ir_image[y * dx + x] ) max = ir_image[y * dx + x];
			}
		}
	}

	if( (max - min) < 3000 )
		point.push_back(pt);
}


bool CBBTempCorrection::DetectBBPosition(unsigned short *ir_image, int dx, int dy, vector<POINT> &bb_pos, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate)
{
	const int s_lbp[16] = { 135, 143,  15, 127,
		131, 135, 127, 126,
		129, 243, 124, 126,
		241, 240, 120, 124 };

	unsigned short *bb_img = new unsigned short[SCALE_IMG * SCALE_IMG];
	int scale_dx = SCALE_IMG;
	int scale_dy = SCALE_IMG;
	int lbp[16];
	int diff_pix[8];
	int det_x = 0;
	int det_y = 0;
	int det_dx = 0;
	int det_dy = 0;
	float det_match = 0;
	int diff_thresh = 500;

	if( !bb_pos.empty() ) {
		for( POINT pt : bb_pos ) {
			int edge_ex = pt.x;
			int edge_sx = pt.x;
			int edge_ey = pt.y;
			int edge_sy = pt.y;
			int edge_lx = 0;
			int edge_ly = 0;
			int edge_rx = 0;
			int edge_ry = 0;
			int edge_lx2 = 0;
			int edge_ly2 = 0;
			int edge_rx2 = 0;
			int edge_ry2 = 0;
			int edge_ed = 0;
			bool bEdge[8] = { 1, 1, 1, 1, 1, 1,1,1 };

			for( int k = 0; k < 20; k++ ) {
				if( bEdge[0] && (pt.x + k + 2) < dx && (ir_image[(pt.y)*dx + (pt.x + k)] - ir_image[(pt.y)*dx + (pt.x + k + 2)]) > diff_thresh ) {
					bEdge[0] = false;
					edge_ex = pt.x + k;
				}

				if( bEdge[1] && (pt.x - k - 2) >= 0 && (ir_image[(pt.y)*dx + (pt.x - k)] - ir_image[(pt.y)*dx + (pt.x - k - 2)]) > diff_thresh ) {
					bEdge[1] = false;
					edge_sx = pt.x - k;
				}

				if( bEdge[2] && (pt.y + k + 2) < dy && (ir_image[(pt.y + k)*dx + (pt.x)] - ir_image[(pt.y + k + 2)*dx + (pt.x)]) > diff_thresh ) {
					bEdge[2] = false;
					edge_ey = pt.y + k;
				}

				if( bEdge[3] && (pt.y - k - 2) >= 0 && (ir_image[(pt.y - k)*dx + (pt.x)] - ir_image[(pt.y - k - 2)*dx + (pt.x)]) > diff_thresh ) {
					bEdge[3] = false;
					edge_sy = pt.y - k;
				}

				if( bEdge[4] && (pt.y + k + 2) < dy && (pt.x + k + 2) <= dx && (ir_image[(pt.y + k)*dx + (pt.x + k)] - ir_image[(pt.y + k + 2)*dx + (pt.x + k + 2)]) > diff_thresh ) {
					bEdge[4] = false;
					edge_rx = pt.x + k;
					edge_ry = pt.y + k;
				}

				if( bEdge[5] && (pt.y - k - 2) >= 0 && (pt.x - k - 2) >= 0 && (ir_image[(pt.y - k)*dx + (pt.x - k)] - ir_image[(pt.y - k - 2)*dx + (pt.x - k - 2)]) > diff_thresh ) {
					bEdge[5] = false;
					edge_lx = pt.x + k;
					edge_ly = pt.y + k;
				}

				if( bEdge[6] && (pt.y + k + 2) < dy && (pt.x - k - 2) >= 0 && (ir_image[(pt.y + k)*dx + (pt.x - k)] - ir_image[(pt.y + k + 2)*dx + (pt.x - k - 2)]) > diff_thresh ) {
					bEdge[6] = false;
					edge_rx2 = pt.x - k;
					edge_ry2 = pt.y + k;
				}

				if( bEdge[7] && (pt.y - k - 2) >= 0 && (pt.x + k + 2) <= dx && (ir_image[(pt.y - k)*dx + (pt.x + k)] - ir_image[(pt.y - k - 2)*dx + (pt.x + k + 2)]) > diff_thresh ) {
					bEdge[7] = false;
					edge_lx2 = pt.x - k;
					edge_ly2 = pt.y + k;
				}
			}

			int dist_dx = edge_rx - edge_lx;
			int dist_dy = edge_ry - edge_ly;
			float dist = (float)sqrt(dist_dx * dist_dx + dist_dy * dist_dy);

			int dist_dx2 = edge_rx2 - edge_lx2;
			int dist_dy2 = edge_ry2 - edge_ly2;
			float dist2 = (float)sqrt(dist_dx2 * dist_dx2 + dist_dy2 * dist_dy2);

			//if (dist_dx < 1 || dist_dy < 1 || dist_dx2 < 1 || dist_dy2 < 1)
			//	continue;

			int major_axis = (edge_ex - edge_sx) > (edge_ey - edge_sy) ? (edge_ex - edge_sx) : (edge_ey - edge_sy);
			int minor_axis = (edge_ex - edge_sx) < (edge_ey - edge_sy) ? (edge_ex - edge_sx) : (edge_ey - edge_sy);

			if( edge_ex == edge_sx || edge_ey == edge_sy || dist > major_axis || dist <= 1 || dist2 > major_axis || dist2 <= 1 )
				continue;

			int new_x = (int)(edge_sx + ((edge_ex - edge_sx)*0.5f));
			int new_y = (int)(edge_sy + ((edge_ey - edge_sy)*0.5f));

			float rate_xy = major_axis / (float)minor_axis;
			if( (edge_ex - edge_sx) < 2 || (edge_ey - edge_sy) < 2 || abs(dist - dist2) > 2 || rate_xy > 2 )
				continue;

			int half_scale = (int)(SCALE_IMG*0.5f);

			if( (new_y - half_scale) < 0 || (new_y - half_scale + SCALE_IMG - 1) >= dy || (new_x - half_scale) < 0 || (new_x - half_scale) >= dx )
				continue;

			for( int m = 0; m < SCALE_IMG; m++ )
				memcpy(&bb_img[m * SCALE_IMG], &ir_image[(new_y - half_scale + m)*dx + (new_x - half_scale)], sizeof(unsigned short) * SCALE_IMG);

			scale_dx = SCALE_IMG;
			scale_dy = SCALE_IMG;
			int scale_idx = (int)((SCALE_IMG - 8)*0.5f);

			for( int k = 0; k < scale_idx ; k++ ) {
				// lbp 계산

				unsigned short *bb_scale = new unsigned short[(SCALE_IMG - k * 2)*(SCALE_IMG - k * 2)];
				resizeBilinearGray(bb_img, scale_dx, scale_dy, bb_scale, scale_dx - k * 2, scale_dy - k * 2);
				scale_dx = SCALE_IMG - k * 2;
				scale_dy = SCALE_IMG - k * 2;

				int c_dx = (int)(scale_dx * 0.5f) + 1;
				int c_dy = (int)(scale_dy * 0.5f) + 1;
				int lbp_idx = 0;

				for( int y = c_dy - 4 + 1; y < c_dy + 4; y = y + 2 ) {
					for( int x = c_dx - 4 + 1; x < c_dx + 4; x = x + 2 ) {
						diff_pix[0] = (bb_scale[y*scale_dx + x] - bb_scale[(y - 1)*scale_dx + (x - 1)]) > 0 ? 1 : 0;
						diff_pix[1] = (bb_scale[y*scale_dx + x] - bb_scale[(y - 1)*scale_dx + (x - 0)]) > 0 ? 1 : 0;
						diff_pix[2] = (bb_scale[y*scale_dx + x] - bb_scale[(y - 1)*scale_dx + (x + 1)]) > 0 ? 1 : 0;
						diff_pix[3] = (bb_scale[y*scale_dx + x] - bb_scale[(y - 0)*scale_dx + (x + 1)]) > 0 ? 1 : 0;
						diff_pix[4] = (bb_scale[y*scale_dx + x] - bb_scale[(y + 1)*scale_dx + (x + 1)]) > 0 ? 1 : 0;
						diff_pix[5] = (bb_scale[y*scale_dx + x] - bb_scale[(y + 1)*scale_dx + (x + 1)]) > 0 ? 1 : 0;
						diff_pix[6] = (bb_scale[y*scale_dx + x] - bb_scale[(y + 1)*scale_dx + (x + 1)]) > 0 ? 1 : 0;
						diff_pix[7] = (bb_scale[y*scale_dx + x] - bb_scale[(y - 0)*scale_dx + (x - 1)]) > 0 ? 1 : 0;

						lbp[lbp_idx++] = 128 * diff_pix[7] + 64 * diff_pix[6] + 32 * diff_pix[5] + 16 * diff_pix[4] + 8 * diff_pix[3] + 4 * diff_pix[2] + 2 * diff_pix[1] * diff_pix[0];
					}
				}

				float match = 0;
				float sum_mag_x_feature = 0;
				float sum_mag_2 = 0;
				float sum_feature_2 = 0;

				for( int k = 0; k < 16; k++ ) {
					match += (lbp[k] - s_lbp[k]) * (lbp[k] - s_lbp[k]);

					sum_mag_x_feature += lbp[k] * s_lbp[k];
					sum_mag_2 += lbp[k] * lbp[k];
					sum_feature_2 += s_lbp[k] * s_lbp[k];
				}

				match = sum_mag_x_feature / (float)sqrt(sum_mag_2 * sum_feature_2);

				if( match > det_match ) {
					det_match = match;
					det_x = new_x;
					det_y = new_y;
					det_dx = (int)(9 * SCALE_IMG / scale_dx);
					det_dy = (int)(9 * SCALE_IMG / scale_dy);
				}

				delete[] bb_scale;
			}
		}
	}

	delete[] bb_img;

	obj_cx = det_x;
	obj_cy = det_y;
	obj_dx = det_dx;
	obj_dy = det_dy;
	match_rate = det_match;

	return (det_match > 0.85) ? true : false;
}

void CBBTempCorrection::CreateLBP(unsigned short *ir_image, int dx, int dy, int obj_cx, int obj_cy, int obj_dx, int obj_dy)
{
	int half_dx = (int)(obj_dx * 0.5f);
	int half_dy = (int)(obj_dy * 0.5f);
	int diff_pix[8];
	int avg[9] = { 0, };

	if( obj_lbp != NULL )
		delete[] obj_lbp;

	obj_lbp = new int[(half_dx) * (half_dy)];
	lbp_idx = 0;

	for( int y = obj_cy - half_dy; y <= (obj_cy + half_dy - 5); y = y + 2 ) {
		for( int x = obj_cx - half_dx; x <= (obj_cx + half_dx - 5); x = x + 2 ) {
			avg[0] = (int)((ir_image[(y + 0)*dx + (x + 0)] + ir_image[(y + 0)*dx + (x + 1)] + ir_image[(y + 1)*dx + (x + 0)] + ir_image[(y + 1)*dx + (x + 1)])*0.25f);
			avg[1] = (int)((ir_image[(y + 0)*dx + (x + 2)] + ir_image[(y + 0)*dx + (x + 3)] + ir_image[(y + 1)*dx + (x + 2)] + ir_image[(y + 1)*dx + (x + 3)])*0.25f);
			avg[2] = (int)((ir_image[(y + 0)*dx + (x + 4)] + ir_image[(y + 0)*dx + (x + 4)] + ir_image[(y + 1)*dx + (x + 4)] + ir_image[(y + 1)*dx + (x + 5)])*0.25f);
			avg[3] = (int)((ir_image[(y + 2)*dx + (x + 0)] + ir_image[(y + 2)*dx + (x + 1)] + ir_image[(y + 3)*dx + (x + 0)] + ir_image[(y + 3)*dx + (x + 1)])*0.25f);
			avg[4] = (int)((ir_image[(y + 2)*dx + (x + 2)] + ir_image[(y + 2)*dx + (x + 3)] + ir_image[(y + 3)*dx + (x + 2)] + ir_image[(y + 3)*dx + (x + 3)])*0.25f);
			avg[5] = (int)((ir_image[(y + 2)*dx + (x + 4)] + ir_image[(y + 2)*dx + (x + 5)] + ir_image[(y + 3)*dx + (x + 4)] + ir_image[(y + 3)*dx + (x + 5)])*0.25f);
			avg[6] = (int)((ir_image[(y + 4)*dx + (x + 0)] + ir_image[(y + 4)*dx + (x + 1)] + ir_image[(y + 5)*dx + (x + 0)] + ir_image[(y + 5)*dx + (x + 1)])*0.25f);
			avg[7] = (int)((ir_image[(y + 4)*dx + (x + 2)] + ir_image[(y + 4)*dx + (x + 3)] + ir_image[(y + 5)*dx + (x + 2)] + ir_image[(y + 5)*dx + (x + 3)])*0.25f);
			avg[8] = (int)((ir_image[(y + 4)*dx + (x + 4)] + ir_image[(y + 4)*dx + (x + 5)] + ir_image[(y + 5)*dx + (x + 4)] + ir_image[(y + 5)*dx + (x + 5)])*0.25f);

			diff_pix[0] = (avg[4] - avg[0]) > 0 ? 1 : 0;
			diff_pix[1] = (avg[4] - avg[1]) > 0 ? 1 : 0;
			diff_pix[2] = (avg[4] - avg[2]) > 0 ? 1 : 0;
			diff_pix[3] = (avg[4] - avg[5]) > 0 ? 1 : 0;
			diff_pix[4] = (avg[4] - avg[8]) > 0 ? 1 : 0;
			diff_pix[5] = (avg[4] - avg[7]) > 0 ? 1 : 0;
			diff_pix[6] = (avg[4] - avg[6]) > 0 ? 1 : 0;
			diff_pix[7] = (avg[4] - avg[3]) > 0 ? 1 : 0;

			obj_lbp[lbp_idx++] = 128 * diff_pix[7] + 64 * diff_pix[6] + 32 * diff_pix[5] + 16 * diff_pix[4] + 8 * diff_pix[3] + 4 * diff_pix[2] + 2 * diff_pix[1] * diff_pix[0];
		}
	}

	//delete[] obj_lbp;
	//obj_lbp = NULL;
}

/******************************************************************************************
bool CBBTempCorrection::MatchingLBP(unsigned short *ir_image, int dx, int dy, int in_half_w, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate)
{
	//m_dwTick = ::GetTickCount();
	//DebugString(_T("1. dwTick : %d"), ::GetTickCount() - m_dwTick);
	int half_dx = (int)(det_obj_dx * 0.5f);
	int half_dy = (int)(det_obj_dy * 0.5f);
	int diff_pix[8];
	int avg[9] = { 0, };
	float origin_match = 0;
	int det_x = 0;
	int det_y = 0;
	int det_dx = 0;
	int det_dy = 0;
	int det_x2 = 0;
	int det_y2 = 0;
	int det_dx2 = 0;
	int det_dy2 = 0;
	float det_match = 0;
	bool bObj = false;

	int *lbp = new int[(half_dx) * (half_dy)];
	int idx = 0;

	int det_area_sx = (det_obj_cx - in_half_w - half_dx) > 0 ? (det_obj_cx - in_half_w) : half_dx + 1;
	int det_area_sy = (det_obj_cy - in_half_w - half_dy) > 0 ? (det_obj_cy - in_half_w) : half_dy + 1;
	int det_area_ex = (det_obj_cx + in_half_w + half_dx) < dx ? (det_obj_cx + in_half_w) : dx - half_dx - 6;
	int det_area_ey = (det_obj_cy + in_half_w + half_dx) < dy ? (det_obj_cy + in_half_w) : dy - half_dy - 6;

	//DebugString(_T("2. dwTick : %d"), ::GetTickCount() - m_dwTick);
	for( int ky = det_area_sy; ky < det_area_ey; ky++ ) {
		for( int kx = det_area_sx; kx < det_area_ex; kx++ ) {
			idx = 0;

			for( int y = ky - half_dy; y <= (ky + half_dy - 5); y = y + 2 ) {
				for( int x = kx - half_dx; x <= (kx + half_dx - 5); x = x + 2 ) {
					avg[0] = (int)((ir_image[(y + 0)*dx + (x + 0)] + ir_image[(y + 0)*dx + (x + 1)] + ir_image[(y + 1)*dx + (x + 0)] + ir_image[(y + 1)*dx + (x + 1)])*0.25f);
					avg[1] = (int)((ir_image[(y + 0)*dx + (x + 2)] + ir_image[(y + 0)*dx + (x + 3)] + ir_image[(y + 1)*dx + (x + 2)] + ir_image[(y + 1)*dx + (x + 3)])*0.25f);
					avg[2] = (int)((ir_image[(y + 0)*dx + (x + 4)] + ir_image[(y + 0)*dx + (x + 5)] + ir_image[(y + 1)*dx + (x + 4)] + ir_image[(y + 1)*dx + (x + 5)])*0.25f);
					avg[3] = (int)((ir_image[(y + 2)*dx + (x + 0)] + ir_image[(y + 2)*dx + (x + 1)] + ir_image[(y + 3)*dx + (x + 0)] + ir_image[(y + 3)*dx + (x + 1)])*0.25f);
					avg[4] = (int)((ir_image[(y + 2)*dx + (x + 2)] + ir_image[(y + 2)*dx + (x + 3)] + ir_image[(y + 3)*dx + (x + 2)] + ir_image[(y + 3)*dx + (x + 3)])*0.25f);
					avg[5] = (int)((ir_image[(y + 2)*dx + (x + 4)] + ir_image[(y + 2)*dx + (x + 5)] + ir_image[(y + 3)*dx + (x + 4)] + ir_image[(y + 3)*dx + (x + 5)])*0.25f);
					avg[6] = (int)((ir_image[(y + 4)*dx + (x + 0)] + ir_image[(y + 4)*dx + (x + 1)] + ir_image[(y + 5)*dx + (x + 0)] + ir_image[(y + 5)*dx + (x + 1)])*0.25f);
					avg[7] = (int)((ir_image[(y + 4)*dx + (x + 2)] + ir_image[(y + 4)*dx + (x + 3)] + ir_image[(y + 5)*dx + (x + 2)] + ir_image[(y + 5)*dx + (x + 3)])*0.25f);
					avg[8] = (int)((ir_image[(y + 4)*dx + (x + 4)] + ir_image[(y + 4)*dx + (x + 5)] + ir_image[(y + 5)*dx + (x + 4)] + ir_image[(y + 5)*dx + (x + 5)])*0.25f);

					diff_pix[0] = (avg[4] - avg[0]) > 0 ? 1 : 0;
					diff_pix[1] = (avg[4] - avg[1]) > 0 ? 1 : 0;
					diff_pix[2] = (avg[4] - avg[2]) > 0 ? 1 : 0;
					diff_pix[3] = (avg[4] - avg[5]) > 0 ? 1 : 0;
					diff_pix[4] = (avg[4] - avg[8]) > 0 ? 1 : 0;
					diff_pix[5] = (avg[4] - avg[7]) > 0 ? 1 : 0;
					diff_pix[6] = (avg[4] - avg[6]) > 0 ? 1 : 0;
					diff_pix[7] = (avg[4] - avg[3]) > 0 ? 1 : 0;

					lbp[idx++] = 128 * diff_pix[7] + 64 * diff_pix[6] + 32 * diff_pix[5] + 16 * diff_pix[4] + 8 * diff_pix[3] + 4 * diff_pix[2] + 2 * diff_pix[1] * diff_pix[0];
				}
			}

			float match = 0;
			float sum_mag_x_feature = 0;
			float sum_mag_2 = 0;
			float sum_feature_2 = 0;

			for( int k = 0; k < idx; k++ ) {
				//match += (lbp[k] - obj_lbp[k]) * (lbp[k] - obj_lbp[k]);

				sum_mag_x_feature += lbp[k] * obj_lbp[k];
				sum_mag_2 += lbp[k] * lbp[k];
				sum_feature_2 += obj_lbp[k] * obj_lbp[k];
			}

			match = sum_mag_x_feature / (float)sqrt(sum_mag_2 * sum_feature_2);

			if( kx == det_obj_cx && ky == det_obj_cy )
				origin_match = match;

			if( match > det_match ) {
				det_match = match;

				det_x = kx;
				det_y = ky;
				det_dx = det_obj_dx;
				det_dy = det_obj_dy;
			}
		}
	}

	//DebugString(_T("3. dwTick : %d,   ")
	//	_T("det_area_sx[%d], det_area_Sy[%d], det_area_ex[%d], ")
	//	_T("det_area_ey[%d], half_dy[%d], half_dx[%d]"), ::GetTickCount() - m_dwTick,
	//	det_area_sx, det_area_sy, det_area_ex, det_area_ey, half_dy, half_dx);

	if( det_match > 0.9 ) {
		if( (det_x - half_dx) < 0 || (det_x + half_dx) >= dx || (det_y - half_dy) < 0 || (det_y + half_dy) >= dy )
			bObj = false;
		else
			bObj = true;
	}

	if( origin_match > 0.9 ) {
		int half_dx = (int)(det_dx * 0.5f) + 10;
		int half_dy = (int)(det_dy * 0.5f) + 10;
		int sum_x = 0;
		int sum_y = 0;
		int max_x = 0;
		int min_x = 1000;
		int max_y = 0;
		int min_y = 1000;
		int sum_idx = 0;
		unsigned short ct_pix = ir_image[det_y * dx + det_x];

		if( (det_x - half_dx) >= 0 && (det_x + half_dx) < dx && (det_y - half_dy) >= 0 && (det_y + half_dy) < dy ) {
			for( int y = det_y - half_dy; y < det_y + half_dy; y++ ) {
				for( int x = det_x - half_dx; x < det_x + half_dx; x++ ) {
					if( abs(ir_image[y*dx + x] - ct_pix) < 500 ) {
						sum_x += x;
						sum_y += y;
						sum_idx++;

						if( max_x < x ) max_x = x;
						if( min_x > x ) min_x = x;
						if( max_y < y ) max_y = y;
						if( min_y > y ) min_y = y;
					}
				}
			}

			det_x2 = (int)(sum_x / sum_idx + 0.5f);
			det_y2 = (int)(sum_y / sum_idx + 0.5f);
			int w = (int)((max_x - min_x)*1.6f);// 6;
			int h = (int)((max_y - min_y)*1.6f);// 6;
			det_dx2 = (w > h) ? w : h;
			det_dy2 = det_dx;
		}
	}

	if( (det_dx - det_obj_dx) <= 2 ) {
		obj_cx = det_x2;
		obj_cy = det_y2;
		obj_dx = det_dx2;
		obj_dy = det_dy2;
	} else {
		obj_cx = det_x;
		obj_cy = det_y;
		obj_dx = det_dx;
		obj_dy = det_dy;
	}

	//obj_cx = det_x;
	//obj_cy = det_y;
	//obj_dx = det_dx;
	//obj_dy = det_dy;
	match_rate = det_match;

	delete[] lbp;
	//DebugString(_T("4. dwTick : %d"), ::GetTickCount() - m_dwTick);
	return bObj;
}

******************************************************************************************/

bool CBBTempCorrection::MatchingLBP(unsigned short *ir_image, int dx, int dy, int in_half_w, int &obj_cx, int &obj_cy, int &obj_dx, int &obj_dy, float &match_rate)
{
	int half_dx = (int)(det_obj_dx * 0.5f);
	int half_dy = (int)(det_obj_dy * 0.5f);
	int diff_pix[8];
	int avg[9] = { 0, };
	float origin_match = 0;
	int det_x = 0;
	int det_y = 0;
	int det_dx = 0;
	int det_dy = 0;
	int det_x2 = 0;
	int det_y2 = 0;
	int det_dx2 = 0;
	int det_dy2 = 0;
	float det_match = 0;
	bool bObj = false;

	int *lbp = new int[(half_dx) * (half_dy)];
	int idx = 0;
	bool bDetect = false;

	int det_area_sx = (det_obj_cx - in_half_w - half_dx) > 0 ? (det_obj_cx - in_half_w) : half_dx+1;
	int det_area_sy = (det_obj_cy - in_half_w - half_dy) > 0 ? (det_obj_cy - in_half_w) : half_dy+1;
	int det_area_ex = (det_obj_cx + in_half_w + half_dx) < dx ? (det_obj_cx + in_half_w) : dx - half_dx-1;
	int det_area_ey = (det_obj_cy + in_half_w + half_dx) < dy ? (det_obj_cy + in_half_w) : dy - half_dy-1;

	int match_x[3]{ 0, };
	int match_y[3]{ 0, };
	float matching_rate[3]{ 0, };
	int match_temp[3]{ 0, };
	int temp_min = 1000000;

	for (int ky = det_area_sy; ky < det_area_ey; ky++)
	{
		for (int kx = det_area_sx; kx < det_area_ex; kx++)
		{
			idx = 0;
			
			for (int y = ky - half_dy; y <= (ky + half_dy - 5); y=y+2)
			{
				for (int x = kx - half_dx; x <= (kx + half_dx - 5); x=x+2)
				{
					avg[0] = (int)((ir_image[(y + 0)*dx + (x + 0)] + ir_image[(y + 0)*dx + (x + 1)] + ir_image[(y + 1)*dx + (x + 0)] + ir_image[(y + 1)*dx + (x + 1)])*0.25f);
					avg[1] = (int)((ir_image[(y + 0)*dx + (x + 2)] + ir_image[(y + 0)*dx + (x + 3)] + ir_image[(y + 1)*dx + (x + 2)] + ir_image[(y + 1)*dx + (x + 3)])*0.25f);
					avg[2] = (int)((ir_image[(y + 0)*dx + (x + 4)] + ir_image[(y + 0)*dx + (x + 5)] + ir_image[(y + 1)*dx + (x + 4)] + ir_image[(y + 1)*dx + (x + 5)])*0.25f);
					avg[3] = (int)((ir_image[(y + 2)*dx + (x + 0)] + ir_image[(y + 2)*dx + (x + 1)] + ir_image[(y + 3)*dx + (x + 0)] + ir_image[(y + 3)*dx + (x + 1)])*0.25f);
					avg[4] = (int)((ir_image[(y + 2)*dx + (x + 2)] + ir_image[(y + 2)*dx + (x + 3)] + ir_image[(y + 3)*dx + (x + 2)] + ir_image[(y + 3)*dx + (x + 3)])*0.25f);
					avg[5] = (int)((ir_image[(y + 2)*dx + (x + 4)] + ir_image[(y + 2)*dx + (x + 5)] + ir_image[(y + 3)*dx + (x + 4)] + ir_image[(y + 3)*dx + (x + 5)])*0.25f);
					avg[6] = (int)((ir_image[(y + 4)*dx + (x + 0)] + ir_image[(y + 4)*dx + (x + 1)] + ir_image[(y + 5)*dx + (x + 0)] + ir_image[(y + 5)*dx + (x + 1)])*0.25f);
					avg[7] = (int)((ir_image[(y + 4)*dx + (x + 2)] + ir_image[(y + 4)*dx + (x + 3)] + ir_image[(y + 5)*dx + (x + 2)] + ir_image[(y + 5)*dx + (x + 3)])*0.25f);
					avg[8] = (int)((ir_image[(y + 4)*dx + (x + 4)] + ir_image[(y + 4)*dx + (x + 5)] + ir_image[(y + 5)*dx + (x + 4)] + ir_image[(y + 5)*dx + (x + 5)])*0.25f);

					diff_pix[0] = (avg[4] - avg[0]) > 0 ? 1 : 0;
					diff_pix[1] = (avg[4] - avg[1]) > 0 ? 1 : 0;
					diff_pix[2] = (avg[4] - avg[2]) > 0 ? 1 : 0;
					diff_pix[3] = (avg[4] - avg[5]) > 0 ? 1 : 0;
					diff_pix[4] = (avg[4] - avg[8]) > 0 ? 1 : 0;
					diff_pix[5] = (avg[4] - avg[7]) > 0 ? 1 : 0;
					diff_pix[6] = (avg[4] - avg[6]) > 0 ? 1 : 0;
					diff_pix[7] = (avg[4] - avg[3]) > 0 ? 1 : 0;

					lbp[idx++] = 128 * diff_pix[7] + 64 * diff_pix[6] + 32 * diff_pix[5] + 16 * diff_pix[4] + 8 * diff_pix[3] + 4 * diff_pix[2] + 2 * diff_pix[1] * diff_pix[0];
				}
			}

			float match = 0;
			float sum_mag_x_feature = 0;
			float sum_mag_2 = 0;
			float sum_feature_2 = 0;

			for (int k = 0; k < idx; k++)
			{
				//match += (lbp[k] - obj_lbp[k]) * (lbp[k] - obj_lbp[k]);

				sum_mag_x_feature += lbp[k] * obj_lbp[k];
				sum_mag_2 += lbp[k] * lbp[k];
				sum_feature_2 += obj_lbp[k] * obj_lbp[k];
			}

			match = sum_mag_x_feature / sqrt(sum_mag_2 * sum_feature_2);

			int obj_value = abs(det_obj_data - GetObjValue(ir_image, dx, dy, kx, ky));

			// 기존 좌표와 동일 했을 경우 데이터 저장
			if (det_obj_cx == kx && det_obj_cy == ky)
			{
				match_x[0] = kx;
				match_y[0] = ky;
				matching_rate[0] = match;
				match_temp[0] = obj_value;
			}

			// 매칭률이 가장 높은 곳 저장
			if (match > det_match)
			{
				det_match = match;

				match_x[1] = kx;
				match_y[1] = ky;
				matching_rate[1] = match;
				match_temp[1] = obj_value;
			}

			// 매칭된 곳의 온도가 가장 유사한 곳
			if (temp_min > obj_value)
			{
				temp_min = obj_value;

				match_x[2] = kx;
				match_y[2] = ky;
				matching_rate[2] = match;
				match_temp[2] = obj_value;
			}
		}
	}

	if (det_match > 0.9)
	{
		if (match_temp[2] < BB_TEMP_DIFF && matching_rate[2] > 0.90f)
		{
			det_x = match_x[2];
			det_y = match_y[2];
			det_dx = det_obj_dx;
			det_dy = det_obj_dy;
			bObj = true;
		}
		else if (match_temp[0] < BB_TEMP_DIFF && matching_rate[0] > 0.95f)
		{
			det_x = match_x[0];
			det_y = match_y[0];
			det_dx = det_obj_dx;
			det_dy = det_obj_dy;
			bDetect = true;
			bObj = true;
		}
		else if (match_temp[1] < BB_TEMP_DIFF && matching_rate[1] > 0.90f)
		{
			det_x = match_x[1];
			det_y = match_y[1];
			det_dx = det_obj_dx;
			det_dy = det_obj_dy;
			bObj = true;
		}
		else {
			bObj = false;
		}

		if( (det_x - half_dx) < 0 || (det_x + half_dx) >= dx || (det_y - half_dy) < 0 || (det_y + half_dy) >= dy )
			bObj = false;
	}

	if ( bObj )
	{
		int half_dx = (int)(det_dx * 0.5f) + 10;
		int half_dy = (int)(det_dy * 0.5f) + 10;
		int sum_x = 0;
		int sum_y = 0;
		int max_x = 0;
		int min_x = 1000;
		int max_y = 0;
		int min_y = 1000;
		int sum_idx = 0;

		int sum =	ir_image[(det_y - 1)*dx + (det_x - 1)] + ir_image[(det_y - 0)*dx + (det_x - 1)] + ir_image[(det_y + 1)*dx + (det_x - 1)] +
					ir_image[(det_y - 1)*dx + (det_x - 0)] + ir_image[(det_y - 0)*dx + (det_x + 0)] + ir_image[(det_y + 1)*dx + (det_x + 0)] +
					ir_image[(det_y - 1)*dx + (det_x + 1)] + ir_image[(det_y - 0)*dx + (det_x + 1)] + ir_image[(det_y + 1)*dx + (det_x + 1)];

		unsigned short ct_pix = (unsigned short)(sum / 9.0f);

		if ((det_x - half_dx -1) >= 0 && (det_x + half_dx + 1) < dx && (det_y - half_dy -1) >= 0 && (det_y + half_dy +1) < dy)
		{
			for (int y = det_y - half_dy; y < det_y + half_dy; y++)
			{
				for (int x = det_x - half_dx; x < det_x + half_dx; x++)
				{
					int sum =	ir_image[(y - 1)*dx + (x - 1)] + ir_image[(y - 0)*dx + (x - 1)] + ir_image[(y + 1)*dx + (x - 1)] +
								ir_image[(y - 1)*dx + (x - 0)] + ir_image[(y - 0)*dx + (x + 0)] + ir_image[(y + 1)*dx + (x + 0)] +
								ir_image[(y - 1)*dx + (x + 1)] + ir_image[(y - 0)*dx + (x + 1)] + ir_image[(y + 1)*dx + (x + 1)];

					unsigned short target_pix = (unsigned short)(sum / 9.0f);

					if (abs(target_pix - ct_pix) < 200)
					{
						sum_x += x;
						sum_y += y;
						sum_idx++;

						if (max_x < x) max_x = x;
						if (min_x > x) min_x = x;
						if (max_y < y) max_y = y;
						if (min_y > y) min_y = y;
					}
				}
			}

			det_x2 = (int)(sum_x / sum_idx + 0.5f);
			det_y2 = (int)(sum_y / sum_idx + 0.5f);

			float rate = 1.6f;

			if (sum_idx > 100) rate = 1.6f;
			else if (sum_idx > 20) rate = 1.8f;
			else rate = 2.2f;

			int w = (int)((max_x - min_x)*rate + 0.5f);// 6;
			int h = (int)((max_y - min_y)*rate + 0.5f);// 6;

			det_dx2 = (w > h) ? w : h;
			det_dy2 = det_dx2;
		}
	}

	if(bObj)
	{
		if (bDetect || (abs(det_x2 - det_obj_cx) <= 1 && abs(det_y2 - det_obj_cy) <= 1))
		{
			obj_cx = det_obj_cx;
			obj_cy = det_obj_cy;
			obj_dx = det_obj_dx;
			obj_dy = det_obj_dy;
		}
		else if (update_obj_cnt >= 0)
		{
			if (update_obj_cnt >= 0 && (abs(det_x2 - update_obj_cx) <= 1 && abs(det_y2 - update_obj_cy) <= 1))
				update_obj_cnt++;
			else
				update_obj_cnt = -1;

			if (update_obj_cnt > UPDATE_KEEP_NO)
			{
				obj_cx = update_obj_cx;
				obj_cy = update_obj_cy;
				obj_dx = det_obj_dx;
				obj_dy = det_obj_dy;

				det_obj_cx = obj_cx;
				det_obj_cy = obj_cy;

				update_obj_cnt = -1;
			}
			else
			{
				obj_cx = det_x2;
				obj_cy = det_y2;
				obj_dx = det_obj_dx;
				obj_dy = det_obj_dy;

				bObj = false;
			}
		}
		else
		{
			obj_cx = det_x2;
			obj_cy = det_y2;
			obj_dx = det_obj_dx;
			obj_dy = det_obj_dy;

			update_obj_cx = det_x2;
			update_obj_cy = det_y2;
			update_obj_cnt = 0;

			bObj = false;
		}
	}

	match_rate = det_match;

	delete[] lbp;

	return bObj;
}

int CBBTempCorrection::GetMovingMean(int value, bool bActive)
{
	int err_sum = 0;

	if (bActive && t1_bActive)
	{
		t3_err = t2_err;
		t2_err = t1_err;
		t1_err = value;

		for (int k = 0; k < 59; k++)
		{
			if (abs(t2_err - t1_err) > NUC_TEMP_CORRECT && abs(t3_err - t1_err) > NUC_TEMP_CORRECT)	// 22.5 = 0.15도, 20 = 0.13도
			{
				err_arr[k] = value;
				err_sum += value;
			}
			else
			{
				err_arr[k] = err_arr[k + 1];
				err_sum += err_arr[k];
			}
		}

		if (abs(t2_err - t1_err) > NUC_TEMP_CORRECT && abs(t3_err - t1_err) > NUC_TEMP_CORRECT)
		{
			err_arr[59] = value;
			t1_err = value;
			t2_err = value;
			t3_err = value;
		}
		else
		{
			err_arr[59] = t1_err;
			err_sum += t1_err;			
		}
	}
	else
	{
		t1_err = err_arr[59];
		t2_err = err_arr[59];

		for (int k = 0; k < 60; k++)
			err_sum += err_arr[k];
	}

	t1_bActive = bActive;

	int mean_err = (int)(err_sum / 60.0);
	int result_err = mean_err;

	if (abs(t2_err - t1_err) > NUC_TEMP_CORRECT)
		result_err = (int)(0.8 * t1_err + 0.2 * mean_err);
	else
		result_err = mean_err;

	//int result_err = (int)(0.8 * t1_moving_mean + 0.2 * mean_err);
	//t1_moving_mean = result_err;

	return result_err;


	/*int err_sum = 0;

	if( bActive && t1_bActive ) {
		t2_err = t1_err;
		t1_err = value;

		for( int k = 0; k < 59; k++ ) {
			err_arr[k] = err_arr[k + 1];
			err_sum += err_arr[k];
		}

		err_arr[59] = t2_err;
		err_sum += t2_err;
	} else {
		t1_err = err_arr[59];
		t2_err = err_arr[59];

		for( int k = 0; k < 60; k++ )
			err_sum += err_arr[k];
	}

	t1_bActive = bActive;

	int mean_err = (int)(err_sum / 60.0);
	int result_err = (int)(0.8 * t1_moving_mean + 0.2 * mean_err);
	t1_moving_mean = result_err;

	return result_err;*/
}

int CBBTempCorrection::GetMovingAverage(int *arr, int &arr_idx, int total_no, int value, bool bReal)
{
	if (bReal)
	{		
		for (int i = 0; i < total_no-1; i++)
		{
			arr[i] = arr[i+1];
		}
		arr[total_no-1] = value;

		if (arr_idx < total_no)
			arr_idx++;
	}

	int sum = 0;
	for (int k = 0; k < total_no; k++)
		sum += arr[k];

	return (int)(sum / (float)arr_idx);
}

/**
* @brief		Compute gain and offset for 2-point correction
* @param[in]	*dpcLv		: Pointer of Camera DPC level data
* @param[in]	width		: image width
* @param[in]	height		: image height
* @param[in]	rect1		: RECT coor structure of low-level blackbody
* @param[in]	rect2		: RECT coor structure of high-level blackbldy
* @param[in]	bb1			: first black-body temperature (unit : celcius)
* @param[in]	bb2			: second black-body temperature (unit : celcius)
* @rtn			error code
*/
const int CBBTempCorrection::Compute2PtCorrect(unsigned short *dpcLv, int width, int height, RECT low_rect, RECT high_rect, float low_bb, float high_bb)
{
	int sum_low_rect = 0;
	int sum_high_rect = 0;
	float low_rect_mean = 0;
	float high_rect_mean = 0;

	if( dpcLv == NULL )
		return -1;

	// first roi region summation
	for( int y = low_rect.top; y < low_rect.bottom; y++ )
		for( int x = low_rect.left; x < low_rect.right; x++ )
			sum_low_rect += dpcLv[y*width + x];

	low_rect_mean = (float)(sum_low_rect / (float)((low_rect.right - low_rect.left)*(low_rect.bottom - low_rect.top)));

	// second roi region summation
	for( int y = high_rect.top; y < high_rect.bottom; y++ )
		for( int x = high_rect.left; x < high_rect.right; x++ )
			sum_high_rect += dpcLv[y*width + x];

	high_rect_mean = (float)(sum_high_rect / (float)((high_rect.right - high_rect.left)*(high_rect.bottom - high_rect.top)));

	gain = (high_bb - low_bb) / (high_rect_mean - low_rect_mean);
	offset = low_bb - gain * low_rect_mean;

	return 1;
}

/**
* @brief		Apply 2-point correction
* @param[in]	*dpcLv			: Pointer of the Camera DPC level data
* @param[in]	width			: image width
* @param[in]	height			: image height
* @param[in]	min_temp		: reference minimum temperature (ex. normal temperature : -30)
* @param[in]	max_temp		: reference maximum temperature (ex. normal temperature : 130)
* @param[out]	*correctLv		: Pointer of the corrected temperature level
* @rtn			error code
*/
const int CBBTempCorrection::Apply2PtCorrect(unsigned short *dpcLv, int width, int height, float min_temp, float max_temp, unsigned short *correctLv)
{
	float ftemp = 0;

	if( dpcLv == NULL )
		return -1;

	for( int k = 0; k < (width*height); k++ ) {
		ftemp = dpcLv[k] * gain + offset;

		float temp_lv = (ftemp - min_temp) * (12000 / (max_temp - min_temp)) + 2000;
		correctLv[k] = (unsigned short)__max(2000, __min(temp_lv, 14000));
	}

	return 1;
}

/**
* @brief		Apply 2-point correction
* @param[in]	*dpcLv			: Camera DPC level data
* @param[in]	width			: image width
* @param[in]	height			: image height
* @param[in]	low_temp_limit	: low temperature limit
* @param[in]	high_temp_limit : high temperature limit
* @param[out]	temp			: celcius temperature of image
* @rtn			error code
*/
const int CBBTempCorrection::Apply2PtCorrect(unsigned short *dpcLv, int width, int height, float min_temp, float max_temp, float *temp)
{
	float ftemp = 0;

	if( dpcLv == NULL )
		return -1;

	for( int k = 0; k < (width*height); k++ ) {
		ftemp = dpcLv[k] * gain + offset;
		if( temp != NULL )
			temp[k] = __max(min_temp, __min(ftemp, max_temp));

		float temp_lv = (ftemp - min_temp) * (12000 / (max_temp - min_temp)) + 2000;
		dpcLv[k] = (unsigned short)__max(2000, __min(temp_lv, 14000));
	}

	return 1;
}


void CBBTempCorrection::SearchBBPosManual(unsigned short *ir_image, int dx, int dy, int sx, int sy, int ex, int ey, int &obj_cx, int &obj_cy)
{
	int w = ex - sx;
	int h = ey - sy;
	int cx = sx + (int)(w*0.5f);
	int cy = sy + (int)(h*0.5f);

	int max = 0;
	int max_cx = cx;
	int max_cy = cy;

	for( int y = (cy - 5); y <= (cy + 5); y++ ) {
		if( (y - 1) >= 0 && (y + 1) < dy ) {
			int mean = (int)((ir_image[(y - 1)*dx + cx] + ir_image[y*dx + cx] + ir_image[(y + 1)*dx + cx]) / 3.0f);

			if( mean > max ) {
				max = mean;
				max_cx = cx;
				max_cy = y;
			}
		}
	}

	for( int x = (cx - 5); x <= (cx + 5); x++ ) {
		int mean = (int)((ir_image[cy * dx + (x - 1)] + ir_image[cy * dx + x] + ir_image[cy * dx + (x + 1)]) / 3.0f);
		if( (x - 1) >= 0 && (x + 1) < dx ) {
			if( mean > max ) {
				max = mean;
				max_cx = x;
				max_cy = cy;
			}
		}
	}

	int sum_x = 0;
	int sum_y = 0;
	int sum_idx = 0;
	unsigned short ct_pix =
		(unsigned short)((ir_image[(max_cy - 1) * dx + (max_cx - 1)] + ir_image[(max_cy - 1) * dx + max_cx] + ir_image[max_cy * dx + (max_cx - 1)] + ir_image[max_cy * dx + max_cx]) / 4.0);

	for( int y = (max_cy - 4); y <= (max_cy + 4); y++ ) {
		for( int x = (max_cx - 4); x <= (max_cx + 4); x++ ) {
			if( abs(ir_image[y*dx + x] - ct_pix) < 500 ) {
				sum_x += x;
				sum_y += y;
				sum_idx++;
			}
		}
	}

	obj_cx = (int)(sum_x / sum_idx + 0.5f);
	obj_cy = (int)(sum_y / sum_idx + 0.5f);
}


int CBBTempCorrection::GetObjValue(unsigned short *ir_image, int dx, int dy, int cx, int cy)
{	
	if ((cx - 1) >= 0 && (cy - 1) >= 0 && (cx + 1) < dx && (cy + 1) < dy)
	{
		int sum =	ir_image[(cy - 1)*dx + (cx - 1)] + ir_image[(cy - 0)*dx + (cx - 1)] + ir_image[(cy + 1)*dx + (cx - 1)] +
					ir_image[(cy - 1)*dx + (cx - 0)] + ir_image[(cy - 0)*dx + (cx + 0)] + ir_image[(cy + 1)*dx + (cx + 0)] +
					ir_image[(cy - 1)*dx + (cx + 1)] + ir_image[(cy - 0)*dx + (cx + 1)] + ir_image[(cy + 1)*dx + (cx + 1)];
		return (int)(sum / 9.0f);
	}

	return 0;
}
