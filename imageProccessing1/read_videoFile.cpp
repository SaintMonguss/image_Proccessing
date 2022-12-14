#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void put_string(Mat& frame, string text, Point pt, int value) {}

int main()
{
	VideoCapture capture;
	capture.open("../video_file.avi");
	CV_Assert(capture.isOpened());

	double frame_rate = capture.get(CAP_PROP_FPS);
	int delay = 1000 / frame_rate;
	int frame_cnt = 0;
	Mat frame;

	while (capture.read(frame))
	{
		if (waitKey(delay) >= 0) break;

		if (frame_cnt < 100);
		else if (frame_cnt < 200);