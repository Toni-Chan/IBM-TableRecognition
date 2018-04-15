#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace std;
using namespace cv;

void GetRect(Mat thresh, Rect& rect);

int main() {
	Mat ori = imread("..\\mask.jpg", IMREAD_GRAYSCALE);
	namedWindow("show", WINDOW_AUTOSIZE);
	Mat thresh;
	adaptiveThreshold(~ori, thresh, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

	//dilate(thresh, thresh, Mat(), Point(-1, -1));

	Rect rect;
	GetRect(thresh, rect);
	cout << rect << endl;

	Mat another = imread("..\\123.jpg", IMREAD_GRAYSCALE);
	Mat another_thresh;
	adaptiveThreshold(~another, another_thresh, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

	Rect another_rect;
	GetRect(another_thresh, another_rect);
	cout << another_rect;

	Mat ori_table = thresh(rect);
	Mat another_table = another_thresh(another_rect);

	Mat resized_another;
	resize(another_table, resized_another, Size(rect.width, rect.height));

	Mat out;
	bitwise_xor(ori_table, resized_another, out);
	Mat resized_out;
	resize(out, resized_out, Size(out.cols / 2, out.rows / 2));

	//erode(out, out, Mat(), Point(-1, -1));
	//imshow("show", thresh);
	//imshow("show", resized_another);
	//imshow("show_ori", ori_table);
	//imshow("show", resized_out);
	imshow("show", resized_out);
	imshow("result", out);
	waitKey(0);
}

void GetRect(Mat thresh, Rect& rect) {
	Mat horizontial = thresh.clone();
	Mat vertical = thresh.clone();

	int scale = 20;
	int horizontial_size = horizontial.cols / scale;
	//get horizontial lines in morphology
	Mat horizontial_lines = getStructuringElement(MORPH_RECT, Size(horizontial_size, 1));
	//first erode and then dilate
	erode(horizontial, horizontial, horizontial_lines, Point(-1, -1));
	dilate(horizontial, horizontial, horizontial_lines, Point(-1, -1));

	int vertical_size = vertical.rows / scale;
	Mat vertical_lines = getStructuringElement(MORPH_RECT, Size(1, vertical_size));
	
	erode(vertical, vertical, vertical_lines, Point(-1, -1));
	dilate(vertical, vertical, vertical_lines, Point(-1, -1));

	//combine horizontial and vertical lines
	Mat combine_first = horizontial + vertical;
	Mat corners;
	bitwise_and(horizontial, vertical, corners);

	vector<vector<Point>> contours;
	findContours(combine_first, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	int max = 0; 
	for (int i = 0; i < contours.size(); i++ ) {
		if (contours[i].size() >= contours[max].size())
			max = i;
	}

	Mat poly;
	approxPolyDP(Mat(contours[max]), poly, 3, true);
	rect = boundingRect(poly);

}