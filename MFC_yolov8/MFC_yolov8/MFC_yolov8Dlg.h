// MFC_yolov8Dlg.h: 헤더 파일
//

#pragma once
#include "afxwin.h"  // MFC 관련 헤더 파일
#include <opencv2/opencv.hpp>
// CMFCyolov8Dlg 대화 상자
class CMFCyolov8Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMFCyolov8Dlg)

public:
	CMFCyolov8Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_YOLOV8_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnBnClickedButtonLoadOnnx();     // ONNX 파일 로드 버튼 핸들러
	afx_msg void OnBnClickedButtonLoadImage();    // 이미지 파일 로드 버튼 핸들러
	afx_msg void OnBnClickedButtonDetection();    // Detection 버튼 핸들러
	void CMFCyolov8Dlg::DisplayMatInPictureControl(cv::Mat& matImage, int controlIndex);

private:
	CEdit m_editOnnxPath;     // ONNX 파일 경로 Edit Control
	CEdit m_editImagePath;    // 이미지 파일 경로 Edit Control
	CStatic m_pictureLeft;    // 왼쪽 Picture Control
	CStatic m_pictureRight;   // 오른쪽 Picture Control
};
