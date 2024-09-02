// MFC_OpenCV_Ex1Dlg.cpp
#include "pch.h"
#include "framework.h"
#include "MFC_OpenCV_Ex1.h"
#include "MFC_OpenCV_Ex1Dlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <io.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad1();
	afx_msg void OnBnClickedButtonLoad2();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LOAD1, &CAboutDlg::OnBnClickedButtonLoad1)
	ON_BN_CLICKED(IDC_BUTTON_LOAD2, &CAboutDlg::OnBnClickedButtonLoad2)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedButtonLoad1()
{
	AfxMessageBox(_T("Load1 버튼 클릭됨"));
}

void CAboutDlg::OnBnClickedButtonLoad2()
{
	AfxMessageBox(_T("Load2 버튼 클릭됨"));
}

// CMFCOpenCVEx1Dlg 대화 상자
CMFCOpenCVEx1Dlg::CMFCOpenCVEx1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_OPENCV_EX1_DIALOG, pParent)
	, m_path1(_T(""))
	, m_path2(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCOpenCVEx1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE1, m_picture1);
	DDX_Control(pDX, IDC_PICTURE2, m_picture2);
	DDX_Text(pDX, IDC_EDIT3, m_path1);
	DDX_Text(pDX, IDC_EDIT4, m_path2);
}

BEGIN_MESSAGE_MAP(CMFCOpenCVEx1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD1, &CMFCOpenCVEx1Dlg::OnBnClickedMfcbutton1)
	ON_BN_CLICKED(IDC_BUTTON_LOAD2, &CMFCOpenCVEx1Dlg::OnBnClickedMfcbutton2)
END_MESSAGE_MAP()

BOOL CMFCOpenCVEx1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	return TRUE;
}

void CMFCOpenCVEx1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMFCOpenCVEx1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMFCOpenCVEx1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include <locale>
#include <codecvt>

void CMFCOpenCVEx1Dlg::OnBnClickedMfcbutton1()
{
	// 파일 탐색기 열기
	CFileDialog dlg(TRUE, _T("이미지 파일"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Image Files (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp|All Files (*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		m_path1 = dlg.GetPathName(); // 선택된 파일 경로를 m_path1에 저장
		UpdateData(FALSE); // 경로를 Edit Control에 표시

		// CString을 std::wstring으로 변환
		std::wstring path1W = m_path1.GetString();

		// std::wstring을 std::string으로 변환 (UTF-8)
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		std::string path1 = conv.to_bytes(path1W);

		// 디버깅용 메시지: 경로 확인
		// CString debugMessage;
		// debugMessage.Format(_T("이미지 경로: %s"), m_path1);
		// AfxMessageBox(debugMessage);

		// 이미지 경로에서 OpenCV로 이미지 로드
		cv::Mat img = cv::imread(path1, cv::IMREAD_COLOR);
		if (img.empty())
		{

			AfxMessageBox(_T("이미지를 로드할 수 없습니다."));
			
		}


		// clock_t start, finish;
		// double duration;

		// start = clock();

		/*실행 시간을 측정하고 싶은 코드*/
		// Picture Control에 이미지 표시
		CMFCOpenCVEx1Dlg::DisplayMatInPictureControl(img, 1); // m_picture2에 이미지를 표시

		// finish = clock();

		// duration = (double)(finish - start) / CLOCKS_PER_SEC;
		// CString debugMessage;
		// debugMessage.Format(_T("%f초"), duration);
		// AfxMessageBox(debugMessage);
	}
}



void CMFCOpenCVEx1Dlg::OnBnClickedMfcbutton2()
{
	// 파일 탐색기 열기
	CFileDialog dlg(TRUE, _T("이미지 파일"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Image Files (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp|All Files (*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		m_path2 = dlg.GetPathName(); // 선택된 파일 경로를 m_path2에 저장
		UpdateData(FALSE); // 경로를 Edit Control에 표시

		// CString을 std::wstring으로 변환
		std::wstring path2W = m_path2.GetString();

		// std::wstring을 std::string으로 변환 (UTF-8)
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		std::string path2 = conv.to_bytes(path2W);

		// 디버깅용 메시지: 경로 확인
		// CString debugMessage;
		// debugMessage.Format(_T("이미지 경로: %s"), m_path2);
		// AfxMessageBox(debugMessage);

		// 이미지 경로에서 OpenCV로 이미지 로드
		cv::Mat img = cv::imread(path2, cv::IMREAD_COLOR);
		if (img.empty())
		{

			AfxMessageBox(_T("이미지를 로드할 수 없습니다."));

		}


		// Picture Control에 이미지 표시
		CMFCOpenCVEx1Dlg::DisplayMatInPictureControl(img, 2); // m_picture2에 이미지를 표시

	}
}



void CMFCOpenCVEx1Dlg::DisplayMatInPictureControl(cv::Mat& matImage, int controlIndex)
{
	// Picture Control의 크기 가져오기
	CRect rect;
	if (controlIndex == 1)
	{
		m_picture1.GetClientRect(&rect);
	}
	else if (controlIndex == 2)
	{
		m_picture2.GetClientRect(&rect);
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
		m_picture1.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
		m_picture1.SetBitmap(hBitmap);
	}
	else if (controlIndex == 2)
	{
		m_picture2.ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);  // 스타일 설정
		m_picture2.SetBitmap(hBitmap);
	}
}



/*
void CMFCOpenCVEx1Dlg::DisplayImage(cv::Mat& img, CStatic& pictureControl)
{
	// 이미지가 제대로 변환되었는지 확인
	if (img.empty())
	{
		AfxMessageBox(_T("이미지를 변환하는 중 오류가 발생했습니다."));
		return;
	}

	// Picture Control의 클라이언트 영역 크기 가져오기
	CRect rect;
	pictureControl.GetClientRect(&rect);
	int clientWidth = rect.Width();
	int clientHeight = rect.Height();

	// 이미지 사이즈를 Picture Control의 크기에 맞게 리사이즈
	cv::Mat resizedImg;
	cv::resize(img, resizedImg, cv::Size(clientWidth, clientHeight));

	// CDC 및 HDC 가져오기
	CDC* pDC = pictureControl.GetDC();

	// RGB 채널 각각을 CDC에 플로팅
	for (int y = 0; y < resizedImg.rows; ++y)
	{
		for (int x = 0; x < resizedImg.cols; ++x)
		{
			// BGR 순서로 값을 가져옴
			cv::Vec3b color = resizedImg.at<cv::Vec3b>(y, x);
			COLORREF rgbColor = RGB(color[2], color[1], color[0]); // BGR to RGB 변환

			// 픽셀 설정
			pDC->SetPixel(x, y, rgbColor);
		}
	}

	// DC 해제
	pictureControl.ReleaseDC(pDC);
}
*/