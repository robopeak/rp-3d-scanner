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


#pragma once

// OpenCV is required

class CameraUndistortHelper
{
public:
    CameraUndistortHelper(int cx, int cy);
    ~CameraUndistortHelper();

    bool loadCalibrationfromFile(const char * camerainfofile, const char * cameradistortfile);
    void undistortImage(const IplImage * input, IplImage * output);

protected:

    bool    _is_loaded;
    int     _frame_cx, _frame_cy;
    CvMat * _intrinsic_cam;
    CvMat * _intrinsic_distort;
    CvMat *  _cam_map_x, *_cam_map_y;
};