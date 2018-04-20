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

	initExcelTempMat(excCols, excRows);

	for (int i = 0; i < rects.size(); i++) {
		Point pos = getPartPosition(rects[i].rect, smallWidth, smallHeight);
		//string text = content[rects[i].index];
		string text = to_string(rects[i].index);
		insertExcelTempMat(pos, text);
	}

	clearUselessTiles();

	writeMatToExcelFile();
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

void TableImageInfo::initExcelTempMat(int cols, int rows) {
	excelTempMat.clear();
	for (int i = 0; i < rows; i++) {
		vector<string> temp;
		temp.clear();
		for (int j = 0; j < cols; j++)
			temp.push_back("");
		excelTempMat.push_back(temp);
	}
}

void TableImageInfo::clearUselessTiles() {
	excelMat.clear();
	vector<int> blankCols, blankRows;
	int rows = excelTempMat.size();
	int cols = excelTempMat[0].size();
	for (int i = 0; i < rows; i++) {
		if (isBlank(i, 0)) blankRows.push_back(i);
	}
	for (int i = 0; i < cols; i++) {
		if (isBlank(i, 1)) blankCols.push_back(i);
	}
	for (int i = 0; i < rows; i++) {
		if (isFound(blankRows, i)) continue;
		else {
			vector<string> temp;
			for (int j = 0; j < cols; j++) {
				if (isFound(blankCols, j)) continue;
				else
					temp.push_back(getExcelTempContent(j, i));
			}
			excelMat.push_back(temp);
		}
	}
}

bool TableImageInfo::isBlank(int line, int mode) {
	if (mode == 0) {
		int cols = excelTempMat[0].size();
		bool flag = true;
		for (int i = 0; i < cols; i++) {
			if (excelTempMat[line][i] != "") {
				flag = false;
				break;
			}
		}
		return flag;
	}
	else {
		int rows = excelTempMat.size();
		bool flag = true;
		for (int i = 0; i < rows; i++) {
			if (excelTempMat[i][line] != "") {
				flag = false;
				break;
			}
		}
		return flag;
	}
}

bool TableImageInfo::isFound(vector<int>& container, int coord) {
	int size = container.size();
	int flag = false;
	for (int i = 0; i < size; i++) {
		if (container[i] == coord) {
			flag = true;
			break;
		}
	}
	return flag;
}

void TableImageInfo::writeMatToExcelFile() {
	ofstream out(pathName + ".csv");
	out << pathName << "," << endl;
	int rows = excelMat.size();
	int cols = excelMat[0].size();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			out << getExcelContent(j, i) << ",";
		}
		out << endl;
	}
}