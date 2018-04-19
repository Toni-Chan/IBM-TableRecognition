#include "TableImage.h"

void TableImage::startProcess() {
	if (!isValid()) {
		cerr << "ERROR::IMAGE::NOT_VALID" << endl;
		return;
	}
	cout << "Processing image: " << filename << endl;
	//workflow of process
	cloneOriImage();
	eliminateNoise();
	makeAlignment();
	selectLargestBox();
	distinguishStructures();
	cutImage();
	cout << "Image process complete" << endl;
}

void TableImage::cloneOriImage() {
	//cout << "cloning image..." << endl;
	cutImg = oriImg.clone();
	proImg = ~oriImg.clone();
	center = Point(oriImg.cols / 2, oriImg.rows / 2);
	//cout << "clone complete" << endl;
}

void TableImage::eliminateNoise() {
	//cout << "eliminating noise" << endl;
	Mat temp = proImg.clone();

	// blocksize and C
	adaptiveThreshold(temp, proImg, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY,	9, -10);

	Mat kernelD = getStructuringElement(MORPH_DILATE, Size(2, 2));
	Mat kernelE = getStructuringElement(MORPH_ERODE, Size(2, 2));

	dilate(proImg, proImg, kernelD);
	erode(proImg, proImg, kernelE);

	// show("oriGray", proImg);
	//cout << "eliminate complete" << endl;
}

void TableImage::makeAlignment() {
	//cout << "making rotation to align image" << endl;
	// GRAY_THRESH and HOUGH_VOTE
	int GRAY_THRESH = 150;
	int HOUGH_VOTE = 100;
	Mat padded;
	int dftWidth = getOptimalDFTSize(proImg.rows);
	int dftHeight = getOptimalDFTSize(proImg.cols);
	copyMakeBorder(proImg, padded, 0, dftWidth - proImg.rows, 0, dftHeight - proImg.cols, BORDER_CONSTANT, Scalar::all(0));
	copyMakeBorder(cutImg, padded, 0, dftWidth - proImg.rows, 0, dftHeight - proImg.cols, BORDER_CONSTANT, Scalar::all(0));

	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat combinedImg;
	merge(planes, 2, combinedImg);

	dft(combinedImg, combinedImg);

	split(combinedImg, planes);
	magnitude(planes[0], planes[1], planes[0]);

	Mat magMat = planes[0];
	magMat += Scalar::all(1);
	log(magMat, magMat);

	magMat = magMat(Rect(0, 0, magMat.cols & -2, magMat.rows & -2));

	int cx = magMat.cols / 2;
	int cy = magMat.rows / 2;
	// magnify signal
	Mat q0(magMat, Rect(0, 0, cx, cy));
	Mat q1(magMat, Rect(0, cy, cx, cy));
	Mat q2(magMat, Rect(cx, cy, cx, cy));
	Mat q3(magMat, Rect(cx, 0, cx, cy));

	Mat tmp;
	q0.copyTo(tmp);
	q2.copyTo(q0);
	tmp.copyTo(q2);

	q1.copyTo(tmp);
	q3.copyTo(q1);
	tmp.copyTo(q3);

	normalize(magMat, magMat, 0, 1, CV_MINMAX);
	Mat magImg(magMat.size(), CV_8UC1);
	magMat.convertTo(magImg, CV_8UC1, 255, 0);

	threshold(magImg, magImg, GRAY_THRESH, 255, CV_THRESH_BINARY);
	// show("magImg", magImg, 4);

	vector<Vec2f> lines;
	HoughLines(magImg, lines, 1, (double)CV_PI / 180, HOUGH_VOTE, 0, 0);

	double angle = 0;
	double angleThresh = 45.0 * (double)CV_PI / 180.0;
	double HALF_PI = (double)CV_PI / 90.0;
	for (int i = 0; i < lines.size(); i++) {
		double theta = lines[i][1];
		// //cout << "theta: " << theta * 180.0 / CV_PI << endl;
		// only when theta in (0,45) or (135, 180) will apply
		if ((theta > 0 && theta < angleThresh) || (theta < CV_PI && theta > CV_PI - angleThresh)) {
			angle = theta;
			break;
		}
		else
			continue;
	}
	if (angle != HALF_PI) {
		angle = atan(proImg.rows * tan(angle) / proImg.cols);
	}
	
	double angleD = angle * 180.0 / (double)CV_PI;
	alignDegree = angleD;
	//cout << "Alignment Degree:" << alignDegree << endl;
	if (abs(angleD) > 0.6) {
		Point center = Point(proImg.cols / 2, proImg.rows / 2);
		Mat rotMat = getRotationMatrix2D(center, angleD, 1.0);
		Mat dstImg = Mat::ones(proImg.size(), CV_8UC3);
		warpAffine(proImg, dstImg, rotMat, proImg.size(), 1, 0, Scalar(255, 255, 255));

		angleD = abs(angleD);
		double tanAngleD = tan(angleD*CV_PI / 180.0);
		int subRectHeight = (proImg.rows * tanAngleD - proImg.cols * tanAngleD * tanAngleD) / (1 - tanAngleD * tanAngleD) / sin(angleD * CV_PI / 180.0);
		int subRectWidth = subRectHeight * proImg.cols / proImg.rows;

		Rect rect = Rect(center.x - subRectWidth / 2, center.y - subRectHeight / 2, subRectWidth, subRectHeight);
		center = Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
		proImg = Mat(dstImg, rect);
		// show("rotation", proImg, 4);

		dstImg = Mat::ones(cutImg.size(), CV_8UC3);
		warpAffine(cutImg, dstImg, rotMat, cutImg.size(), 1, 0, Scalar(255, 255, 255));
		cutImg = Mat(dstImg, rect);
		// show("rotation_ori", cutImg);
	}
	//cout << "alignment complete" << endl;
}

