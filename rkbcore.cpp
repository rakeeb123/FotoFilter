//
//  main.cpp
//  GradientTest
//
//  Created by Hossain, Rakeeb on 2018-01-02.
//  Copyright © 2018 Hossain, Rakeeb. All rights reserved.
//

#include <iostream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <opencv2/imgproc/imgproc.hpp>
#include <dirent.h>
#include <cmath>
#include <fstream>

using namespace std;
using namespace cv;

enum SeverityScale
{
    MODERATETOLOW = 6,
    MODERATE,
    MODERATETOHIGH,
    HIGH,
    HIGHER,
    HIGHEST,
};

string vidDir = "/Users/232878/Desktop/Development/";

void makeFrameGradient(int lowerBound, int upperBound, string gradientFolder) {
    string lowerBoundGradientPath = gradientFolder + to_string(lowerBound) + ".jpg";
    string nextGradientPath = gradientFolder + to_string(lowerBound + 1) + ".jpg";
    string upperBoundGradientPath = gradientFolder + to_string(upperBound) + ".jpg";
    
    Mat frame1 = imread(lowerBoundGradientPath);
    Mat frame2 = imread(nextGradientPath);
    Mat frameFinal = imread(upperBoundGradientPath);
    Mat newFrame = frame1;
    bool seizureBool = true;
    bool continueBool = false;
    int thresholdInt = 135;
    for (int count = 1; count <= 8; count++)
    {
        string pointPath = vidDir + to_string(lowerBound + count) + ".jpg";
        for(int x = 0; x < frame1.rows; x++)
        {
            for(int y = 0; y < frame1.cols; y++)
            {
                Vec3b color1 = frame1.at<Vec3b>(x,y);
                Vec3b color2 = frame2.at<Vec3b>(x,y);
                Vec3b newColorFrame = newFrame.at<Vec3b>(x,y);
                
                if (color1[0] - color2[0] > thresholdInt)
                {
                    newColorFrame[0] = color1[0] - 10;
                }
                else if (color2[0] - color1[0] > thresholdInt)
                {
                    newColorFrame[0] = color1[0] + thresholdInt;
                }
                else if (color1[1] - color2[1] > thresholdInt)
                {
                    newColorFrame[1] = color1[1] - 10;
                    
                }
                else if (color2[1] - color1[1] > thresholdInt)
                {
                    newColorFrame[1] = color1[1] + 10;
                    
                }
                else if (color1[2] - color2[2] > thresholdInt)
                {
                    newColorFrame[2] = color1[2] - 10;
                }
                else if (color2[2] - color1[2] > thresholdInt)
                {
                    newColorFrame[2] = color1[2] + 10;
                }
                else
                {
                    continueBool = true;
                }
                newFrame.at<Vec3b>(x,y) = newColorFrame;
            }
        }
        if (continueBool == true)
        {
            frame2 = imread(pointPath);
        }
        
        frame1 = newFrame;
        imwrite(pointPath, newFrame);
        
        if (count == 8 && seizureBool == true)
        {
            lowerBound = lowerBound + count;
            lowerBoundGradientPath = gradientFolder + to_string(lowerBound) + ".jpg";
            nextGradientPath = gradientFolder + to_string(lowerBound + 1) + ".jpg";
            frame1 = imread(lowerBoundGradientPath);
            frame2 = imread(nextGradientPath);
            count = 0;
        }
        if (lowerBound + count == upperBound)
        {
            break;
        }
    }
}

void lowerContrast(int lowerBound, int upperBound, string gradientFolder, double alpha)
{
    
    for (int count = 0; count <= (upperBound - lowerBound); count++)
    {
        string lowerBoundGradientPath = gradientFolder + to_string(lowerBound + count) + ".jpg";
        Mat frame1 = imread(lowerBoundGradientPath);
        Mat updatedFrame = Mat::zeros( frame1.size(), frame1.type() );
        
        for(int x = 0; x < frame1.rows; x++)
        {
            for(int y = 0; y < frame1.cols; y++)
            {
                for(int c = 0; c < 3; c++)
                {
                    updatedFrame.at<Vec3b>(x,y)[c] = saturate_cast<uchar>(alpha*(frame1.at<Vec3b>(x,y)[c]) + 0);
                }
            }
        }
        imwrite(lowerBoundGradientPath, updatedFrame);
    }
}

