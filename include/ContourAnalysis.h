#ifndef CONTOUR_ANALYSIS_H
#define CONTOUR_ANALYSIS_H

#include <opencv2/opencv.hpp>
#include <vector>

struct BoundingInfo {
    cv::Rect boundingRect;
    std::vector<cv::Point> corners;
    cv::Point topPoint;
    cv::Point bottomPoint;
    cv::Point leftPoint;
    cv::Point rightPoint;
    int area;
};

struct SimpleBounds {
    cv::Point topPoint;     // 最上白点
    cv::Point bottomPoint;  // 最下白点  
    cv::Point leftPoint;    // 最左白点
    cv::Point rightPoint;   // 最右白点
    cv::Rect boundsRect;    // 边界矩形
    bool found;             // 是否找到白点
    int whitePixelCount;    // 白色像素数量
};

class ContourAnalysis {
public:
    ContourAnalysis();
    
    // 原有函数
    BoundingInfo analyzeWhiteRegions(const cv::Mat& binaryImage);
    void drawAnalysisResult(cv::Mat& image, const BoundingInfo& info);
    void setMinArea(int minArea);
    
    // 新增直接查找边界点的函数
    SimpleBounds findWhiteBoundsDirectly(const cv::Mat& binaryImage);
    void drawSimpleBounds(cv::Mat& image, const SimpleBounds& bounds);
    
private:
    int minArea_;
    std::vector<std::vector<cv::Point>> findWhiteContours(const cv::Mat& binaryImage);
    BoundingInfo mergeContoursInfo(const std::vector<std::vector<cv::Point>>& contours);
};

#endif // CONTOUR_ANALYSIS_H