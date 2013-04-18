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
 *    PC Client, Main Logic
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
#include "scanner_client.h"

#include "model_render.h"

using namespace rp::net;


#define PI 3.14159265


int total_dev = 0;

int avaliable_dev_list_proc (hid_dev_desc_t * desc)
{
    printf(" Dev#%d Path:%s\n", 
        total_dev++, desc->dev_path);
    return 0;
}


C3DScanner theScanner;
CvCapture  * cam_input = NULL;


bool dev_connect(int cameraID)
{
    printf("--List all the connected devices:\n");
    theScanner.enum_dev(avaliable_dev_list_proc);
    printf("--End of dev listing\n");

    if (total_dev==0)
    {
        printf("No avaliable device found, exit...\n");
        return false;
    }

    printf("Chose the first one to connect...\n");
    if (!theScanner.bind())
    {
        printf("Cannot connect to the target device, exit...\n");
        return false;
    }

    printf("Capture video signal from camera ID: %d...", cameraID);
    cam_input = cvCaptureFromCAM(CV_CAP_DSHOW+cameraID);
    if (!cam_input){
        printf("Error.\n  Cannot connect to the camera.\n");
        return false;
    }
    printf("OK\n");
    return true;
}


void dev_disconnect()
{
    cvReleaseCapture( &cam_input );
}




float findLaserCenterByRow(unsigned char * row, size_t rowsz)
{
    static const unsigned char THRESHOLD_MIN_PWR = 25;
    static const unsigned char THRESHOLD_BLOB_DIFF = 10;
    static const int           THRESHOLD_ALLOWED_BLOB_SZ = 20;
    int centerPos = 0;
    unsigned char maxPwr = 0;
    int centerSize = 0;

    int currentPos = 0;
    while (currentPos<rowsz) {
        if (maxPwr<row[currentPos]) {
            centerSize = 1;
            int centerPos_candidate = currentPos;
            unsigned char maxPwr_candidate = row[currentPos];
            maxPwr=maxPwr_candidate;
            centerPos = centerPos_candidate;
        }
        else 
        {
            ++currentPos;
        }
    }

    if (maxPwr < THRESHOLD_MIN_PWR) return -1;

    float logicPwr = 0.0f, totalPwr=0.0f;

    for ( currentPos = centerPos-10; currentPos<=centerPos+10; currentPos++)
    {
        float currentPwr;
        if (currentPos>=0 && currentPos<rowsz){
           currentPwr = row[currentPos];
        }else{
           currentPwr = 0.0f;
        }
        logicPwr+=currentPwr;
        totalPwr+=currentPwr*currentPos;
    }

    return totalPwr/logicPwr;
}


void on_mouse_evt(int evt,int x,int y,int flags,void*prama)
{
    static int piccnt = 0;
    if (evt == 1){
        char filename[250];
        sprintf(filename, "img%03d.bmp", piccnt++);
        IplImage* currentFrame = cvQueryFrame(cam_input);
        cvSaveImage(filename, currentFrame);
    }
}

void camera_calibration_helper(int argc, _TCHAR* argv[])
{
    IplImage* currentFrame;
    cvNamedWindow("Original");

    int cameraID = 0;
    if (argc>=2) {
        cameraID = atoi(argv[1]);
    }

    printf("Capture video signal from camera ID: %d...", cameraID);
    cam_input = cvCaptureFromCAM(CV_CAP_DSHOW+cameraID);
    if (!cam_input){
        printf("Error.\n  Cannot connect to the camera.\n");
        return;
    }

    cvSetMouseCallback("Original",  on_mouse_evt);

    while(1){
        currentFrame = cvQueryFrame(cam_input);
        ::cvShowImage("Original", currentFrame);

        int key = cvWaitKey(1);
        if (key == VK_ESCAPE)
                break;

    }

}

struct MeasureResult
{
    float distance;
    float yaw;
    float pitch;
};


