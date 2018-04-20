#include <stdio.h>
#include <stdlib.h>

int main() {
	system("TableExtraction.exe");
	printf("Image processing part completed\n");
	system("OCR.exe");
	printf("Content recognization completed\n");
	system("GenerateTable.exe");
	printf("Table generation completed\n");
}