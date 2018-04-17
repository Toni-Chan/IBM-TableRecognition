#pragma once
#include <fstream>
#include <iostream>
#include <opencv2\core\core.hpp>

using namespace std;
using namespace cv;

//name used to be TableImageParts
//class to temporarily-store, output and readin an image's information
class TableImageInfo {
	string fileName;
	int imageWidth, imageHeight;
	int tableWidth, tableHeight;
	vector<Rect> rects;
public:
	TableImageInfo(string filename) :fileName(filename) {}
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

	void readFromFile();
	void writeToFile();
};