void calcDistanceByPos( float x, int y, int img_height, MeasureResult * result)
{
/*
       dist = a/(b-c*x)+d
*/
    const float a = 698.1;
    const float b = 4.354;
    const float c = 0.006912;
    const float d = -40.88;
    
    const float baseline = 100.0;

    const float laser_angle = 1.486407302;
    const float rotation_r  = 49.2;
    const float focal = a/baseline;

    float center_distance = a/(b-c*x);

    float pitch_angle = atan(((y - img_height/2)*c)/(focal));
    float pitch_distance = center_distance/cos(pitch_angle);
    
    float laser_to_dist_pt = center_distance*tan(PI/2 - laser_angle);
    float laser_to_current_pt = sqrt(pitch_distance*pitch_distance + laser_to_dist_pt*laser_to_dist_pt);
    float laser_to_center_pt  = sqrt(center_distance*center_distance + laser_to_dist_pt*laser_to_dist_pt);


    float real_center_distance = sqrt( (laser_to_dist_pt-rotation_r)*(laser_to_dist_pt-rotation_r) +  center_distance*center_distance);
    float yaw_angle = PI/2 - acos((rotation_r*rotation_r + real_center_distance*real_center_distance - laser_to_center_pt*laser_to_center_pt)/2.0f/rotation_r/real_center_distance);
 

    float real_distance = sqrt( (laser_to_dist_pt-rotation_r)*(laser_to_dist_pt-rotation_r) +  pitch_distance*pitch_distance);

    result->distance = real_distance;
    result->yaw = yaw_angle;
    result->pitch = pitch_angle -18.0*PI/180.0f;
}


void renderTester()
{
    int cloud_pt_row_delta = 5;
    int cloud_pt_row_num = 480/cloud_pt_row_delta;

    ModelRender render;
    render.init(true,1024,700, video::SColor(255,0,0,0));


    FILE * dumpfile = fopen("dump__.asc", "r");

    int col,  row;
    float x, y,z;

    int lastcol =0 , lastrow=0;

    CloudPoint * cloudpt_col = new CloudPoint[cloud_pt_row_num];

    while ( fscanf(dumpfile,"%d %d %f %f %f", &col, &row, &x, &y, &z) == 5 ) {
        

        if (col == lastcol) {    
            for (int missingrow = lastrow+1; missingrow<row; ++missingrow)
            {
                cloudpt_col[missingrow].col = col;
                cloudpt_col[missingrow].row = missingrow;
                cloudpt_col[missingrow].height = cloud_pt_row_num;
                cloudpt_col[missingrow].X = 0;
                cloudpt_col[missingrow].Y = 0;
                cloudpt_col[missingrow].Z = 0;
            }
        } else {
            for (int missingrow = lastrow+1; missingrow<cloud_pt_row_num; ++missingrow)
            {
                cloudpt_col[missingrow].col = lastcol;
                cloudpt_col[missingrow].row = missingrow;
                cloudpt_col[missingrow].height = cloud_pt_row_num;
                cloudpt_col[missingrow].X = 0;
                cloudpt_col[missingrow].Y = 0;
                cloudpt_col[missingrow].Z = 0;
            }

            render.appendData(cloudpt_col, cloud_pt_row_num);

            for (int missingcol = lastcol+1; missingcol<col; ++missingcol)
            {
                for (int missingrow = 0; missingrow<cloud_pt_row_num; ++missingrow)
                {
                    cloudpt_col[missingrow].col = lastcol;
                    cloudpt_col[missingrow].row = missingrow;
                    cloudpt_col[missingrow].height = cloud_pt_row_num;
                    cloudpt_col[missingrow].X = 0;
                    cloudpt_col[missingrow].Y = 0;
                    cloudpt_col[missingrow].Z = 0;
                }
                render.appendData(cloudpt_col, cloud_pt_row_num);
            }

            render.do_render();
            delay(1000/30);
        }


        cloudpt_col[row].col = col;
        cloudpt_col[row].row = row;
        cloudpt_col[row].height = cloud_pt_row_num;
        cloudpt_col[row].X = x;
        cloudpt_col[row].Y = y;
        cloudpt_col[row].Z = z;
        lastcol = col;
        lastrow = row;
    }

    delete [] cloudpt_col;
    fclose(dumpfile);
    render.fin();
    render.run();
}

