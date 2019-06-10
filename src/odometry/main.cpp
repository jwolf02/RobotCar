#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

void featureDetection(const Mat &img_1, std::vector<Point2f>& points1)	{
    std::vector<KeyPoint> keypoints_1;
    int fast_threshold = 20;
    bool nonmaxSuppression = true;
    FAST(img_1, keypoints_1, fast_threshold, nonmaxSuppression);
    KeyPoint::convert(keypoints_1, points1, std::vector<int>());
}

void featureTracking(const Mat &img_1, const Mat &img_2, std::vector<Point2f>& points1, std::vector<Point2f>& points2, std::vector<uchar>& status)	{

//this function automatically gets rid of points for which tracking fails

    std::vector<float> err;
    Size winSize=Size(21,21);
    TermCriteria termcrit=TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01);

    calcOpticalFlowPyrLK(img_1, img_2, points1, points2, status, err, winSize, 3, termcrit, 0, 0.001);

    //getting rid of points for which the KLT tracking failed or those who have gone outside the frame
    int indexCorrection = 0;
    for( int i=0; i<status.size(); i++)
    {  Point2f pt = points2.at(i- indexCorrection);
        if ((status.at(i) == 0)||(pt.x<0)||(pt.y<0))	{
            if((pt.x<0)||(pt.y<0))	{
                status.at(i) = 0;
            }
            points1.erase (points1.begin() + i - indexCorrection);
            points2.erase (points2.begin() + i - indexCorrection);
            indexCorrection++;
        }

    }

}

int main(int argc, const char *argv[], const char *env[]) {

}