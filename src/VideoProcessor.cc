#include "VideoProcessor.h"
#include <iostream>

VideoProcessor::VideoProcessor() 
    : brightnessThreshold_(100), 
      processingMode_(DIRECT_BOUNDS) {}

void VideoProcessor::setBrightnessThreshold(int threshold) {
    brightnessThreshold_ = threshold;
}

void VideoProcessor::setProcessingMode(ProcessingMode mode) {
    processingMode_ = mode;
}

void VideoProcessor::setMinContourArea(int minArea) {
    contourAnalyzer_.setMinArea(minArea);
}

int VideoProcessor::getBrightnessThreshold() const {
    return brightnessThreshold_;
}

cv::Mat VideoProcessor::processFrame(const cv::Mat& frame) {
    cv::Mat grayFrame, binaryFrame;
    
    // 转换为灰度图并二值化
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    cv::threshold(grayFrame, binaryFrame, brightnessThreshold_, 255, cv::THRESH_BINARY);
    
    switch (processingMode_) {
        case BINARY_ONLY:
            return binaryFrame;
            
        case CONTOUR_ANALYSIS:
            {
                auto boundingInfo = contourAnalyzer_.analyzeWhiteRegions(binaryFrame);
                cv::Mat resultFrame = frame.clone();
                contourAnalyzer_.drawAnalysisResult(resultFrame, boundingInfo);
                return resultFrame;
            }
            
        case DIRECT_BOUNDS:
            {
                auto bounds = contourAnalyzer_.findWhiteBoundsDirectly(binaryFrame);
                cv::Mat resultFrame = frame.clone();
                contourAnalyzer_.drawSimpleBounds(resultFrame, bounds);
                return resultFrame;
            }
    }
    
    return binaryFrame;
}

void VideoProcessor::displayInfo(cv::Mat& frame, int frameCount) const {
    std::string info = "Frame: " + std::to_string(frameCount);
    cv::putText(frame, info, 
                cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
}

void VideoProcessor::processVideo(const std::string& videoPath) {
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "错误：无法打开视频文件: " << videoPath << std::endl;
        return;
    }
    
    double fps = cap.get(cv::CAP_PROP_FPS);
    int width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    
    std::cout << "视频信息: " << std::endl;
    std::cout << "  分辨率: " << width << "x" << height << std::endl;
    std::cout << "  帧率: " << fps << " FPS" << std::endl;
    std::cout << "  总帧数: " << totalFrames << std::endl;
    std::cout << "  亮度阈值: " << brightnessThreshold_ << " (0-255)" << std::endl;
    
    std::string modeStr;
    switch (processingMode_) {
        case BINARY_ONLY: modeStr = "二值化显示"; break;
        case CONTOUR_ANALYSIS: modeStr = "轮廓分析"; break;
        case DIRECT_BOUNDS: modeStr = "直接边界检测"; break;
    }
    std::cout << "  处理模式: " << modeStr << std::endl;
    
    std::string resultWindow = "处理结果";
    cv::namedWindow("原始视频", cv::WINDOW_AUTOSIZE);
    cv::namedWindow(resultWindow, cv::WINDOW_AUTOSIZE);
    
    cv::Mat frame, processedFrame;
    int frameCount = 0;
    
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "视频处理完成!" << std::endl;
            break;
        }
        
        frameCount++;
        
        processedFrame = processFrame(frame);
        displayInfo(frame, frameCount);
        
        cv::imshow("原始视频", frame);
        cv::imshow(resultWindow, processedFrame);
        
        char key = static_cast<char>(cv::waitKey(30));
        if (key == 27 || key == 'q') {
            std::cout << "用户中断处理" << std::endl;
            break;
        } else if (key == 'p') {
            cv::waitKey(0);
        } else if (key == '+') {
            brightnessThreshold_ = std::min(255, brightnessThreshold_ + 5);
            std::cout << "亮度阈值增加到: " << brightnessThreshold_ << std::endl;
        } else if (key == '-') {
            brightnessThreshold_ = std::max(0, brightnessThreshold_ - 5);
            std::cout << "亮度阈值减少到: " << brightnessThreshold_ << std::endl;
        } else if (key == 'm') {
            processingMode_ = static_cast<ProcessingMode>((processingMode_ + 1) % 3);
            std::string newModeStr;
            switch (processingMode_) {
                case BINARY_ONLY: newModeStr = "二值化显示"; break;
                case CONTOUR_ANALYSIS: newModeStr = "轮廓分析"; break;
                case DIRECT_BOUNDS: newModeStr = "直接边界检测"; break;
            }
            std::cout << "切换到模式: " << newModeStr << std::endl;
        }
        
        if (frameCount % 100 == 0) {
            std::cout << "已处理 " << frameCount << " / " << totalFrames << " 帧" << std::endl;
        }
    }
    
    cap.release();
    cv::destroyAllWindows();
    std::cout << "总共处理了 " << frameCount << " 帧" << std::endl;
}