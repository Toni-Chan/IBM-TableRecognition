#include "commons.h"
#include "TableImage.h"

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
		TableImage ti = TableImage(i, imageName);
		ti.startProcess();
	}
	//TableImage ti = TableImage(1001, "../mask.jpg");
	//ti.startProcess();
	return 0;
}