
// MFC_OpenCV_Ex1Dlg.h: 헤더 파일
//
#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// CMFCOpenCVEx1Dlg 대화 상자
class CMFCOpenCVEx1Dlg : public CDialogEx
{
// 생성입니다.
public:
	void CMFCOpenCVEx1Dlg::DisplayMatInPictureControl(cv::Mat& matImage, int controlIndex);
	afx_msg void OnBnClickedButtonLoad1();
	afx_msg void OnBnClickedButtonLoad2();
	CMFCOpenCVEx1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_OPENCV_EX1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMfcbutton1();
	afx_msg void OnBnClickedMfcbutton2();
	CStatic m_picture1;
	CStatic m_picture2;
	CString m_path1;
	CString m_path2;
};
