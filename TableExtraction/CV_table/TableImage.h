#pragma once
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <direct.h>

using namespace std;
using namespace cv;

bool minHor(Vec3i, Vec3i);	// Compare function in sorting for horizontial lines
bool minVer(Vec3i, Vec3i);	// Compare function in sorting for vertical lines

// class of Table Image with Borders
class TableImage {
	// ID for file managing
	int id;		
	// input path
	String filename;	
	// original image, only used for backup
	Mat oriImg;			
	// processed image, is changed in size and channels while processing
	Mat proImg;			
	// a Daemon image of proImg, is changed simultaneously as proImg, used for final cutting pieces
	Mat cutImg;			
	// flag indicates the validity of object
	bool useable = true;	
	// flag indicates whether the imshow window is opened
	bool showing = false;	
	// struct to store horizontial and vertical lines
	vector<Vec3i> lineHor, lineVer;	
	// if the image need to be affined(spinned), record its value
	double alignDegree;		

	// Inner check function to get validity of object
	bool isValid();		
	// Obsoleted, used to be testing fuction
	void show(String name, Mat& pic, int scale);	
	/*	Sort container of lines, 
	*	Type 0: horizontial mode
	*	Type 1: vertical mode
	*/
	void sortLines(vector<Vec3i>&, int Type);	
	/*	Merge lines that are, or seems to be one line, 
	*	Return a container including merged lines
	*	Type 0: horizontial mode
	*	Type 1: vertical mode
	*/
	vector<Vec3i> mergeLines(vector<Vec3i>&, int Type);
	/** Return true if the new line should be merged into the group
	*	group is in format [sum of x][sum of y][maxlength of line]
	*	count is num of members in group
	*	newLine is in format [x][y][length]
	*	Type 0: horizontial mode
	*	Type 1: vertical mode
	*/
	bool relevent(Vec3i group, int count, Vec3i newLine, int Type);
	/*	Return true if the four lines can outline a rectangle
	*	first two parameters h1 and h2 are two horizontial lines
	*	last two parameters v1 and v2 are two vertical lines
	*/
	bool isRect(Vec3i, Vec3i, Vec3i, Vec3i);
	/*	Return tre if points in corner is a tapezoid
	*	corner is a container of Point of size 4
	*	if the points are corners of tapezoid, the function will reorder the points to match rectangle
	*/
	bool isTapezoid(vector<Point2f>&);

public:
	// Image is cutted into pieces by rectanles
	vector<Rect> cuttingRects;
	TableImage(int id, String name) :id(id), filename(name) {
		readImage();
	}
	// Input image through path, will print error message while encounter failure
	void readImage();
	// Workflow of table processing
	void startProcess();
	// backup original image
	void cloneOriImage();
	// using mean kernel to eliminate noise
	void eliminateNoise();
	// cut out the larges rectangle in image, then only process inner message
	void selectLargestBox();
	// test whether the image need to be affined
	void makeAlignment();
	// findout horizontial and vertical lines in the image
	void distinguishStructures();
	// cut up the table
	void cutImage();

};

inline void TableImage::readImage() {
	oriImg = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	if (oriImg.empty()) {
		cerr << "ERROR::IMAGE::READ" << endl;
		useable = false;
	}
}

inline bool TableImage::isValid() {
	return useable;
}

inline void TableImage::show(String name, Mat& pic, int scale = 1) {
	if (!showing) {
		namedWindow(filename, WINDOW_AUTOSIZE);
		showing = true;
	}
	if (scale != 1) {
		Mat resized;
		resize(pic, resized, Size(pic.cols / scale, pic.rows / scale));
		imshow(name, resized);
	}
	else
		imshow(name, pic);
}