void overlayFilter(int lowerBound, int upperBound, string gradientFolder, double alpha)
{
    for (int count = 0; count <= (upperBound - lowerBound); count++)
    {
        string lowerBoundGradientPath = gradientFolder + to_string(lowerBound + count) + ".jpg";
        Mat frame1 = imread(lowerBoundGradientPath);
        Mat overlay;
        frame1.copyTo(overlay);
        cv::rectangle(overlay, cv::Rect(0, 0, frame1.cols, frame1.rows), cv::Scalar(198, 202, 209), -1);

        cv::addWeighted(overlay, alpha, frame1, 1.0 - alpha, 0.0, frame1);
        
        imwrite(lowerBoundGradientPath, frame1);
    }
}

void blurFilter(int lowerBound, int upperBound, string gradientFolder, double alpha)
{
    for (int count = 0; count <= (upperBound - lowerBound); count++)
    {
        string lowerBoundGradientPath = gradientFolder + to_string(lowerBound + count) + ".jpg";
        Mat frame1 = imread(lowerBoundGradientPath);
        blur(frame1, frame1, Size(alpha, alpha));
        
        imwrite(lowerBoundGradientPath, frame1);
    }
}

vector< vector<int> > relativeLuminance(Mat colorFrame)
{
    vector<vector<int> > brightnessMatrix;
    brightnessMatrix.resize(colorFrame.rows);
    for (int x = 0; colorFrame.rows > x; x++)
    {
        brightnessMatrix[x].resize(colorFrame.cols);
        
        for (int y = 0; colorFrame.cols > y; y++)
        {
            int pixelValue = (int)colorFrame.at<uchar>(x,y);
            brightnessMatrix[x][y] = pixelValue;
        }
    }
    return brightnessMatrix;
}


