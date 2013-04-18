/*
 *    RoboPeak Project
 *    http://www.robopeak.com
 *
 *    3D Scanner
 *    An applet of RoboticPeak Project
 *
 *    By Shikai Chen (shikai.chn@gmail.com)
 *
 *
 *    Camera Image Undistortor
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "stdafx.h"
#include "common.h"
#include "cameraundistortor.h"

#define SAFE_MAT_RELEASE(p) \
    do {if (p) cvReleaseMat(&(p));}while(0)

CameraUndistortHelper::CameraUndistortHelper(int cx, int cy)
: _is_loaded(false)
, _frame_cx(cx)
, _frame_cy(cy)
, _intrinsic_cam(NULL)
, _intrinsic_distort(NULL)
, _cam_map_x(NULL)
, _cam_map_y(NULL)
{
    _cam_map_x = cvCreateMat(cy, cx, CV_32F);
    _cam_map_y = cvCreateMat(cy, cx, CV_32F);
}

CameraUndistortHelper::~CameraUndistortHelper()
{
    SAFE_MAT_RELEASE(_intrinsic_cam);
    SAFE_MAT_RELEASE(_intrinsic_distort);
    SAFE_MAT_RELEASE(_cam_map_x);
    SAFE_MAT_RELEASE(_cam_map_y);
}

bool CameraUndistortHelper::loadCalibrationfromFile(const char * camerainfofile, const char * cameradistortfile){
    SAFE_MAT_RELEASE(_intrinsic_cam);
    SAFE_MAT_RELEASE(_intrinsic_distort);
    _is_loaded = false;
    _intrinsic_cam = (CvMat*)cvLoad(camerainfofile);
    _intrinsic_distort  = (CvMat*)cvLoad(cameradistortfile);

    if (!_intrinsic_cam || !_intrinsic_distort) return false;

    cvInitUndistortMap( _intrinsic_cam, _intrinsic_distort, 
            _cam_map_x, _cam_map_y);
    _is_loaded = true;
    return true;
}

void CameraUndistortHelper::undistortImage(const IplImage * input, IplImage * output)
{
    if (!_is_loaded) return;

    cvRemap(input, output, _cam_map_x, _cam_map_y);
}
