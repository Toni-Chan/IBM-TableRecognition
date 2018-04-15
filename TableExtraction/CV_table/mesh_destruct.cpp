#include <iostream> 
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace std;
using namespace cv;

#define LINE_BASE 20
#define HOUGH_VOTE 100

class Line {
public:

	Point startPoint;
	int length;
	Line(Point startPoint, int length): startPoint(startPoint), length(length){}
	Line(Rect rect, int hr) :startPoint(Point(rect.x, rect.y)) {
		if (hr == 0) length = rect.width;
		else if (hr == 1) length = rect.height;
	}
	~Line() {}
};

int main() {
	Mat ori_img = imread("..\\31.jpg", IMREAD_GRAYSCALE);
	namedWindow("Window", WINDOW_AUTOSIZE);
	cout << ori_img.cols << " " << ori_img.rows << endl;

	Mat reverse_img = ~ori_img;
	Mat threshed;
	adaptiveThreshold(reverse_img, threshed, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

	// get horizontial lines
	Mat horizon_lines = threshed.clone();
	int horizon_rect_size = ori_img.cols / LINE_BASE;
	Mat horizon_lines_kernel = getStructuringElement(MORPH_RECT, Size(horizon_rect_size, 1));
	erode(horizon_lines, horizon_lines, horizon_lines_kernel);
	dilate(horizon_lines, horizon_lines, horizon_lines_kernel);
	// get horizontial contours
	vector<vector<Point>> hor_contours;
	findContours(horizon_lines, hor_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	cout << hor_contours.size() << endl;

	vector<Line> hor_lines_para;
	for (int i = 0; i < hor_contours.size(); i++) {
		Mat temp(hor_contours[i]);
		Rect temp_rect = boundingRect(temp);
		hor_lines_para.push_back(Line(temp_rect, 0));
	}

	 for (int i = 0; i < hor_lines_para.size(); i++) {
	 	cout << hor_lines_para[i].startPoint<< " "<<hor_lines_para[i].length << endl;
	 }

	Mat vertical_lines = threshed.clone();
	int vertical_rect_size = ori_img.rows / LINE_BASE; 
	Mat vertical_lines_kernel = getStructuringElement(MORPH_RECT, Size(1, vertical_rect_size));

	erode(vertical_lines, vertical_lines, vertical_lines_kernel);
	dilate(vertical_lines, vertical_lines, vertical_lines_kernel);

	vector<vector<Point>> ver_contours;
	findContours(vertical_lines, ver_contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	cout << ver_contours.size() << endl;

	vector<Line> ver_lines_para;
	for (int i = 0; i < ver_contours.size(); i++) {
		Mat temp(ver_contours[i]);
		Rect temp_rect = boundingRect(temp);
		ver_lines_para.push_back(Line(temp_rect, 1));
	}
	for (int i = 0; i < ver_lines_para.size(); i++)
		cout << ver_lines_para[i].startPoint << " " << ver_lines_para[i].length << endl;

	Mat resized_output;
	resize(vertical_lines, resized_output, Size(ori_img.cols / 2, ori_img.rows / 2));

	imshow("Window", resized_output);
	waitKey(0);
}