void TableImage::selectLargestBox() {
	//cout << "largest box calibration" << endl;
	int OUT_BORDER = (oriImg.cols > oriImg.rows) ? oriImg.cols / 200 : oriImg.rows / 200;
	Mat temp = proImg.clone();
	Mat kernelD = getStructuringElement(MORPH_DILATE, Size(2, 2));
	Mat kernelE = getStructuringElement(MORPH_ERODE, Size(2, 2));
	dilate(temp, temp, kernelD);
	erode(temp, temp, kernelE);

	vector<vector<Point>> contours;
	findContours(temp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int maxLength = 0;
	int max = -1;
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > maxLength) {
			maxLength = contours[i].size();
			max = i;
		}
	}

	vector<Point> largestContour = contours[max];
	Rect largeRect = boundingRect(largestContour);
	Rect finalRect = Rect(largeRect.x - OUT_BORDER, largeRect.y - OUT_BORDER,
		largeRect.width + 2 * OUT_BORDER, largeRect.height + 2 * OUT_BORDER);
	center = Point(finalRect.x + finalRect.width / 2, finalRect.y + finalRect.height / 2);

	vector<vector<Point>> oneLargeContour;
	oneLargeContour.push_back(largestContour);
	temp = Mat(Size(proImg.cols, proImg.rows), CV_8UC1);
	drawContours(temp, oneLargeContour, -1, Scalar(255, 255, 255));
	vector<Point2f> corners;

	kernelD = getStructuringElement(MORPH_DILATE, Size(3, 3));
	dilate(temp, temp, kernelD);
	goodFeaturesToTrack(temp, corners, 4, 0.05, 30, noArray(), 9);

	////cout << corners.size() << endl;
	//for (int i = 0; i < corners.size(); i++) {
	//	//cout << corners[i] << endl;
	//	circle(temp, corners[i], 4, Scalar(255, 255, 255), 2);
	//}
	// show("select", temp, 2);
	//Mat resized;
	//resize(temp, resized, Size(temp.cols / 4, temp.rows / 4));
	//show("select", resized);
	//waitKey(0);

	if (isTapezoid(corners)) {
		//cout << "is Tapezoid!" << endl;
		vector<Point2f> rectCorners;
		rectCorners.push_back(Point2f(largeRect.x, largeRect.y));
		rectCorners.push_back(Point2f(largeRect.x + largeRect.width, largeRect.y));
		rectCorners.push_back(Point2f(largeRect.x, largeRect.y + largeRect.height));
		rectCorners.push_back(Point2f(largeRect.x + largeRect.width, largeRect.y + largeRect.height));
		//for (int i = 0; i < corners.size(); i++) {
		//	//cout << corners[i] << endl;
		//}
		Mat perspTrans = getPerspectiveTransform(corners, rectCorners);
		Mat dst;
		warpPerspective(proImg, proImg, perspTrans, Size(proImg.cols, proImg.rows), INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
		warpPerspective(cutImg, cutImg, perspTrans, Size(cutImg.cols, cutImg.rows), INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));
		// show("perspective", proImg);
		////cout << perspTrans << endl;
	}

	Mat titleImg = cutImg.clone();
	Mat onetemp = Mat(Size(cutImg.cols, cutImg.rows), oriImg.channels(), Scalar(255,255,255));
	onetemp(largeRect).copyTo(titleImg(largeRect));
	//show("titleImg", titleImg, 4);
	//waitKey(0);
	info.writeImagePart("title", titleImg);

	proImg = Mat(proImg, finalRect);
	cutImg = Mat(cutImg, finalRect);

	info.setTableHeight(finalRect.height);
	info.setTableWidth(finalRect.width);
	//show("after subMat", proImg,4);
	//show("first cut", cutImg,4);
	//waitKey(0);
	//cout << "largest box pinpointed" << endl;
}

