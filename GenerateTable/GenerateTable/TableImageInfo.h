#pragma once
#include "commons.h"

using namespace std;
using namespace cv;

//name used to be TableImageParts
//class to temporarily-store, output and readin an image's information
class TableImageInfo {
	string pathName;
	int imageWidth, imageHeight;
	int tableWidth, tableHeight;
	vector<Rect> rects;
public:
	static string rootPath;
	static void setRootPath(string name) { rootPath = name; _mkdir(rootPath.c_str()); }

	TableImageInfo(string filename) { 
		size_t posDot = filename.find_last_of("."); 
		size_t posSlide = filename.find_last_of("/");
		pathName = rootPath + filename.substr(posSlide, posDot - posSlide) + "_" + filename.substr(posDot + 1);
		cout << pathName << endl;
		_mkdir(pathName.c_str());
	}

	int getImageWidth() { return imageWidth; }
	int getImageHeight() { return imageHeight; }
	int getTableWidth() { return tableWidth; }
	int getTableHeight() { return tableHeight; }
	int getNumberOfRectangle() { return rects.size(); }
	Rect getRect(int index) {
		if (index >= rects.size()) {
			cerr << "ERROR::TABLEIMAGEINFO::INDEX_OUT_OF_BOUND" << endl;
			return Rect();
		}
		else return rects[index];
	}
	void setImageWidth(int width) { this->imageWidth = width; }
	void setImageHeight(int height) { this->imageHeight = height; }
	void setTableWidth(int width) { this->tableWidth = width; }
	void setTableHeight(int height) { this->tableHeight = height; }
	void insertRect(Rect temp) { rects.push_back(temp); }

	bool readImage(string path, Mat& img);
	bool writeImagePart(string name, Mat&);

	void readFromFile();
	void writeToFile();
};

inline bool TableImageInfo::readImage(string path, Mat& img) {
	img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
	if (img.empty()) {
		cerr << "ERROR::TABLEIMAGEINFO::IMAGE_READ" << endl;
		return false;
	}
	return true;
}

inline bool TableImageInfo::writeImagePart(string name, Mat& img) {
	return imwrite(pathName + "/" + name + ".jpg", img);
}
