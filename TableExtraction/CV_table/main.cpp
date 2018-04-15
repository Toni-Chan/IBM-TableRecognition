#include "TableImage.h"

int main() {
	TableImage ti = TableImage(1001, "../mask.jpg");
	ti.startProcess();
	return 0;
}