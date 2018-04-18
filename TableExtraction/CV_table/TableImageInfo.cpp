#include "TableImageInfo.h"

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
		int x, y;
		in >> num;
		for (int i = 0; i < num; i++) {
			cin >> x >> y >> width >> height;
			rects.push_back(Rect(x, y, width, height));
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
		out << rects[i].x << " " << rects[i].y << " "
			<< rects[i].width << " " << rects[i].height << endl;
	}
	out.close();
}