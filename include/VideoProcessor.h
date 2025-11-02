#ifndef VIDEO_PROCESSOR_H
#define VIDEO_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include "ContourAnalysis.h"

class VideoProcessor {
public:
    enum ProcessingMode {
        BINARY_ONLY,        // 只显示二值化
        CONTOUR_ANALYSIS,   // 轮廓分析
        DIRECT_BOUNDS       // 直接边界检测
    };
    
    VideoProcessor();
    
    void setBrightnessThreshold(int threshold);
    void setProcessingMode(ProcessingMode mode);
    void setMinContourArea(int minArea);
    
    void processVideo(const std::string& videoPath);
    int getBrightnessThreshold() const;

private:
    int brightnessThreshold_;
    ProcessingMode processingMode_;
    ContourAnalysis contourAnalyzer_;
    
    cv::Mat processFrame(const cv::Mat& frame);
    void displayInfo(cv::Mat& frame, int frameCount) const;
};

#endif // VIDEO_PROCESSOR_H