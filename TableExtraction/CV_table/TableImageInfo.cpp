#include "TableImageInfo.h"

bool compareRects(NotedRect, NotedRect);

string TableImageInfo::rootPath = "";
//only admit to read when file already exists.
void TableImageInfo::readFromFile() {
	ifstream in(pathName + ".txt");
	if (!in) {
		cerr << "File " << pathName << " not exists!" << endl;
		return;
	}
	else {
		string filename;
		in >> filename;
		if (filename != pathName) {
			cout << "Warning: file name not matched" << endl;
			cout << "openpath: " << pathName << " filecontent: " << filename << endl;
		}
		int width, height;
		in >> width >> height;
		setImageWidth(width);
		setImageHeight(height);
		in >> width >> height;
		setTableWidth(width);
		setTableHeight(height);

		rects.clear();
		int num;
		int index;
		int x, y;
		in >> num;
		for (int i = 0; i < num; i++) {
			in >> index;
			in >> x >> y >> width >> height;
			NotedRect nr;
			nr.index = index;
			nr.rect = Rect(x, y, width, height);
			rects.push_back(nr);
		}
	}
}

void TableImageInfo::writeToFile() {
	ofstream out(pathName + ".txt");
	out << pathName << endl;
	out << imageWidth << " " << imageHeight << endl;
	out << tableWidth << " " << tableHeight << endl;
	out << rects.size() << endl;
	for (int i = 0; i < rects.size(); i++) {
		out << rects[i].index << " ";
		out << rects[i].rect.x << " " << rects[i].rect.y << " "
			<< rects[i].rect.width << " " << rects[i].rect.height << endl;
	}
	out.close();
}

void TableImageInfo::sortRects() {
	std::sort(rects.begin(), rects.end(), compareRects);
}

bool compareRects(NotedRect r1, NotedRect r2) {
	if (r1.rect.y < r2.rect.y) return true;
	else if (r1.rect.y > r2.rect.y) return false;
	else {
		if (r1.rect.x < r2.rect.x) return true;
		else if (r1.rect.x > r2.rect.x) return false;
		else {
			if (r1.rect.width > r2.rect.width) return true;
			else if (r1.rect.width < r2.rect.width) return false;
			else {
				if (r1.rect.height > r2.rect.height) return true;
				else return false;
			}
		}
	}
}

void TableImageInfo::generateTable() {
	cout << "Generating excel " << pathName << ".csv" << endl;
	sortRects();
	int smallWidth = getStandardWidth();
	int smallHeight = getStandardHeight();

	int excCols = getExcelCols(smallWidth);
	int excRows = getExcelRows(smallHeight);

	initExcelMat(excCols, excRows);

	for (int i = 0; i < rects.size(); i++) {
		Point pos = getPartPosition(rects[i].rect, smallWidth, smallHeight);
		//string text = content[rects[i].index];
		string text = to_string(rects[i].index);
		insertExcelMat(pos, text);
	}

	ofstream out(pathName + ".csv");
	out << pathName << "," << endl;
	for (int i = 0; i < excRows; i++) {
		for (int j = 0; j < excCols; j++) {
			out << getExcelContent(j, i) << ",";
		}
		out << endl;
	}
}

int TableImageInfo::getStandardHeight() {
	int minHeight = 65535;
	for (int i = 0; i < rects.size(); i++) {
		if (rects[i].rect.height < minHeight) {
			minHeight = rects[i].rect.height;
		}
	}
	return minHeight;
}

int TableImageInfo::getStandardWidth() {
	int minWidth = 65535;
	for (int i = 0; i < rects.size(); i++) {
		if (rects[i].rect.width < minWidth) {
			minWidth = rects[i].rect.width;
		}
	}
	return minWidth;
}

void TableImageInfo::initExcelMat(int cols, int rows) {
	for (int i = 0; i < rows; i++) {
		vector<string> temp;
		for (int j = 0; j < cols; j++)
			temp.push_back("");
		excelMat.push_back(temp);
	}
}
