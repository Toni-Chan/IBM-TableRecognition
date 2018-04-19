#include "commons.h"
#include "TableImageInfo.h"

using namespace std;

int main(int argc, char** argv) {
	string filename;
	if (argc > 1) filename = argv[1];
	else filename = "./images.txt";

	ifstream file(filename);
	if (!file) {
		cerr << "File " << filename << " not exists, exiting." << endl;
	}

	string directory;
	if (argc > 2) directory = argv[2];
	else directory = "./output";

	TableImageInfo::setRootPath(directory);

	int numOfImages;
	file >> numOfImages;
	for (int i = 0; i < numOfImages; i++) {
		string imageName;
		file >> imageName;
		TableImageInfo tii(imageName);
		tii.readFromFile();
		//cout << tii.getImageWidth() << " " << tii.getImageWidth() << " " << tii.getNumberOfRectangle() << " " << endl;
		//for (int j = 0; j < tii.getNumberOfRectangle(); j++)
		//	cout << tii.getRect(j) << endl;
		tii.generateTable();
	}
	//TableImage ti = TableImage(1001, "../mask.jpg");
	//ti.startProcess();
	return 0;
}