#include "TableImage.h"
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
	string filename;
	if (argc > 1) filename = argv[1];
	else filename = ".\\images.txt";

	ifstream file(filename);
	if (!file) {
		cerr << "File " << filename << " not exists, exiting." << endl;
	}

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