vector<vector<int > > seizureDetection(string path)
{
    
    int numOfFrames = 300;
    int n = 0;
    int seizureCount = 0;
    
    vector<vector<int > > seizureBoundaries(round(300/30), vector<int>(2));
    
    while(n + 30 <= numOfFrames)
    {
        cout << "Level: " << n << endl;
        string path1 = path + to_string(n+1) + ".jpg";
        string path2 = path + to_string(n+2) + ".jpg";
        string path3 = path + to_string(n+3) + ".jpg";
        string path4 = path + to_string(n+4) + ".jpg";
        string path5 = path + to_string(n+5) + ".jpg";
        string path6 = path + to_string(n+6) + ".jpg";
        string path7 = path + to_string(n+7) + ".jpg";
        string path8 = path + to_string(n+8) + ".jpg";
        string path9 = path + to_string(n+9) + ".jpg";
        string path10 = path + to_string(n+10) + ".jpg";
        string path11 = path + to_string(n+11) + ".jpg";
        string path12 = path + to_string(n+12) + ".jpg";
        string path13 = path + to_string(n+13) + ".jpg";
        string path14 = path + to_string(n+14) + ".jpg";
        string path15 = path + to_string(n+15) + ".jpg";
        string path16 = path + to_string(n+16) + ".jpg";
        string path17 = path + to_string(n+17) + ".jpg";
        string path18 = path + to_string(n+18) + ".jpg";
        string path19 = path + to_string(n+19) + ".jpg";
        string path20 = path + to_string(n+20) + ".jpg";
        string path21 = path + to_string(n+21) + ".jpg";
        string path22 = path + to_string(n+22) + ".jpg";
        string path23 = path + to_string(n+23) + ".jpg";
        string path24 = path + to_string(n+24) + ".jpg";
        string path25 = path + to_string(n+25) + ".jpg";
        string path26 = path + to_string(n+26) + ".jpg";
        string path27 = path + to_string(n+27) + ".jpg";
        string path28 = path + to_string(n+28) + ".jpg";
        string path29 = path + to_string(n+29) + ".jpg";
        string path30 = path + to_string(n+30) + ".jpg";
        
        Mat frame1 = imread(path1);
        Mat frame2 = imread(path2);
        Mat frame3 = imread(path3);
        Mat frame4 = imread(path4);
        Mat frame5 = imread(path5);
        Mat frame6 = imread(path6);
        Mat frame7 = imread(path7);
        Mat frame8 = imread(path8);
        Mat frame9 = imread(path9);
        Mat frame10 = imread(path10);
        Mat frame11 = imread(path11);
        Mat frame12 = imread(path12);
        Mat frame13 = imread(path13);
        Mat frame14 = imread(path14);
        Mat frame15 = imread(path15);
        Mat frame16 = imread(path16);
        Mat frame17 = imread(path17);
        Mat frame18 = imread(path18);
        Mat frame19 = imread(path19);
        Mat frame20 = imread(path20);
        Mat frame21 = imread(path21);
        Mat frame22 = imread(path22);
        Mat frame23 = imread(path23);
        Mat frame24 = imread(path24);
        Mat frame25 = imread(path25);
        Mat frame26 = imread(path26);
        Mat frame27 = imread(path27);
        Mat frame28 = imread(path28);
        Mat frame29 = imread(path29);
        Mat frame30 = imread(path30);
        
        cvtColor(frame1, frame1, CV_BGR2GRAY);
        cvtColor(frame2, frame2, CV_BGR2GRAY);
        cvtColor(frame3, frame3, CV_BGR2GRAY);
        cvtColor(frame4, frame4, CV_BGR2GRAY);
        cvtColor(frame5, frame5, CV_BGR2GRAY);
        cvtColor(frame6, frame6, CV_BGR2GRAY);
        cvtColor(frame7, frame7, CV_BGR2GRAY);
        cvtColor(frame8, frame8, CV_BGR2GRAY);
        cvtColor(frame9, frame9, CV_BGR2GRAY);
        cvtColor(frame10, frame10, CV_BGR2GRAY);
        cvtColor(frame11, frame11, CV_BGR2GRAY);
        cvtColor(frame12, frame12, CV_BGR2GRAY);
        cvtColor(frame13, frame13, CV_BGR2GRAY);
        cvtColor(frame14, frame14, CV_BGR2GRAY);
        cvtColor(frame15, frame15, CV_BGR2GRAY);
        cvtColor(frame16, frame16, CV_BGR2GRAY);
        cvtColor(frame17, frame17, CV_BGR2GRAY);
        cvtColor(frame18, frame18, CV_BGR2GRAY);
        cvtColor(frame19, frame19, CV_BGR2GRAY);
        cvtColor(frame20, frame20, CV_BGR2GRAY);
        cvtColor(frame21, frame21, CV_BGR2GRAY);
        cvtColor(frame22, frame22, CV_BGR2GRAY);
        cvtColor(frame23, frame23, CV_BGR2GRAY);
        cvtColor(frame24, frame24, CV_BGR2GRAY);
        cvtColor(frame25, frame25, CV_BGR2GRAY);
        cvtColor(frame26, frame26, CV_BGR2GRAY);
        cvtColor(frame27, frame27, CV_BGR2GRAY);
        cvtColor(frame28, frame28, CV_BGR2GRAY);
        cvtColor(frame29, frame29, CV_BGR2GRAY);
        cvtColor(frame30, frame30, CV_BGR2GRAY);
        
        vector < vector<int> > frame1Luminance = relativeLuminance(frame1);
        vector < vector<int> > frame2Luminance = relativeLuminance(frame2);
        vector < vector<int> > frame3Luminance = relativeLuminance(frame3);
        vector < vector<int> > frame4Luminance = relativeLuminance(frame4);
        vector < vector<int> > frame5Luminance = relativeLuminance(frame5);
        vector < vector<int> > frame6Luminance = relativeLuminance(frame6);
        vector < vector<int> > frame7Luminance = relativeLuminance(frame7);
        vector < vector<int> > frame8Luminance = relativeLuminance(frame8);
        vector < vector<int> > frame9Luminance = relativeLuminance(frame9);
        vector < vector<int> > frame10Luminance = relativeLuminance(frame10);
        vector < vector<int> > frame11Luminance = relativeLuminance(frame11);
        vector < vector<int> > frame12Luminance = relativeLuminance(frame12);
        vector < vector<int> > frame13Luminance = relativeLuminance(frame13);
        vector < vector<int> > frame14Luminance = relativeLuminance(frame14);
        vector < vector<int> > frame15Luminance = relativeLuminance(frame15);
        vector < vector<int> > frame16Luminance = relativeLuminance(frame16);
        vector < vector<int> > frame17Luminance = relativeLuminance(frame17);
        vector < vector<int> > frame18Luminance = relativeLuminance(frame18);
        vector < vector<int> > frame19Luminance = relativeLuminance(frame19);
        vector < vector<int> > frame20Luminance = relativeLuminance(frame20);
        vector < vector<int> > frame21Luminance = relativeLuminance(frame21);
        vector < vector<int> > frame22Luminance = relativeLuminance(frame22);
        vector < vector<int> > frame23Luminance = relativeLuminance(frame23);
        vector < vector<int> > frame24Luminance = relativeLuminance(frame24);
        vector < vector<int> > frame25Luminance = relativeLuminance(frame25);
        vector < vector<int> > frame26Luminance = relativeLuminance(frame26);
        vector < vector<int> > frame27Luminance = relativeLuminance(frame27);
        vector < vector<int> > frame28Luminance = relativeLuminance(frame28);
        vector < vector<int> > frame29Luminance = relativeLuminance(frame29);
        vector < vector<int> > frame30Luminance = relativeLuminance(frame30);
        vector<vector<vector<int> > > luminanceMatrix {frame1Luminance, frame2Luminance, frame3Luminance, frame4Luminance, frame5Luminance, frame6Luminance, frame7Luminance, frame8Luminance, frame9Luminance, frame10Luminance, frame11Luminance, frame12Luminance, frame13Luminance, frame14Luminance, frame15Luminance, frame16Luminance, frame17Luminance, frame18Luminance, frame19Luminance, frame20Luminance, frame21Luminance, frame22Luminance, frame23Luminance, frame24Luminance, frame25Luminance, frame26Luminance, frame27Luminance, frame28Luminance, frame29Luminance, frame30Luminance};
        
        int a = 16;
        int b = 16;
        bool adjustCols = false;
        bool adjustRows = false;
        int outCountX = 0;
        int outCountY = 0;
        
        vector<double> avgLumMatrix(30);
        double avgLum = 0.0;
        //Loop through frames 16x16 and do DFT
        for(int j = 0; j < frame1.rows; j += 16)
        {
            adjustCols = false;
            if (frame1.rows % 16 != 0 && j + 16 > frame1.rows && adjustRows == false)
            {
                a = frame1.rows - j;
                adjustRows = !adjustRows;
            }
            outCountX++;
            
            for(int k = 0; k < frame1.cols; k = k + 16)
            {
                if (frame1.cols % 16 != 0 && k + 16 > frame1.cols && adjustCols == false)
                {
                    b = frame1.cols - k;
                    adjustCols = !adjustCols;
                }
                
                outCountY++;
                for(int i = 0; i < 30; i++)
                {
                    for (int x = 0; x < a; x++)
                    {
                        for (int y = 0; y < b; y++)
                        {
                            avgLum += luminanceMatrix[i][x + j][y + k];
                        }
                    }
                    avgLum = avgLum/(a*b);
                    avgLumMatrix[i] = avgLum;
                    avgLum = 0;
                }
                if (outCountX == 62 && outCountY == 4 && n == 0) {
                    cout << "we're in\n";
                    ofstream outputFile;
                    string filename = "output.csv";
                    outputFile.open(filename);
                    if (outputFile.is_open())
                    {
                        for (int z = 0; z < avgLumMatrix.size(); z++) {
                            outputFile << avgLumMatrix[z] << "\n";
                        }
                        outputFile.close();
                    }
                }
                // Try and find peak of FFT array instead
                vector<double> amp = avgLumMatrix;
                vector<double> peaks;
                vector<double> index = {0};
                // vector<double>::iterator it;
                const int NOISE = -1;               // Level up to and including which peaks will be excluded
                int wideStart = -1;                 // The start of any current wide peak
                int grad = -1;                      // Sign of gradient (almost)
                //    =  1 for increasing
                //    =  0 for level AND PREVIOUSLY INCREASING (so potential wide peak)
                //    = -1 for decreasing OR level, but previously decreasing
                // A sharp peak is identified by grad=1 -> grad=-1
                // A wide  peak is identified by grad=0 -> grad=-1
                double count = 0.0;
                for ( int i = 0; i < amp.size(); i++ )
                {
                    if ( amp[i+1] < amp[i] )         // Only possibility of a peak
                    {
                        if ( grad == 1 && amp[i] > NOISE )
                        {
                            peaks.push_back(amp[i]);
                            index.push_back(i);
                        }
                        else if ( grad == 0 && amp[i] > NOISE )
                        {
                        }
                        grad = -1;
                    }
                    else if ( amp[i+1] == amp[i] )   // Check for start of a wide peak
                    {
                        if ( grad == 1 )
                        {
                            wideStart = i;
                            grad = 0;
                        }
                    }
                    else
                    {
                        grad = 1;
                    }
                }
                
                for (int num = 0; num < peaks.size(); num++)
                {
                    vector<double> betweenPeaksMin;
                    vector<double> betweenPeaksMax;
                    double min = 0.0, otherMin = 0.0;
                    
                    for (int abc = 0; abc < (index[num + 1] - index[num] - 1); abc++)
                    {
                        betweenPeaksMin.push_back(amp[abc + index[num] + 1]);
                        min = *min_element(betweenPeaksMin.begin(), betweenPeaksMin.end());
                    }
                    if (num + 2 < index.size())
                    {
                        for (int abc = 0; abc < (index[num + 2] - index[num + 1] - 1); abc++)
                        {
                            betweenPeaksMax.push_back(amp[abc + index[num + 1] + 1]);
                            otherMin = *min_element(betweenPeaksMax.begin(), betweenPeaksMax.end());
                        }
                    }
                    else
                    {
                        for (int abc = 0; abc < (30 - index[num + 1] - 1); abc++)
                        {
                            betweenPeaksMax.push_back(amp[abc + index[num + 1] + 1]);
                            otherMin = *min_element(betweenPeaksMax.begin(), betweenPeaksMax.end());
                        }
                    }
                    if ((peaks[num] - min)/(min) > 0.10 && (peaks[num] - otherMin)/(otherMin) > 0.10)
                    {
                        count++;
                    }
                }
                if (count >= 3)
                {
                    seizureCount++;
                }
            }
            outCountY = 0;
            
        }
        
        double portionOfScreen = 1.0*(seizureCount*32.0*32.0)/(341.0*256.0);
        if (portionOfScreen > 0.25)
        {
            seizureBoundaries[n/30][0] = n + 1;
            seizureBoundaries[n/30][1] = n + 30;
            cout << "Seizure at frames" << n + 1 << "to" << n + 30 << endl;
        }
        
        seizureCount = 0;
        n = n+30;
    }
    return seizureBoundaries;
    
    // Make this function only work for areas of pixels that take up more than 10% of the viewport
}

