//
//  main.cpp
//  FotoFilter
//
//  Created by Hossain, Rakeeb on 2017-12-24.
//  Copyright © 2017 Hossain, Rakeeb. All rights reserved.
//

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace cv;


int load(int num)
{
    for(int constant = 1; constant <= num; constant++)
    {
        
        Mat image;
        string vidPath = "/Users/232878/Desktop/Strobe/" + to_string(constant) + ".jpg";
        image = imread(vidPath, CV_LOAD_IMAGE_COLOR);   // Read the file
        
        if(! image.data )                              // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
        
        namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
        imshow( "Display window", image );                   // Show our image inside it.
        
        waitKey(0); // Wait for a keystroke in the window
        
    }
    return 0;
}

int main(int argc, const char * argv[])
{
    /*
    
    //Change colors of an image
    if(argc != 1)
    {
        cout << "Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }
    
    Mat image = imread("/Users/232878/Desktop/b.jpg", CV_LOAD_IMAGE_COLOR);

    if(! image.data)
    {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }
    

    for(int x = 0; x < image.rows; x++)
    {
        for(int y = 0; y < image.cols; y++)
        {
            Vec3b color = image.at<Vec3b>(x,y);
            
            if(250 <= color[0])
            {
                color[0] = 0;
                color[1] = 0;
                color[2] = 0;
            }
            
            image.at<Vec3b>(x,y) = color;
        }
    }

    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", image);
    
    waitKey(0);
    return 0;
     */
    
/*
    //Capture framerate of a video
    VideoCapture video(argv[1]);
    
    if (argc != 2)
    {
        cout << "Parameters are incorrect" << endl;
    }
    
    double fps = video.get(CAP_PROP_FPS);
    cout << "Framerate: " << fps << endl;
    
*/
    
    //Extract + Save Frames
    //THIS STUFF USED TO EXIST
    
    string vidDir = "/Users/232878/Desktop/Development/pokemon.mp4";
    string locationDir = "/Users/232878/Desktop/Development/";
    VideoCapture cap(vidDir);
    
    if (!cap.isOpened())
        return -1;
    
    Mat firstFrame;
    Mat nextFrame;
    Mat frame;
    int count = 1;
    
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    cout << cap.get(CV_CAP_PROP_FRAME_COUNT);
    for (count = 1; count < cap.get(CV_CAP_PROP_FRAME_COUNT); count++)
    {
        cap >> frame;
        string filePath = locationDir + to_string(count) + ".jpg";
        imwrite(filePath,frame,compression_params);
    }
    
    //THIS STUFF USED TO EXIST
    /*
    for (anotherCount = 1; anotherCount < cap.get(CV_CAP_PROP_FRAME_COUNT); anotherCount++)
    {
        string path = "/Users/232878/Desktop/Strobe/" + to_string(anotherCount) + ".jpg";
        firstFrame = imread(path);
        
        string nextPath = "/Users/232878/Desktop/Strobe/" + to_string(anotherCount + 1) + ".jpg";
        nextFrame = imread(nextPath);

        for(int x = 0; x < firstFrame.rows; x++)
        {
            for(int y = 0; y < firstFrame.cols; y++)
            {
                
                Vec3b color = firstFrame.at<Vec3b>(x,y);
                Vec3b nextColor = nextFrame.at<Vec3b>(x,y);
                Vec3b newColor = 0;
                
                newColor[0] = (color[0] + nextColor[0])/2;
                newColor[1] = (color[1] + nextColor[1])/2;
                newColor[2] = (color[2] + nextColor[2])/2;

                firstFrame.at<Vec3b>(x,y) = newColor;
                nextFrame.at<Vec3b>(x,y) = newColor;
                
                
                 Point3_<uchar>* color = firstFrame.ptr<Point3_<uchar> >(x,y);
                 Point3_<uchar>* nextColor = nextFrame.ptr<Point3_<uchar> >(x,y);
                 
                 nextColor->x = (color->x + nextColor->x)/2;
                 nextColor->y = (color->y + nextColor->y)/2;
                 nextColor->z = (color->z + nextColor->z)/2;

            }
        }
        for (int i=1; i<51; i=i+2)
        {
            // smooth the image in the "src" and save it to "dst"
            // blur(firstFrame, firstFrame, Size(i,i));
            
            // Gaussian smoothing
            GaussianBlur( firstFrame, firstFrame, Size( i, i ), 0, 0 );
        }
        imwrite(path,firstFrame,compression_params);
        imwrite(nextPath,nextFrame,compression_params);

    }
     */

    /*
    VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.
    
    if (!stream1.isOpened()) { //check if video device has been initialised
        cout << "cannot open camera";
    }
    
    //unconditional loop
    while (true) {
        Mat cameraFrame;
        stream1.read(cameraFrame);
        imshow("cam", cameraFrame);
        if (waitKey(30) >= 0)
            break;
    }
    */

}
