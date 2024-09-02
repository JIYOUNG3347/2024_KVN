// MFC_yolov8Dlg.cpp: 구현 파일
//
#include <iostream>
#include "pch.h"
#include "framework.h"
#include "MFC_yolov8.h"
#include "MFC_yolov8Dlg.h"
#include "afxdialogex.h"
#include <string> // std::string 사용을 위한 헤더 추가
#include <vector>
#include <opencv2/opencv.hpp>
#include "inference.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCyolov8Dlg 대화 상자

IMPLEMENT_DYNAMIC(CMFCyolov8Dlg, CDialogEx)

// 전역 변수
cv::Mat g_loadedImage;
std::string g_onnxPath;

CMFCyolov8Dlg::CMFCyolov8Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_YOLOV8_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCyolov8Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ONNX_PATH, m_editOnnxPath);  // ONNX 파일 경로 Edit Control 연결
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_editImagePath); // 이미지 파일 경로 Edit Control 연결
	DDX_Control(pDX, IDC_PICTURE_LEFT, m_pictureLeft);     // 왼쪽 Picture Control 연결
	DDX_Control(pDX, IDC_PICTURE_RIGHT, m_pictureRight);  // 오른쪽 Picture Control 연결
}

BEGIN_MESSAGE_MAP(CMFCyolov8Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_ONNX, &CMFCyolov8Dlg::OnBnClickedButtonLoadOnnx)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMAGE, &CMFCyolov8Dlg::OnBnClickedButtonLoadImage)
	ON_BN_CLICKED(IDC_BUTTON_DETECTION, &CMFCyolov8Dlg::OnBnClickedButtonDetection)
END_MESSAGE_MAP()

// CMFCyolov8Dlg 메시지 처리기

BOOL CMFCyolov8Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 대화 상자의 아이콘을 설정합니다.  애플리케이션의 주 창이 대화 상자가 아닐 경우에는 프레임워크가
	// 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCyolov8Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CDialogEx dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 버튼을 추가할 경우 아이콘을 그리려면 아래 코드가 필요합니다.
//  문서/뷰 모델을 사용하는 MFC 애플리케이션에서는 프레임워크가 이 작업을 자동으로 수행합니다.

