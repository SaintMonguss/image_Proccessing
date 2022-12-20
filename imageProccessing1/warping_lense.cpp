#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

vector<Point2f> findImageCorners(Mat image, Size boardSize)
{
	static int cnt = 0;
	vector<Point2f> imgPoints;
	Mat gray; // ũ�� ���� x
	cvtColor(image, gray, COLOR_RGB2GRAY); // 2��° ���ڴ� 1��° ���ڿ� ������� ������ ���� �̹����� ��µ� ����.
											// �̶� gray�� ����� ������
	bool found = findChessboardCorners(gray, boardSize, imgPoints);

	/* findChess... �Լ��� ����
	*	ù ���ڷ� ���� �̹����� ü���� ����, 8��Ʈ �׷��� �������̰ų� �÷����� �� 
	*	������ ü�� ������ ����ϰ� �����°� �ƴϱ� ����.
	*	2��° ���ڴ� �� �� ���� ���� �𼭸��� ��
	*	3��°�� ��� �迭(�ּҿ� ���� ����)
	*	
	*	��ȯ�� boolŸ�� / ������ ��ŭ ���� �𼭸��� ã�� ���ϰų�, ã�Ҿ �����ϴµ� �����ϸ� 0��ȯ
	*/

	if (found) 
	{
		cornerSubPix(gray, imgPoints, Size(5, 5), Size(-1, -1),
			//TermCriteria() = (���� ����, �ִ� �ݺ�Ƚ��, ��Ȯ��). ��, �Ʒ� �ڵ�� �ִ� Ƚ��(30ȸ)��ŭ �ݺ��ϰ�, ��Ȯ��(0.1)���� ������ �Ŀ� �����. 
			// ���⼭ 0.1 ��Ȯ����°��� �ݺ��߿� �ڳ��� ��ġ�� 0.1 ���Ϸ� �̵����� �ɶ��� ����.
			TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1));

		/*
		* Ķ���극�̼��� ���� ���ڳĴ� ���е��� �޸�.
		* �ڳ� ��ġ�� �� �״�� ���°��� �ƴ϶� ���� �ȼ������� ��Ȯ���� ��� ���� �ٽ� �ѹ� ��ó�� ������ ��ħ
		* 
		* ù ���ڴ� �Է� �̹���
		* �ι�° ���ڴ� ����Ʈ�� �Է� �� ����� ����ϴ� vector 
		* ����° ���ڴ� �������� ����� ������ �ִ� �Ű�����
		* ������� ��� ���� �̷���� ������ ã�� Ž�� ������ ����
		*         ���� 
		* ���� �Ϲ������δ� üĿ������ ���簢�� ��ĭ�� �𼭸� ���̸� �Է��ϰ� ��
		* ���� ������ (���� * 2 + 1) // +1�� ������ �ڱ� �ڽ��� ũ��
		* 4��° ���ڴ� �߽������� �󸶳� �����Ұ������� ����, (-1, -1)�� �������� �ʰڴٴ� ��
		*/
		drawChessboardCorners(image, boardSize, imgPoints, found);
		/*
		* ���� ��ó���� ���� ���� ����Ʈ�� 4��° ����[found]�� ���̶��, �������� ����� �𼭸��� �׷���
		*/
		imshow("image_" + to_string(cnt), image);
		waitKey();
		destroyWindow("image_" + to_string(cnt++));
	}

	return imgPoints;
}

//������ ���� ���ٰ� ������ ������� �񱳱� ��ǥ ����
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
	Size boardSize(8, 7), imageSize; // 8�ʺ�, 7���� bord ����, ������/����� ĭ �� X, ����������� �̷���� �簢���� ���Ͱ��� ����
	float squareSize = 5.f;

	vector<String> filelist;
	filelist.push_back("../chessboard/chessboard_01.jpg");
	filelist.push_back("../chessboard/chessboard_02.jpg");
	filelist.push_back("../chessboard/chessboard_03.jpg");

	vector<vector<Point2f>> imagePoints; // float�� x,y ��ǥ���� ������ ����Ʈ
	vector<vector<Point3f>> objectPoints; // float�� x,y,z ��ǥ���� ������ ����Ʈ

	for (int i = 0; i < filelist.size(); i++)
	{
		Mat image = imread(filelist[i], 1);
		//imread �̹����� �ε� ���н� NULL�� ��ȯ
		// ù��° ���ڴ� �����̸�, �ι�° ���ڴ� 
		//Ư���� ���� Ȯ���ڰ� �ƴ� �������� �̹��� ������ ����.
		//�ι�° ���ڴ� ���� �ɼ��� �ִ°�, ������ ����, ȸ��, ���� ��ȯ, ������ ��ȯ��
		CV_Assert(image.data);
		//��Ÿ�ӿ� ()���� ������ 0�̸� ���ܸ� �߻���Ű�� �Լ�
		//���� ó���� ���

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
	* ���� ������ ���� ���⿡�� ī�޶� ���� �� �ܺ� �Ű������� ã�� ���� ��ǥ.
	* 
	* ù��° ���ڴ� calcObjectCorners�� ���� ����� �񱳱� ��ǥ���� ����(üĿ���带 �ٲ��� �ʾҴٸ� ��� ����)
	* �ι�° ���ڴ� ���� �̹������� ����
	* �׹�° ���ڴ� �Լ��� ���� �� ī�޶� ���� �Ķ���� ���� ��ȯ���� ����
	* �ټ���° ���ڴ� �ְ� ��� ���
	* 
	*/

	undistort(image, undistorted, cameraMatrix, distCoeffs);
	/*
	* ī�޶� ����� ���� �ְ� ������ ���� �Լ�
	*
	* �ι�° ���ڴ� �ְ� ������ ���� ������ �����
	* ����° ���ڴ� ���� calibrateCamera���� ����� cameraMatrix ��, ī�޶� ���� �Ķ����(�Ǵ� ���) ��.
	* �׹�° ���ڴ� ī�޶� �ְ� ����� �Է¹���
	* 
	* �����Ŀ� ���� �ְ��� ���ŵ� ������ ��ȯ��
	*/

	cout << "cameraMatrix " << endl << cameraMatrix << endl << endl;
	cout << "�ְ��� " << endl << distCoeffs << endl << endl;

	printf("RMS error reported by calibrateCamera: %g\n", rms);
	/*
	* �Լ� ���� ������ ���� �޼���x
	* RMS ������ RPE ��� ��. Re Projection Err�� ����.
	* �� ������ ���� ������ ����. 0 �� �������� ����
	* ��� ��������� �̸��극�̼��� ���� �߰��� �Ķ���� ���� ������ ����ؼ� �����
	* �̸��극�̼� ī�޶��� ���ϰ��� �� RPE��
	*/
	imshow("Original", image);
	imshow("Undistorted", undistorted);
	waitKey();

	return 0;
}