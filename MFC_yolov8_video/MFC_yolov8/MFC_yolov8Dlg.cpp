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
#include <windows.h>
#include <gdiplus.h>  // GDI+ 헤더 파일 추가
#pragma comment(lib, "gdiplus.lib")  // GDI+ 라이브러리 링크

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCyolov8Dlg 대화 상자

IMPLEMENT_DYNAMIC(CMFCyolov8Dlg, CDialogEx)

// 전역 변수
cv::Mat g_loadedImage;
std::string g_onnxPath;
Inference* g_inference = nullptr;
bool g_isVideoPlaying = false; // 영상 재생 상태를 나타내는 플래그
bool g_exitRequested = false;  // 프로그램 종료 요청 상태를 나타내는 플래그

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
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
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
		// 더블 버퍼링을 사용하여 그리기
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);

		// 메모리 DC 생성
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		// 비트맵 생성
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);

		// 메모리 DC에 그리기
		memDC.FillSolidRect(&rect, GetSysColor(COLOR_3DFACE)); // 배경 채우기

		// 추가적으로 필요한 그리기 작업을 메모리 DC에 수행

		// 메모리 DC를 화면에 복사
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		memDC.SelectObject(pOldBitmap); // 원래 비트맵 복구
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

	// 프로젝트 경로에서 모델 경로를 기반으로 classes.txt 경로를 설정
	std::string classesPath = g_onnxPath.substr(0, g_onnxPath.find_last_of('\\')) + "\\classes.txt";

	// 모델 이름 추출
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

	if (g_inference != nullptr)
	{
		delete g_inference;
		g_inference = nullptr;
	}

	// Inference 객체 생성 (ONNX 모델, 입력 사이즈, 클래스 파일 경로, GPU 사용 여부)
	g_inference = new Inference(g_onnxPath, inputSize, classesPath, false);
}

// 두 번째 Load 버튼 (영상 파일 로드) 핸들러
void CMFCyolov8Dlg::OnBnClickedButtonLoadImage()
{
	CFileDialog dlg(TRUE, _T("mp4;avi"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Video Files (*.mp4;*.avi)|*.mp4;*.avi|All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		CString videoPath = dlg.GetPathName();
		m_editImagePath.SetWindowText(videoPath);  // Edit Control에 경로 설정

		// OpenCV로 비디오 로드
		std::string stdVideoPath = CW2A(videoPath, CP_UTF8);  // CString을 std::string으로 변환 후 사용

		m_videoCapture.open(stdVideoPath);
		if (!m_videoCapture.isOpened())
		{
			AfxMessageBox(_T("Failed to load the video."));
			return;
		}

		// 첫 번째 프레임 읽기
		m_videoCapture >> g_loadedImage;
		if (g_loadedImage.empty())
		{
			AfxMessageBox(_T("Failed to load the first frame of the video."));
			return;
		}

		// 첫 번째 프레임을 Picture Control에 표시
		DisplayFrameInControlUsingGDIPlus(g_loadedImage, m_pictureLeft);
	}
}

cv::Mat RunInference(Inference* inf, const cv::Mat& frame)
{
	// 이미지가 비어있는지 확인
	if (frame.empty())
	{
		std::cerr << "Error: Input frame is empty." << std::endl;
		return frame; // 비어있는 Mat 반환
	}

	// Inference 시작
	std::vector<Detection> output = inf->runInference(frame);

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

// Detection 버튼
void CMFCyolov8Dlg::OnBnClickedButtonDetection()
{
	if (!m_videoCapture.isOpened())
	{
		AfxMessageBox(_T("No video loaded."));
		return;
	}

	g_isVideoPlaying = true;  // 영상 재생 중 상태 설정

	cv::Mat frame;

	// GDI+ 초기화
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	while (g_isVideoPlaying && !g_exitRequested)
	{
		m_videoCapture >> frame;  // 다음 프레임을 가져옴
		if (frame.empty())
		{
			AfxMessageBox(_T("Video playback finished or failed."));
			break;
		}

		// 원본 프레임을 왼쪽 Picture Control에 표시
		DisplayFrameInControlUsingGDIPlus(frame, m_pictureLeft);

		// 탐지 수행
		cv::Mat resultFrame = RunInference(g_inference, frame);

		// 탐지된 결과 프레임을 오른쪽 Picture Control에 표시
		DisplayFrameInControlUsingGDIPlus(resultFrame, m_pictureRight);

		// 프레임 간의 간격을 두기 위해 약간의 지연을 추가
		cv::waitKey(1);  // 30 FPS에 맞게 대기 시간 설정 (1000ms / 30 = 약 33ms)
	}

	// GDI+ 종료
	Gdiplus::GdiplusShutdown(gdiplusToken);

	g_isVideoPlaying = false;  // 영상 재생 종료 상태 설정
}

void CMFCyolov8Dlg::DisplayFrameInControlUsingGDIPlus(const cv::Mat& frame, CStatic& pictureControl)
{
	// Picture Control의 크기 가져오기
	CRect rect;

	pictureControl.GetClientRect(&rect);

	int controlWidth = rect.Width();
	int controlHeight = rect.Height();

	// 원본 이미지 크기
	int imgWidth = frame.cols;
	int imgHeight = frame.rows;

	// 이미지 비율을 유지하면서 Picture Control 크기에 맞게 조정
	double widthRatio = static_cast<double>(controlWidth) / imgWidth;
	double heightRatio = static_cast<double>(controlHeight) / imgHeight;
	double scale = std::min(widthRatio, heightRatio);

	int newWidth = static_cast<int>(imgWidth * scale);
	int newHeight = static_cast<int>(imgHeight * scale);

	cv::Mat resizedMatImage;
	cv::resize(frame, resizedMatImage, cv::Size(newWidth, newHeight));

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
	pictureControl.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
	pictureControl.SetBitmap(hBitmap);
}
// 더블 버퍼링을 사용하여 깜빡임 방지
BOOL CMFCyolov8Dlg::OnEraseBkgnd(CDC* pDC)
{
	// 배경을 지우지 않음
	return TRUE;
}

// 창 닫기 처리 (영상 재생 중에도 닫을 수 있도록)
void CMFCyolov8Dlg::OnClose()
{
	g_exitRequested = true;  // 프로그램 종료 요청 플래그 설정

	if (g_isVideoPlaying)
	{
		g_isVideoPlaying = false;  // 영상 재생 종료
		cv::waitKey(1);  // 영상 루프가 종료되도록 약간의 시간 대기
	}

	CDialogEx::OnClose();  // 기본 닫기 동작 수행
	PostQuitMessage(0);  // 프로그램 종료 요청
}
