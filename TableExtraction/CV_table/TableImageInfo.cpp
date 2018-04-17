#include "TableImageInfo.h"

//only admit to read when file already exists.
void TableImageInfo::readFromFile() {
	ifstream in(fileName + ".txt");
	if (!in) {
		cerr << "File " << fileName << " not exists!" << endl;
		return;
	}
	else {
		string filename;
		in >> filename;
		if (filename != fileName) {
			cout << "Warning: file name not matched" << endl;
			cout << "openpath: " << fileName << " filecontent: " << filename << endl;
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
	ofstream out(fileName + ".txt");
	out << fileName << endl;
	out << imageWidth << " " << imageHeight << endl;
	out << tableWidth << " " << tableHeight << endl;
	out << rects.size() << endl;
	for (int i = 0; i < rects.size(); i++) {
		cout << rects[i].x << " " << rects[i].y << " "
			<< rects[i].width << " " << rects[i].height << endl;
	}
	out.close();
}