int main(int argc, const char * argv[])
{
    
    vector<vector<int > > randMatrix = seizureDetection(vidDir);
    
    int decision;
    cout << "Choose a filter method: 1. Color-shift 2. Change Contrast 3. Overlay 4. Blur" << endl;
    cin >> decision;
    double alpha;
    
    if( decision == 1)
    {
        for (int i = 0; i < randMatrix.size(); i++)
        {
            // makeFrameGradient(randMatrix[i][0], randMatrix[i][1], vidDir);
            if (randMatrix[i][0] != 0 && randMatrix[i][1] != 0)
            {
                cout << "Removing seizure at frames" << randMatrix[i][0] << "to" << randMatrix[i][1] << endl;
                makeFrameGradient(randMatrix[i][0], randMatrix[i][1], vidDir);
            }
            
        }
    }
    else if (decision == 2){
        cout << "Input an alpha value (0.0 - 1.0)" << endl;
        cin >> alpha;
        if(alpha <= 1.0 && alpha >= 0.0)
        {
            for (int i = 0; i < randMatrix.size(); i++)
            {
                // makeFrameGradient(randMatrix[i][0], randMatrix[i][1], vidDir);
                if (randMatrix[i][0] != 0 && randMatrix[i][1] != 0)
                {
                    cout << "Removing seizure at frames" << randMatrix[i][0] << "to" << randMatrix[i][1] << endl;
                    lowerContrast(randMatrix[i][0], randMatrix[i][1], vidDir, alpha);
                }
            }
        }
        else{
            cout << "Input a valid alpha and try again!";
        }
    }
    else if (decision == 3){
        cout << "Input an alpha value (0.0 - 1.0)" << endl;
        cin >> alpha;
        if (alpha <= 1.0 && alpha >= 0.0)
        {
            for (int i = 0; i < randMatrix.size(); i++)
            {
                // makeFrameGradient(randMatrix[i][0], randMatrix[i][1], vidDir);
                if (randMatrix[i][0] != 0 && randMatrix[i][1] != 0)
                {
                    cout << "Removing seizure at frames" << randMatrix[i][0] << "to" << randMatrix[i][1] << endl;
                    overlayFilter(randMatrix[i][0], randMatrix[i][1], vidDir, alpha);
                }
            }
        }
        else {
            cout << "Input a valid alpha and try again!";
        }
    }
    else if (decision == 4){
        cout << "Input an alpha value (minimum ,0.0)" << endl;
        cin >> alpha;
        if (alpha >= 0.0)
        {
            for (int i = 0; i < randMatrix.size(); i++)
            {
                // makeFrameGradient(randMatrix[i][0], randMatrix[i][1], vidDir);
                if (randMatrix[i][0] != 0 && randMatrix[i][1] != 0)
                {
                    cout << "Removing seizure at frames" << randMatrix[i][0] << "to" << randMatrix[i][1] << endl;
                    blurFilter(randMatrix[i][0], randMatrix[i][1], vidDir, alpha);
                }
            }
        }
        else {
            cout << "Input a valid alpha and try again!";
        }
    }
    else {
         cout << "Input a valid choice and try again!";
    }
    
    // makeFrameGradient(1, 30, vidDir);
    // TO PUT TOGETHER VIDEO USE: ffmpeg -framerate 60 -i %d.jpg video.mp4
    
}