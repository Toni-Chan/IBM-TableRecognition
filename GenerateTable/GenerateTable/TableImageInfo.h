#pragma once
#include "commons.h"

using namespace std;
using namespace cv;

struct NotedRect {
	int index;
	Rect rect;
};
typedef struct NotedRect NotedRect;

//name used to be TableImageParts
//class to temporarily-store, output and readin an image's information
class TableImageInfo {
	string pathName;
	int imageWidth, imageHeight;
	int tableWidth, tableHeight;
	vector<NotedRect> rects;
	vector<vector<string>> excelMat;
	vector<string> content; // waiting for OCR part

	int getStandardWidth();
	int getStandardHeight();

	int getExcelCols(int unitWidth) { return tableWidth / unitWidth; }
	int getExcelRows(int unitHeight) { return tableHeight / unitHeight; }

	void initExcelMat(int cols, int rows);

	Point getPartPosition(Rect rect, int smallWidth, int smallHeight) { 
		int row = rect.y / smallHeight; int col = rect.x / smallWidth; return Point(col, row);
	}

	void insertExcelMat(Point pos, string text) { excelMat[pos.y][pos.x] = text; }
	string getExcelContent(int col, int row) { return excelMat[row][col]; }

	void sortRects();
public:
	static string rootPath;
	static void setRootPath(string name) { rootPath = name; _mkdir(rootPath.c_str()); }

	TableImageInfo(string filename) { 
		size_t posDot = filename.find_last_of("."); 
		size_t posSlide = filename.find_last_of("/");
		pathName = rootPath + filename.substr(posSlide, posDot - posSlide) + "_" + filename.substr(posDot + 1);
		//cout << pathName << endl;
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
		else return rects[index].rect;
	}
	void setImageWidth(int width) { this->imageWidth = width; }
	void setImageHeight(int height) { this->imageHeight = height; }
	void setTableWidth(int width) { this->tableWidth = width; }
	void setTableHeight(int height) { this->tableHeight = height; }
	void insertRect(int index, Rect temp) { NotedRect nr; nr.index = index; nr.rect = temp; rects.push_back(nr); }

	bool readImage(string path, Mat& img);
	bool writeImagePart(string name, Mat&);

	void readFromFile();
	void writeToFile();

	void generateTable();
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
