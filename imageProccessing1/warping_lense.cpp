#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

vector<Point2f> findImageCorners(Mat image, Size boardSize)
{
	static int cnt = 0;
	vector<Point2f> imgPoints;
	Mat gray; // 크기 선언 x
	cvtColor(image, gray, COLOR_RGB2GRAY); // 2번째 인자는 1번째 인자와 사이즈와 뎁스가 같은 이미지로 출력될 인자.
											// 이때 gray의 사이즈가 결정됨
	bool found = findChessboardCorners(gray, boardSize, imgPoints);

	/* findChess... 함수의 설명
	*	첫 인자로 들어온 이미지는 체스판 보기, 8비트 그레이 스케일이거나 컬러여야 함 
	*	이유는 체스 사진만 깔끔하게 들어오는게 아니기 때문.
	*	2번째 인자는 행 및 열당 내부 모서리의 수
	*	3번째는 출력 배열(주소에 의한 참조)
	*	
	*	반환값 bool타입 / 지정된 만큼 내부 모서리를 찾지 못하거나, 찾았어도 정렬하는데 실패하면 0반환
	*/

	if (found) {
		cornerSubPix(gray, imgPoints, Size(11, 11), Size(-1, -1),
			TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1));
		/*
		* 
		* 
		* 
		*/
		drawChessboardCorners(image, boardSize, imgPoints, found);
		imshow("image_" + to_string(cnt), image);
		waitKey();
		destroyWindow("image_" + to_string(cnt++));
	}

	return imgPoints;
}

vector<Point3f> calcObjectCorners(Size boardSize, float squareSize)
{
	vector<Point3f> corners;
	for (int i = 0; i < boardSize.height; i++)
		for (int j = 0; j < boardSize.width; j++)
		{
			float x = float(j * squareSize);
			float y = float(i * squareSize);
			corners.push_back(Point3f(x, y, 0));
		}

	return corners;
}

int main()
{
	Size boardSize(8, 7), imageSize; // 8너비, 7높이 bord 선언, 검은색/흰색의 칸 수 X, 검흰검흰으로 이루어진 사각형의 센터값이 기준
	float squareSize = 1.f;

	vector<String> filelist;
	filelist.push_back("chessboard_01.jpg");
	filelist.push_back("chessboard_02.jpg");
	filelist.push_back("chessboard_03.jpg");

	vector<vector<Point2f>> imagePoints; // float형 x,y 좌표값을 가지는 포인트
	vector<vector<Point3f>> objectPoints; // float형 x,y,z 좌표값을 가지는 포인트

	for (int i = 0; i < filelist.size(); i++)
	{
		Mat image = imread(filelist[i], 1);
		//imread 이미지를 로드 실패시 NULL을 반환
		// 첫번째 인자는 파일이름, 두번째 인자는 
		//특이한 점은 확장자가 아닌 내용으로 이미지 유형을 결정.
		//두번째 인자는 각종 옵션을 주는것, 사이즈 고정, 회전, 색상 변환, 스케일 변환등
		CV_Assert(image.data);
		//런타임에 ()안의 조건이 0이면 예외를 발생시키는 함수
		//예외 처리에 사용

		vector<Point2f> imgPoints = findImageCorners(image, boardSize);

		if (!imgPoints.empty())
		{
			vector<Point3f> objPoints = calcObjectCorners(boardSize, squareSize);

			imagePoints.push_back(imgPoints);
			objectPoints.push_back(objPoints);
		}
	}

	vector<Mat> rvecs, tvecs;
	Mat cameraMatrix, distCoeffs;
	Mat undistorted;

	Mat image = imread("../image/chessboard_05.jpg", 1);
	CV_Assert(image.data);

	double rms = calibrateCamera(objectPoints, imagePoints, image.size(),
		cameraMatrix, distCoeffs, rvecs, tvecs);

	undistort(image, undistorted, cameraMatrix, distCoeffs);

	cout << "cameraMatrix " << endl << cameraMatrix << endl << endl;
	printf("RMS error reported by calibrateCamera: %g\n", rms);

	imshow("Original", image);
	imshow("Undistorted", undistorted);
	waitKey();

	return 0;
}