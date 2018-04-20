#pragma once
#include "commons.h"
#include "TableImageInfo.h"

bool minHor(Vec3i, Vec3i);	// Compare function in sorting for horizontial lines
bool minVer(Vec3i, Vec3i);	// Compare function in sorting for vertical lines

// class of Table Image with Borders
class TableImage {
	// ID for file managing
	int id;		
	// input path
	String filename;
	// struct to store image info
	TableImageInfo info;
	// make the center of processed image in processing
	Point center;
	// original image, only used for backup
	Mat oriImg;			
	// processed image, is changed in size and channels while processing
	Mat proImg;			
	// a Daemon image of proImg, is changed simultaneously as proImg, used for final cutting pieces
	Mat cutImg;			
	// flag indicates whether the imshow window is opened
	bool showing = false;	
	// struct to store horizontial and vertical lines
	vector<Vec3i> lineHor, lineVer;	
	// if the image need to be affined(spinned), record its value
	double alignDegree;	

	// Initializer of info
	void initInfo();
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
	bool isTapezoid(vector<Point2f>&, Rect);

public:
	TableImage(int id, String name) :id(id), filename(name), info(name) {
		info.readImage(name, oriImg);
		initInfo();
	}
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

inline bool TableImage::isValid() {
	return !oriImg.empty();
}

inline void TableImage::show(String name, Mat& pic, int scale = 1) {
	if (!showing) {
		namedWindow(filename, WINDOW_AUTOSIZE);
		showing = true;
	}
	//if (scale != 1) {
	//	Mat resized;
	//	resize(pic, resized, Size(pic.cols / scale, pic.rows / scale));
	//	imshow(name, resized);
	//}
	//else
	//	imshow(name, pic);
	Mat resized;
	resize(pic, resized, Size(600, 600 * pic.rows / pic.cols));
	imshow(name, resized);

}

inline void TableImage::initInfo() {
	if (isValid()) {
		info.setImageWidth(oriImg.cols);
		info.setImageHeight(oriImg.rows);
	}
	else {
		return;
	}
}