void show_help(int argc, _TCHAR* argv[])
{
    printf("3D Laser Scanner client.\n"
           "Usage :\n"
           "%1 [options] [-d cloudpoint file] [camera id]\n\n"
           "Options:\n"
           " --start <angle>       Start Pan Angle.\n"
           "                       default : 0\n"
           " --end   <angle>       End Pan Angle.\n"
           "                       default : 180\n"
           " --delta <angle>       Stepping Angle.\n"
           "                       default : 0.5\n"
           , argv[0]);
}

int _tmain(int argc, _TCHAR* argv[])
{
    const int IMAGE_HEIGHT = 480;
    const int cloud_pt_row_delta = 1;

    int opt_start_angle10 = 0;
    int opt_end_angle10   = 1800;
    int opt_angle_stepping10 = 5;
    
    bool opt_show_help =false;
    const char * cloudpoint_file = NULL;
    int cameraID = 0;

    int pos = 1;
    while (pos < argc) {
        char * current_opt = argv[pos];

        if (strcmp(current_opt, "-h") == 0 || strcmp(current_opt, "--help") == 0 ) {
            opt_show_help = true;
        } else if (strcmp(current_opt, "-d") == 0 ) {
            cloudpoint_file = argv[++pos];
        } else if (strcmp(current_opt, "--start") == 0) {
            opt_start_angle10 = atof(argv[++pos])*10;
        } else if (strcmp(current_opt, "--end") == 0) {
            opt_end_angle10 = atof(argv[++pos])*10;
        } else if (strcmp(current_opt, "--delta") == 0) {
            opt_angle_stepping10 = atof(argv[++pos])*10;
        }  else {
            cameraID = atoi(argv[pos]);
        }
        ++pos;
    }

//    renderTester();
//    return 0;
#if 0
    camera_calibration_helper(argc, argv);
    return 0;
#endif

    if (opt_show_help) {
        show_help(argc, argv);
        return 0;
    }

    int cloud_pt_row_num = IMAGE_HEIGHT/cloud_pt_row_delta;
    FILE * dumpfile = NULL;
    IplImage* currentFrame;
    IplImage* undistorted;
    IplImage* grayFrame;
    CvSize frameSz;
    CvFont  *  stdFont = new CvFont();
    cvInitFont(stdFont,CV_FONT_VECTOR0,0.5,0.5, 0, 1);
    
    ModelRender render;
    
    int a1;
    int currentAngle10;
    do {
        if (!dev_connect(cameraID)) {
            break;
        }

        currentFrame = cvQueryFrame(cam_input);
        frameSz.height = currentFrame->height;
        frameSz.width  = currentFrame->width;


        CameraUndistortHelper undistortor(currentFrame->width, currentFrame->height);

        if (!undistortor.loadCalibrationfromFile("cam_calib_result/intrinsics_cam.xml","cam_calib_result/intrinsics_distort.xml")){
            printf("error while loading the camera calibration data.\n");
            break;
        }

        //creating view port
        cvNamedWindow("Original");
        cvNamedWindow("Rectified");
        cvNamedWindow("Gray");
        // reset the scanner
        theScanner.setAngle(opt_start_angle10);
        theScanner.enableLaser();


        render.init(true,1024,700, video::SColor(255,0,0,0));

        a1 = 30*3;

    
            
        //waiting the servo to reach the specified heading...
        while(a1--){
            currentFrame = cvQueryFrame(cam_input);
        }

        currentAngle10 = opt_start_angle10;

        float * laserDotArr = new float[frameSz.height];
        grayFrame = cvCreateImage(cvSize(frameSz.width, frameSz.height), IPL_DEPTH_8U, 1);
        undistorted = cvCreateImage(cvSize(frameSz.width, frameSz.height), IPL_DEPTH_8U, 3);
       
        if (cloudpoint_file)
            dumpfile = fopen(cloudpoint_file, "w");

        MeasureResult result = { 0, 0, 0};

        CloudPoint * cloudpt_col = new CloudPoint[cloud_pt_row_num];
        

        while (currentAngle10<opt_end_angle10) {
            currentFrame = cvQueryFrame(cam_input);
            if (!currentFrame) break;

            undistortor.undistortImage(currentFrame, undistorted);

            //convert to gray level only
            cvCvtColor(undistorted, grayFrame, CV_BGR2GRAY);

            //reduce noise
            cvSmooth( grayFrame, grayFrame, CV_GAUSSIAN, 3, 3 );
            
            for (int y=0; y<frameSz.height; ++y)
            {
                laserDotArr[y] = findLaserCenterByRow((unsigned char *)&(grayFrame->imageData[y*grayFrame->widthStep]), frameSz.width);
            }
            

            for (int y=0; y<frameSz.height; ++y)
            {
                if (y % cloud_pt_row_delta == 0)
                {
                    CloudPoint & currentpt = cloudpt_col[y/cloud_pt_row_delta];
                    currentpt.row  = (y / cloud_pt_row_delta);
                    currentpt.col= (currentAngle10 - opt_start_angle10)/opt_angle_stepping10;
                    currentpt.height = cloud_pt_row_num;

                    currentpt.X = currentpt.Y = currentpt.Z = 0;
                }

                //  if (y % 2) continue;
                if (laserDotArr[y] != -1)
                {
                    cvCircle(undistorted,cvPoint(laserDotArr[y],y),2, y == frameSz.height/2?CV_RGB(255,0,0):CV_RGB(0,255,255));
/*
                    char txtMsg[200];
                    if ( y == frameSz.height/2) {
                        sprintf(txtMsg,"x=%.3f.", laserDotArr[y]);
                        printf("current x=%.3f\n", laserDotArr[y]);
                        cvPutText(undistorted, txtMsg, cvPoint(laserDotArr[y],y),stdFont, cvScalar( 200,200,200));
                    }
*/
                    
                    calcDistanceByPos(laserDotArr[y], y, frameSz.height, &result);
                   // printf("%.2f %.2f %.2f\n", result.distance, result.yaw * 180.0f/PI + currentAngle10/10, result.pitch* 180.0f/PI);

                    
                    float px, py ,pz ;
                    px = result.distance * cos(result.pitch) * sin( result.yaw  + currentAngle10*PI/10.0f/180.0f);
                    py = result.distance * cos(result.pitch) * cos( result.yaw  + currentAngle10*PI/10.0f/180.0f);
                    pz = result.distance * sin(-result.pitch);

                    if (fabs(px)>5000 || fabs(py)>5000 || fabs(pz)>1000) continue;
 
                    


                    if (y % cloud_pt_row_delta == 0)
                    {
                        CloudPoint & currentpt = cloudpt_col[y/cloud_pt_row_delta];


                        currentpt.X = px;
                        currentpt.Y = py;
                        currentpt.Z = pz;

                        if (cloudpoint_file)
                            fprintf(dumpfile, "%.2f %.2f %.2f\n", px
                                                            , py
                                                            , pz );
                    }

                }

            }            
            cvShowImage("Original", currentFrame); 
            cvShowImage("Rectified", undistorted);
            cvShowImage("Gray", grayFrame); 
            currentAngle10+=opt_angle_stepping10;
            theScanner.setAngle(currentAngle10);
            render.appendData(cloudpt_col, cloud_pt_row_num);
            render.do_render();

            cvWaitKey(1);
        }

        delete [] cloudpt_col;
        if (cloudpoint_file)
            fclose(dumpfile);
        delete [] laserDotArr;
        cvReleaseImage(&grayFrame);
        cvReleaseImage(&undistorted);
    }while(0);
    
    theScanner.disableLaser();
    dev_disconnect();
    render.fin();
    render.run();
}