void CMFCyolov8Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘의 중앙을 계산합니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끌 때 커서가 표시되도록 시스템에서 이 함수를 호출합니다.
HCURSOR CMFCyolov8Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 첫 번째 Load 버튼 (ONNX 파일 로드) 핸들러
void CMFCyolov8Dlg::OnBnClickedButtonLoadOnnx()
{
	CFileDialog dlg(TRUE, _T("onnx"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("ONNX Files (*.onnx)|*.onnx|All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();
		m_editOnnxPath.SetWindowText(path);  // Edit Control에 경로 설정

		// std::string 변환 및 전역 변수에 저장
		g_onnxPath = CT2A(path);  // CString을 std::string으로 변환하여 전역 변수에 저장
	}
}

// 두 번째 Load 버튼 (이미지 파일 로드) 핸들러
void CMFCyolov8Dlg::OnBnClickedButtonLoadImage()
{
	CFileDialog dlg(TRUE, _T("jpg;png;bmp"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Image Files (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp|All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		CString imagePath = dlg.GetPathName();
		m_editImagePath.SetWindowText(imagePath);  // Edit Control에 경로 설정

		// OpenCV로 이미지 로드 및 전역 변수에 저장
		std::string stdImagePath = CW2A(imagePath, CP_UTF8);  // CString을 std::string으로 변환 후 사용
		g_loadedImage = cv::imread(stdImagePath);
		if (g_loadedImage.empty())
		{
			AfxMessageBox(_T("Failed to load the image."));
			return;
		}

		// Picture Control에 표시하려면 cv::Mat을 HBITMAP으로 변환해야 함
		// 아래는 예시적으로 cv::Mat을 Picture Control에 표시하는 방식:
		/*
		CImage cimage;
		cimage.Create(g_loadedImage.cols, g_loadedImage.rows, 24);

		// cv::Mat 데이터를 CImage로 복사
		for (int y = 0; y < g_loadedImage.rows; y++)
		{
			BYTE* pDest = (BYTE*)cimage.GetPixelAddress(0, y);
			BYTE* pSrc = g_loadedImage.ptr<BYTE>(y);
			memcpy(pDest, pSrc, g_loadedImage.cols * 3);  // 컬러 이미지일 경우
		}

		// Picture Control에 비트맵 설정
		HBITMAP hBitmap = cimage.Detach();
		m_pictureLeft.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
		m_pictureLeft.SetBitmap(hBitmap);
		*/
		CMFCyolov8Dlg::DisplayMatInPictureControl(g_loadedImage, 1);

	}
}



cv::Mat RunInference(const std::string& modelPath, const cv::Mat& frame, bool runOnGPU = false)
{
	// 프로젝트 경로에서 모델 경로를 기반으로 classes.txt 경로를 설정
	std::string classesPath = modelPath.substr(0, modelPath.find_last_of('\\')) + "\\classes.txt";

	std::string fileName = g_onnxPath.substr(g_onnxPath.find_last_of('\\') + 1);

	// 입력 사이즈 초기화
	cv::Size inputSize;

	// 파일 이름에 따라 입력 사이즈 설정
	if (fileName == "yolov8n.onnx") {
		inputSize = cv::Size(640, 640);
	}
	else if (fileName == "yolov8s.onnx") {
		inputSize = cv::Size(640, 480);
	}
	else {
		// 기본값 설정 (필요한 경우)
		inputSize = cv::Size(640, 640);
	}

	// Inference 객체 생성 (ONNX 모델, 입력 사이즈, 클래스 파일 경로, GPU 사용 여부)
	Inference inf(modelPath, inputSize, classesPath, runOnGPU);

	// 이미지가 비어있는지 확인
	if (frame.empty())
	{
		std::cerr << "Error: Input frame is empty." << std::endl;
		return frame; // 비어있는 Mat 반환
	}

	// Inference 시작
	std::vector<Detection> output = inf.runInference(frame);

	// 감지된 객체 수 출력
	int detections = output.size();
	std::cout << "Number of detections: " << detections << std::endl;

	// 각 감지된 객체에 대해 처리
	cv::Mat resultFrame = frame.clone(); // 원본 프레임을 복사하여 결과 프레임 생성
	for (int i = 0; i < detections; ++i)
	{
		Detection detection = output[i];

		// 바운딩 박스와 색상 정보 가져오기
		cv::Rect box = detection.box;
		cv::Scalar color = detection.color;

		// 바운딩 박스 그리기
		cv::rectangle(resultFrame, box, color, 2);

		// 감지 정보 텍스트 생성
		std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
		cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
		cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

		// 텍스트 배경 박스와 텍스트 그리기
		cv::rectangle(resultFrame, textBox, color, cv::FILLED);
		cv::putText(resultFrame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
	}

	// 감지된 결과가 적용된 이미지 반환
	return resultFrame;
}

// Detection 버튼 (YOLO로 Image Detection 수행)
void CMFCyolov8Dlg::OnBnClickedButtonDetection()
{
	bool useGPU = false;
	cv::Mat resultFrame = RunInference(g_onnxPath, g_loadedImage, useGPU);
	/*
	CImage cimage;
	cimage.Create(resultFrame.cols, resultFrame.rows, 24);

	// cv::Mat 데이터를 CImage로 복사
	for (int y = 0; y < resultFrame.rows; y++)
	{
		BYTE* pDest = (BYTE*)cimage.GetPixelAddress(0, y);
		BYTE* pSrc = resultFrame.ptr<BYTE>(y);
		memcpy(pDest, pSrc, resultFrame.cols * 3);  // 컬러 이미지일 경우
	}

	// Picture Control에 비트맵 설정
	HBITMAP hBitmap = cimage.Detach();
	m_pictureRight.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
	m_pictureRight.SetBitmap(hBitmap);                      // Picture Control에 비트맵 설정
	*/
	CMFCyolov8Dlg::DisplayMatInPictureControl(resultFrame, 2);
}

void CMFCyolov8Dlg::DisplayMatInPictureControl(cv::Mat& matImage, int controlIndex)
{
	// Picture Control의 크기 가져오기
	CRect rect;
	if (controlIndex == 1)
	{
		m_pictureLeft.GetClientRect(&rect);
	}
	else if (controlIndex == 2)
	{
		m_pictureRight.GetClientRect(&rect);
	}
	int controlWidth = rect.Width();
	int controlHeight = rect.Height();

	// 원본 이미지 크기
	int imgWidth = matImage.cols;
	int imgHeight = matImage.rows;

	// 이미지 비율을 유지하면서 Picture Control 크기에 맞게 조정
	double widthRatio = static_cast<double>(controlWidth) / imgWidth;
	double heightRatio = static_cast<double>(controlHeight) / imgHeight;
	double scale = std::min(widthRatio, heightRatio);

	int newWidth = static_cast<int>(imgWidth * scale);
	int newHeight = static_cast<int>(imgHeight * scale);

	cv::Mat resizedMatImage;
	cv::resize(matImage, resizedMatImage, cv::Size(newWidth, newHeight));

	// CImage 객체 생성 및 초기화
	CImage cimage;
	cimage.Create(newWidth, newHeight, 24);

	// cv::Mat 데이터를 CImage로 복사
	for (int y = 0; y < newHeight; y++)
	{
		BYTE* pDest = (BYTE*)cimage.GetPixelAddress(0, y);
		BYTE* pSrc = resizedMatImage.ptr<BYTE>(y);
		memcpy(pDest, pSrc, newWidth * 3);  // 컬러 이미지일 경우
	}

	// HBITMAP 객체 생성
	HBITMAP hBitmap = cimage.Detach();

	// Picture Control 선택 및 비트맵 설정
	if (controlIndex == 1)
	{
		m_pictureLeft.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
		m_pictureLeft.SetBitmap(hBitmap);
	}
	else if (controlIndex == 2)
	{
		m_pictureRight.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
		m_pictureRight.SetBitmap(hBitmap);
	}
}
