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