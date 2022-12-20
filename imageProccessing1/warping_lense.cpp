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

	if (found) 
	{
		cornerSubPix(gray, imgPoints, Size(5, 5), Size(-1, -1),
			//TermCriteria() = (만족 조건, 최대 반복횟수, 정확도). 즉, 아래 코드는 최대 횟수(30회)만큼 반복하고, 정확도(0.1)보다 낮아진 후에 종료됨. 
			// 여기서 0.1 정확도라는것은 반복중에 코너의 위치가 0.1 이하로 이동조정 될때를 뜻함.
			TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1));

		/*
		* 캘리브레이션이 좋냐 나쁘냐는 정밀도에 달림.
		* 코너 위치를 얻어낸 그대로 쓰는것이 아니라 서브 픽셀수준의 정확도를 얻기 위해 다시 한번 전처리 공정을 거침
		* 
		* 첫 인자는 입력 이미지
		* 두번째 인자는 포인트의 입력 및 출력을 담당하는 vector 
		* 세번째 인자는 윈도우의 사이즈를 지정해 주는 매개변수
		* 윈도우는 흰검 으로 이루어진 한점을 찾을 탐색 단위를 뜻함
		*         검흰 
		* 따라서 일반적으로는 체커보드의 정사각형 한칸의 모서리 길이를 입력하게 됨
		* 내부 공식은 (인자 * 2 + 1) // +1은 센터의 자기 자신의 크기
		* 4번째 인자는 중심점에서 얼마나 무시할것인지를 뜻함, (-1, -1)은 무시하지 않겠다는 뜻
		*/
		drawChessboardCorners(image, boardSize, imgPoints, found);
		/*
		* 위의 전처리가 끝난 감지 포인트에 4번째 인자[found]가 참이라면, 색상선으로 연결된 모서리를 그려냄
		*/
		imshow("image_" + to_string(cnt), image);
		waitKey();
		destroyWindow("image_" + to_string(cnt++));
	}

	return imgPoints;
}

//변형이 전혀 없다고 가정한 산술적인 비교군 좌표 생성
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
	float squareSize = 5.f;

	vector<String> filelist;
	filelist.push_back("../chessboard/chessboard_01.jpg");
	filelist.push_back("../chessboard/chessboard_02.jpg");
	filelist.push_back("../chessboard/chessboard_03.jpg");

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

	Mat image = imread("../chessboard/chessboard_05.jpg", 1);
	CV_Assert(image.data);

	double rms = calibrateCamera(objectPoints, imagePoints, image.size(),
		cameraMatrix, distCoeffs, rvecs, tvecs);
	/*
	* 보정 패턴의 여러 보기에서 카메라 고유 및 외부 매개변수를 찾는 것이 목표.
	* 
	* 첫번째 인자는 calcObjectCorners를 통해 얻어진 비교군 좌표들의 집합(체커보드를 바꾸지 않았다면 모두 동일)
	* 두번째 인자는 샘플 이미지들의 집합
	* 네번째 인자는 함수를 통해 얻어낸 카메라 내부 파라미터 값을 반환해줄 변수
	* 다섯번째 인자는 왜곡 계수 행렬
	* 
	*/

	undistort(image, undistorted, cameraMatrix, distCoeffs);
	/*
	* 카메라 렌즈로 인한 왜곡 보정을 위한 함수
	*
	* 두번째 인자는 왜곡 보정이 끝난 영상을 출력함
	* 세번째 인자는 위의 calibrateCamera에서 얻어진 cameraMatrix 즉, 카메라 내부 파라미터(또는 행렬) 값.
	* 네번째 인자는 카메라 왜곡 계수의 입력백터
	* 
	* 수행후엔 렌즈 왜곡이 제거된 영상이 반환됨
	*/

	cout << "cameraMatrix " << endl << cameraMatrix << endl << endl;
	cout << "왜곡계수 " << endl << distCoeffs << endl << endl;

	printf("RMS error reported by calibrateCamera: %g\n", rms);
	/*
	* 함수 실행 에러에 대한 메세지x
	* RMS 에러는 RPE 라고도 함. Re Projection Err의 약자.
	* 즉 재투영 시의 편차를 뜻함. 0 에 가까울수록 좋음
	* 산술 평균적으로 켈리브레이션을 통해 발견한 파라미터 값의 편차를 계산해서 얄려줌
	* 켈리브레이션 카메라의 리턴값이 이 RPE임
	*/
	imshow("Original", image);
	imshow("Undistorted", undistorted);
	waitKey();

	return 0;
}