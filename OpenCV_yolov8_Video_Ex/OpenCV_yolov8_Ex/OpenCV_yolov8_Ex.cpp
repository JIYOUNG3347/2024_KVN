#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "inference.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	std::string projectBasePath = "C:/Users/jiyoung/Desktop/KVN";
	bool runOnGPU = false;

	// 모델 경로와 설정 초기화
	Inference inf(projectBasePath + "/sources/yolov8s.onnx", cv::Size(640, 480),
		projectBasePath + "/sources/classes.txt", runOnGPU);

	// 동영상 파일 경로
	std::string videoPath = projectBasePath + "/sources/slow_traffic_small.mp4";

	// 동영상 캡처 객체 생성
	VideoCapture cap(videoPath);

	if (!cap.isOpened()) {
		std::cerr << "Error: Could not open video file." << std::endl;
		return -1;
	}

	Mat frame;
	while (cap.read(frame)) {
		if (frame.empty()) {
			break;
		}

		auto start = std::chrono::high_resolution_clock::now();

		std::vector<Detection> output = inf.runInference(frame);

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> inferenceTime = end - start;
		std::cout << "Inference time: " << inferenceTime.count() << " seconds" << std::endl;

		int detections = output.size();
		std::cout << "Number of detections: " << detections << std::endl;

		for (int i = 0; i < detections; ++i) {
			Detection detection = output[i];

			cv::Rect box = detection.box;
			cv::Scalar color = detection.color;

			// Detection box
			cv::rectangle(frame, box, color, 2);

			// Detection box text
			std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
			cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
			cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

			cv::rectangle(frame, textBox, color, cv::FILLED);
			cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
		}
		// Inference ends here...

		// 이 코드는 결과를 보여주기 위한 것입니다.
		float scale = 0.5;
		cv::resize(frame, frame, cv::Size(frame.cols * scale, frame.rows * scale));
		cv::imshow("Inference", frame);

		if (cv::waitKey(1) >= 0) {  // 30ms마다 프레임을 보여줍니다.
			break;
		}
	}

	cap.release();
	cv::destroyAllWindows();

	return 0;
}
