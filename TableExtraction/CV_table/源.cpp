#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>

using namespace std;
using namespace cv;

int main() {
	Mat ori = imread("..\\31.jpg", IMREAD_GRAYSCALE);
	namedWindow("show", WINDOW_AUTOSIZE);

	//reverse the gray mat and thresh to get white scripts
	Mat thresh_reverse;
	adaptiveThreshold(~ori, thresh_reverse, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

	Mat horizontial = thresh_reverse.clone();
	Mat vertical = thresh_reverse.clone();

	int scale = 20;
	int horizontial_size = horizontial.cols / scale;
	//get horizontial lines in morphology
	Mat horizontial_lines = getStructuringElement(MORPH_RECT, Size(horizontial_size, 1));
	//first erode and then dilate
	erode(horizontial, horizontial, horizontial_lines, Point(-1, -1));
	dilate(horizontial, horizontial, horizontial_lines, Point(-1, -1));
	imshow("horizontial", horizontial);

	int vertical_size = vertical.rows / scale;
	Mat vertical_lines = getStructuringElement(MORPH_RECT, Size(1, vertical_size));

	erode(vertical, vertical, vertical_lines, Point(-1, -1));
	dilate(vertical, vertical, vertical_lines, Point(-1, -1));

	//combine horizontial and vertical lines
	Mat combine_first = horizontial + vertical;
	Mat corners;
	bitwise_and(horizontial, vertical, corners);

	vector<vector<Point>> contours;

	findContours(combine_first, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> contours_polynomials(contours.size());

	vector<Rect> boundRect(contours.size());
	vector<Mat> tables, rois;

	for (vector<Point> contour : contours) {
		double area = contourArea(contour);
		if (area < 100)
			continue;
		else {
			Mat out_temp;
			approxPolyDP(Mat(contour), out_temp, 3, true);
			contours_polynomials.push_back(out_temp);
			Rect rect_temp = boundingRect(Mat(out_temp));
			boundRect.push_back(rect_temp);

			Mat local_corners = corners(rect_temp);
			Mat roi = combine_first(rect_temp);

			vector<vector<Point>> corner_in_rect;
			findContours(local_corners, corner_in_rect, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

			if (corner_in_rect.size() <= 4)
				continue;
			tables.push_back(ori(rect_temp).clone());
			rois.push_back(roi);
			rectangle(ori, rect_temp, Scalar(0, 0, 0), 5, 8, 0);
		}
	}

	Mat resized_reverse, resized_first, resized_show;
	resize(thresh_reverse, resized_reverse, Size(1400, 800));
	resize(rois[0], resized_first, Size(1400, 800));
	resize(tables[0], resized_show, Size(1400, 800));

	imshow("show_reverse", resized_reverse);
	imshow("show_first", resized_first);
	imshow("show", resized_show);

	cout << boundRect.size() << endl;
	for (int i = 0; i < boundRect.size(); i++) {
		cout << boundRect[i] << endl;
	}
	cout << tables.size() << endl;

	//imwrite("..\\mask.jpg", ~thresh_reverse);
	//imshow("show_reverse", thresh_reverse);
	//imshow("show_first", rois[0]);
	//imshow("show", tables[0]);
	waitKey(0);
	return 0;
}