void TableImage::distinguishStructures() {
	//cout << "distinguishing linear structures" << endl;
	Mat horizontialLines = proImg.clone();
	//LINE_BASE
	int LINE_BASE = 30;
	int horizontialRectSize = proImg.cols / LINE_BASE;
	Mat horizontialLineKernel = getStructuringElement(MORPH_RECT, Size(horizontialRectSize, 1));
	erode(horizontialLines, horizontialLines, horizontialLineKernel);
	dilate(horizontialLines, horizontialLines, horizontialLineKernel);
	// show("horizontiallines", horizontialLines);

	vector<vector<Point>> horContours;
	findContours(horizontialLines, horContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//vector<Vec3i> lineHor;
	for (int i = 0; i < horContours.size(); i++) {
		Mat temp(horContours[i]);
		Rect temp_rect = boundingRect(temp);
		lineHor.push_back(Vec3i(temp_rect.x, temp_rect.y + temp_rect.height / 2, temp_rect.width));
	}

	Mat verticalLines = proImg.clone();
	int verticalRectSize = proImg.cols / LINE_BASE;
	Mat verticalLineKernel = getStructuringElement(MORPH_RECT, Size(1, verticalRectSize));
	erode(verticalLines, verticalLines, verticalLineKernel);
	dilate(verticalLines, verticalLines, verticalLineKernel);
	// show("vertical lines", verticalLines);

	vector<vector<Point>> verContours;
	findContours(verticalLines, verContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//vector<Vec3i> lineVer;
	for (int i = 0; i < verContours.size(); i++) {
		Mat temp(verContours[i]);
		Rect temp_rect = boundingRect(temp);
		lineVer.push_back(Vec3i(temp_rect.x + temp_rect.width / 2, temp_rect.y, temp_rect.height));
	}
	//vector<Vec3i> lineHor, lineVer;
	//int HOUGH_VOTE = 500;
	//// HOUGH_VOTE minLength maxGap
	//vector<Vec4i> lines;
	//HoughLinesP(proImg, lines, 1, CV_PI / 180, HOUGH_VOTE, 100, 20);

	//for (size_t i = 0; i < lines.size(); i++)
	//{
	//	Point p1 = Point(lines[i][0], lines[i][1]);
	//	Point p2 = Point(lines[i][2], lines[i][3]);
	//	if (abs(p1.y - p2.y) <= 5) {
	//		if (p1.x < p2.x)
	//			lineHor.push_back(Vec3i(p1.x, p1.y, p2.x - p1.x));
	//		else
	//			lineHor.push_back(Vec3i(p2.x, p2.y, p1.x - p2.x));
	//	}
	//	else if (abs(p1.x - p2.x) <= 10) {
	//		if (p1.y < p2.y)
	//			lineVer.push_back(Vec3i(p1.x, p1.y, p2.y - p1.y));
	//		else
	//			lineVer.push_back(Vec3i(p2.x, p2.y, p1.y - p2.y));
	//	}
	//}

	sortLines(lineHor, 0);
	sortLines(lineVer, 1);

	//lineHor = mergeLines(lineHor, 0);
	//lineVer = mergeLines(lineVer, 1);

	Mat lineResult = Mat(Size(proImg.cols, proImg.rows), CV_8UC1);
	////cout << lineHor.size() << endl;
	for (size_t i = 0; i < lineHor.size(); i++) {
		////cout << lineHor[i][0] << " " << lineHor[i][1] << " " << lineHor[i][2] << endl;
		line(lineResult, Point(lineHor[i][0], lineHor[i][1]), Point(lineHor[i][0] + lineHor[i][2], lineHor[i][1]), Scalar(255, 255, 255), 3);
	}
	////cout << lineVer.size() << endl;
	for (size_t i = 0; i < lineVer.size(); i++) {
		////cout << lineVer[i][0] << " " << lineVer[i][1] << " " << lineVer[i][2] << endl;
		line(lineResult, Point(lineVer[i][0], lineVer[i][1]), Point(lineVer[i][0] , lineVer[i][1] + lineVer[i][2]), Scalar(255, 255, 255), 3);
	}
	// show("lineResult", lineResult, 4);
	// waitKey(0);
	//cout << "structure finding complete" << endl;
}

void TableImage::cutImage() {
	cout << "generating process result ... " << endl;
	//_mkdir("../output");
	//String dir = "../output/" + to_string(id) + "/";
	//cout << "writing pieces into " << dir << endl;
	//_mkdir(dir.c_str());
	int count = 0;
	for (int i = 0; i < lineHor.size() - 1; i++) {
		for (int j = i + 1; j < lineHor.size(); j++) {
			for (int k = 0; k < lineVer.size() - 1; k++) {
				for (int l = k + 1; l < lineVer.size(); l++) {
					if (isRect(lineHor[i], lineHor[j], lineVer[k], lineVer[l]))
					{
						Rect tmp = Rect(lineVer[k][0], lineHor[i][1], lineVer[l][0] - lineVer[k][0], lineHor[j][1] - lineHor[i][1]);
						bool flag = true;
						for (int ii = i + 1; ii < j; ii++)
							if (isRect(lineHor[i], lineHor[ii], lineVer[k], lineVer[l]) || isRect(lineHor[ii], lineHor[j], lineVer[k], lineVer[l])) {
								flag = false;
								break;
							}
						if (!flag) continue;
						flag = true;
						for (int kk = k + 1; kk < l; kk++)
							if (isRect(lineHor[i], lineHor[j], lineVer[k], lineVer[kk]) || isRect(lineHor[i], lineHor[j], lineVer[kk], lineVer[l])) {
								flag = false;
								break;
							}
						if (!flag) continue;
						Mat temp = Mat(cutImg, tmp);
						info.insertRect(count, tmp);
						info.writeImagePart(to_string(count), temp);
						count++;
					}
				}
			}
		}
	}
	//cout << "count" << count << endl;
	//cout << "cutting image complete" << endl;
	info.writeToFile();
}

bool minHor(Vec3i a, Vec3i b) {
	if (a[1] < b[1]) return true;
	else if (a[1] > b[1]) return false;
	else {
		if (a[0] < b[0]) return true;
		else return false;
	}
}

bool minVer(Vec3i a, Vec3i b) {
	if (a[0] < b[0]) return true;
	else if (a[0] > b[0]) return false;
	else {
		if (a[1] < b[1]) return true;
		else return false;
	}
}

void TableImage::sortLines(vector<Vec3i>& lines, int Type) {
	if (Type == 0) {
		std::sort(lines.begin(), lines.end(), minHor);
	}
	else {
		std::sort(lines.begin(), lines.end(), minVer);
	}
}

vector<Vec3i> TableImage::mergeLines(vector<Vec3i>& lines, int Type) {
	// MIN_GAP
	int MIN_GAP = (oriImg.cols > oriImg.rows) ? oriImg.cols / 150 : oriImg.rows / 150;
	vector<Vec3i> ret;

	Vec3i sum(lines[0]);
	int count = 1;
	for (int i = 1; i < lines.size(); i++) {
		if (relevent(sum, count, lines[i], Type)) {
			if (abs(lines[i][0] - sum[0] / count) <= MIN_GAP && abs(lines[i][1] - sum[1] / count) <= MIN_GAP) {
				sum[0] += lines[i][0];
				sum[1] += lines[i][1];
				sum[2] = sum[2] > lines[i][2] ? sum[2] : lines[i][2];
				count++;
			}
			else {
				sum[2] = lines[i][Type] + lines[i][2] - sum[Type] / count;
			}
		}
		else {
			ret.push_back(Vec3i(sum[0] / count, sum[1] / count, sum[2]));
			count = 1;
			sum = Vec3i(lines[i]);
			continue;
		}
	}
	ret.push_back(Vec3i(sum[0] / count, sum[1] / count, sum[2]));
	return ret;
}

bool TableImage::relevent(Vec3i a, int count, Vec3i b, int Type) {
	// MIN_GAP
	int MIN_GAP = (oriImg.cols > oriImg.rows) ? oriImg.cols / 200 : oriImg.rows / 200;
	int x = a[0] / count;
	int y = a[1] / count;
	int length = a[2];
	if (abs(b[0] - x) <= MIN_GAP && abs(b[1] - y) <= MIN_GAP)
		return true;
	else
	{
		switch (Type) {
		case 0:
			if (abs(b[1] - y) <= MIN_GAP && (b[0] >= x && b[0] <= x + length))
				return true;
			else return false;
			break;
		case 1:
			if (abs(b[0] - x) <= MIN_GAP && (b[1] >= y && b[1] <= y + length))
				return true;
			else return false;
		default:
			return false;
		}
	}
}

bool TableImage::isRect(Vec3i h1, Vec3i h2, Vec3i v1, Vec3i v2) {
	int MIN_TOL = 2;
	int y1 = h1[1];
	int y2 = h2[1];
	if (!(v1[1] <= y1 + MIN_TOL && v1[1] + v1[2] >= y2 - MIN_TOL)) return false;
	if (!(v2[1] <= y1 + MIN_TOL && v2[1] + v2[2] >= y2 - MIN_TOL)) return false;
	int x1 = v1[0];
	int x2 = v2[0];
	if (!(h1[0] <= x1 + MIN_TOL && h1[0] + h1[2] >= x2 - MIN_TOL)) return false;
	if (!(h2[0] <= x1 + MIN_TOL && h2[0] + h2[2] >= x2 - MIN_TOL)) return false;
	return true;

}

bool TableImage::isTapezoid(vector<Point2f>& points) {
	if (points.size() != 4) return false;
	bool flag = false;
	// 0&1, 2&3
	double theta1, theta2;
	if (points[0].x == points[1].x) theta1 = CV_PI / 2.0;
	else theta1 = atan(abs(points[0].y - points[1].y) / abs(points[0].x - points[1].x));
	if (points[2].x == points[3].x) theta2 = CV_PI / 2.0;
	else theta2 = atan(abs(points[2].y - points[3].y) / abs(points[2].x - points[3].x));
	if (abs(theta1 - theta2) < 10.0 * CV_PI / 180.0) {
		flag = true;
		////cout << "0,1: " << theta1 << " " << theta2 << endl;
	}
	// 0&2, 1&3
	if (points[0].x == points[2].x) theta1 = CV_PI / 2.0;
	else theta1 = atan(abs(points[0].y - points[2].y) / abs(points[0].x - points[2].x));
	if (points[1].x == points[3].x) theta2 = CV_PI / 2.0;
	else theta2 = atan(abs(points[1].y - points[3].y) / abs(points[1].x - points[3].x));
	if (abs(theta1 - theta2) < 10.0 * CV_PI / 180.0) {
		flag = true;
		////cout << "0,2: " << theta1 << " " << theta2 << endl;
	}
	// 0&3, 1&2
	if (points[0].x == points[3].x) theta1 = CV_PI / 2.0;
	else theta1 = atan(abs(points[0].y - points[3].y) / abs(points[0].x - points[3].x));
	if (points[2].x == points[1].x) theta2 = CV_PI / 2.0;
	else theta2 = atan(abs(points[2].y - points[1].y) / abs(points[2].x - points[1].x));
	if (abs(theta1 - theta2) < 10.0 * CV_PI / 180.0) {
		flag = true;
		////cout << "0,3: " << theta1 << " " << theta2 << endl;
	}

	if (!flag) {
		return false;
	}
	else {
		int min = -1, max = -1;
		int min_cor = 0x7FFF, max_cor = 0;
		for (int i = 0; i < points.size(); i++) {
			if (points[i].x + points[i].y < min_cor) {
				min = i;
				min_cor = points[i].x + points[i].y;
			}
			if (points[i].x + points[i].y > max_cor) {
				max = i;
				max_cor = points[i].x + points[i].y;
			}
		}
		int upright = -1, downleft = -1;
		int maxright = 0, maxdown = 0;
		for (int i = 0; i < points.size(); i++) {
			if (i == min || i == max) continue;
			if (points[i].x > maxright) {
				upright = i;
				maxright = points[i].x;
			}
			if (points[i].y > maxdown) {
				downleft = i;
				maxdown = points[i].y;
			}
		}
		vector<Point> temp;
		for (int i = 0; i < points.size(); i++)
			temp.push_back(points[i]);
		points.clear();
		points.push_back(temp[min]);
		points.push_back(temp[upright]);
		points.push_back(temp[downleft]);
		points.push_back(temp[max]);
		return true;
	}
}