#pragma once
#include "commons.h"

// struct to save rectangle information and corresponding index
// index is used for mapping to content
// vector<NotedRect> rect : rect[i].index -> content[index]
struct NotedRect {
	int index;
	Rect rect;
};
typedef struct NotedRect NotedRect;

//name used to be TableImageParts
//class to temporarily-store, output and readin an image's information
class TableImageInfo {
	// a pathName parsed from image name, changing dot'.' to slide'_'.
	string pathName;
	// basic information of an image and before and after processed.
	int imageWidth, imageHeight;
	int tableWidth, tableHeight;
	// container to store rectangles' information
	vector<NotedRect> rects;
	// temporary container to store contents of excel file
	vector<vector<string>> excelMat, excelTempMat;
	// ocr results.
	vector<string> content; /// waiting for OCR part, also need input method implementation

	// get the smallest piece of a cutted mat, using as primial of excel
	int getStandardWidth();
	int getStandardHeight();
	// get generating mat's rows and columns to initialize temporary mat
	int getExcelCols(int unitWidth) { return tableWidth / unitWidth; }
	int getExcelRows(int unitHeight) { return tableHeight / unitHeight; }
	// initializer of excelTempMat
	void initExcelTempMat(int cols, int rows);
	// fundamental calculation of rectangle's position
	Point getPartPosition(Rect rect, int smallWidth, int smallHeight) { 
		int row = rect.y / smallHeight; int col = rect.x / smallWidth; return Point(col, row);
	}
	// function to modify content of excelTempMat [replaceable!]
	void insertExcelTempMat(Point pos, string text) { excelTempMat[pos.y][pos.x] = text; }
	// function to get content of excelTempMat
	string getExcelTempContent(int col, int row) { return excelTempMat[row][col]; }
	// function to get content of excelMat, using for file stream output
	string getExcelContent(int col, int row) { return excelMat[row][col]; }
	// sorting rectangles, preorder: y>>x>>width>>height
	void sortRects();
	// clear useless tiles (empty rows and columns) in Mat
	void clearUselessTiles();
	// inner function to check whether a row/col of excelTempMat is all blank
	// 0: row mode
	// 1: column mode
	bool isBlank(int, int mode);
	// inner function to check whether a coord is in blankRow/blankCol container
	bool isFound(vector<int>&, int);
	// write Mat to excel output
	void writeMatToExcelFile();
public:
	// root path of this class, default should be "./output", 
	// path name is append after this
	static string rootPath;
	static void setRootPath(string name) { rootPath = name; _mkdir(rootPath.c_str()); }

	// the Constructor will parses image file name to directory name
	TableImageInfo(string filename) { 
		size_t posDot = filename.find_last_of("."); 
		size_t posSlide = filename.find_last_of("/");
		pathName = rootPath + filename.substr(posSlide, posDot - posSlide) + "_" + filename.substr(posDot + 1);
		//cout << pathName << endl;
		_mkdir(pathName.c_str());
	}
	// getters
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
	// setters
	void setImageWidth(int width) { this->imageWidth = width; }
	void setImageHeight(int height) { this->imageHeight = height; }
	void setTableWidth(int width) { this->tableWidth = width; }
	void setTableHeight(int height) { this->tableHeight = height; }
	void insertRect(int index, Rect temp) { NotedRect nr; nr.index = index; nr.rect = temp; rects.push_back(nr); }
	// opencv function : read image from image path
	bool readImage(string path, Mat& img);
	// opencv function : write cutted image to certain path
	bool writeImagePart(string name, Mat&);
	// read and write serialized information of THIS class object
	void readFromFile();
	void writeToFile();
	// function to generate excel file [.csv]
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
