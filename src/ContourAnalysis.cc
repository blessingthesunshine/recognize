#include "ContourAnalysis.h"
#include <iostream>
#include <algorithm>
#include <limits>

ContourAnalysis::ContourAnalysis() : minArea_(100) {}

void ContourAnalysis::setMinArea(int minArea) {
    minArea_ = minArea;
}

std::vector<std::vector<cv::Point>> ContourAnalysis::findWhiteContours(const cv::Mat& binaryImage) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    
    cv::findContours(binaryImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    std::vector<std::vector<cv::Point>> filteredContours;
    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area >= minArea_) {
            filteredContours.push_back(contour);
        }
    }
    
    return filteredContours;
}

BoundingInfo ContourAnalysis::mergeContoursInfo(const std::vector<std::vector<cv::Point>>& contours) {
    BoundingInfo info;
    
    if (contours.empty()) {
        info.boundingRect = cv::Rect(0, 0, 0, 0);
        info.area = 0;
        info.topPoint = cv::Point(0, 0);
        info.bottomPoint = cv::Point(0, 0);
        info.leftPoint = cv::Point(0, 0);
        info.rightPoint = cv::Point(0, 0);
        return info;
    }
    
    std::vector<cv::Point> allPoints;
    int totalArea = 0;
    
    for (const auto& contour : contours) {
        allPoints.insert(allPoints.end(), contour.begin(), contour.end());
        totalArea += static_cast<int>(cv::contourArea(contour));
    }
    
    auto minX = std::min_element(allPoints.begin(), allPoints.end(),
                                [](const cv::Point& a, const cv::Point& b) { return a.x < b.x; });
    auto maxX = std::max_element(allPoints.begin(), allPoints.end(),
                                [](const cv::Point& a, const cv::Point& b) { return a.x < b.x; });
    auto minY = std::min_element(allPoints.begin(), allPoints.end(),
                                [](const cv::Point& a, const cv::Point& b) { return a.y < b.y; });
    auto maxY = std::max_element(allPoints.begin(), allPoints.end(),
                                [](const cv::Point& a, const cv::Point& b) { return a.y < b.y; });
    
    info.leftPoint = *minX;
    info.rightPoint = *maxX;
    info.topPoint = *minY;
    info.bottomPoint = *maxY;
    
    info.boundingRect = cv::boundingRect(allPoints);
    
    info.corners = {
        cv::Point(info.boundingRect.x, info.boundingRect.y),
        cv::Point(info.boundingRect.x + info.boundingRect.width, info.boundingRect.y),
        cv::Point(info.boundingRect.x + info.boundingRect.width, info.boundingRect.y + info.boundingRect.height),
        cv::Point(info.boundingRect.x, info.boundingRect.y + info.boundingRect.height)
    };
    
    info.area = totalArea;
    
    return info;
}

BoundingInfo ContourAnalysis::analyzeWhiteRegions(const cv::Mat& binaryImage) {
    auto contours = findWhiteContours(binaryImage);
    std::cout << "找到 " << contours.size() << " 个白色区域轮廓" << std::endl;
    auto info = mergeContoursInfo(contours);
    if (info.area > 0) {
        std::cout << "白色区域分析: " << std::endl;
        std::cout << "  面积: " << info.area << " 像素" << std::endl;
        std::cout << "  矩形: [" << info.boundingRect.x << ", " << info.boundingRect.y 
                  << "] " << info.boundingRect.width << "x" << info.boundingRect.height << std::endl;
        std::cout << "  边界点 - 上:(" << info.topPoint.x << "," << info.topPoint.y 
                  << ") 下:(" << info.bottomPoint.x << "," << info.bottomPoint.y 
                  << ") 左:(" << info.leftPoint.x << "," << info.leftPoint.y 
                  << ") 右:(" << info.rightPoint.x << "," << info.rightPoint.y << ")" << std::endl;
    } else {
        std::cout << "未找到符合条件的白色区域" << std::endl;
    }
    
    return info;
}

void ContourAnalysis::drawAnalysisResult(cv::Mat& image, const BoundingInfo& info) {
    if (info.area == 0) {
        cv::putText(image, "No white region found", 
                   cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, 
                   cv::Scalar(0, 0, 255), 2);
        return;
    }
    
    cv::rectangle(image, info.boundingRect, cv::Scalar(0, 255, 0), 2);
    
    std::string rectInfo = "Rect: [" + std::to_string(info.boundingRect.x) + "," +
                          std::to_string(info.boundingRect.y) + "] " +
                          std::to_string(info.boundingRect.width) + "x" +
                          std::to_string(info.boundingRect.height);
    
    std::string areaInfo = "Area: " + std::to_string(info.area);
    
    cv::putText(image, rectInfo, cv::Point(10, 30), 
               cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(image, areaInfo, cv::Point(10, 60), 
               cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
}

// 新增的直接边界检测函数
SimpleBounds ContourAnalysis::findWhiteBoundsDirectly(const cv::Mat& binaryImage) {
    SimpleBounds bounds;
    bounds.found = false;
    bounds.whitePixelCount = 0;
    
    // 初始化边界点为极端值
    bounds.topPoint = cv::Point(-1, std::numeric_limits<int>::max());
    bounds.bottomPoint = cv::Point(-1, -1);
    bounds.leftPoint = cv::Point(std::numeric_limits<int>::max(), -1);
    bounds.rightPoint = cv::Point(-1, -1);
    
    // 遍历二值化图像的每个像素
    for (int y = 0; y < binaryImage.rows; ++y) {
        const uchar* row = binaryImage.ptr<uchar>(y);
        for (int x = 0; x < binaryImage.cols; ++x) {
            // 如果是白色像素 (255)
            if (row[x] == 255) {
                bounds.whitePixelCount++;
                bounds.found = true;
                
                if (y < bounds.topPoint.y) {
                    bounds.topPoint = cv::Point(x, y);
                }
                
                if (y > bounds.bottomPoint.y) {
                    bounds.bottomPoint = cv::Point(x, y);
                }
                
                if (x < bounds.leftPoint.x) {
                    bounds.leftPoint = cv::Point(x, y);
                }
                
                if (x > bounds.rightPoint.x) {
                    bounds.rightPoint = cv::Point(x, y);
                }
            }
        }
    }
    
    if (bounds.found) {
        int minX = bounds.leftPoint.x;
        int maxX = bounds.rightPoint.x;
        int minY = bounds.topPoint.y;
        int maxY = bounds.bottomPoint.y;
        
        bounds.boundsRect = cv::Rect(minX, minY, maxX - minX + 1, maxY - minY + 1);
    }
    
    // 调试信息
    if (bounds.found) {
        std::cout << "直接检测 - 白色像素: " << bounds.whitePixelCount 
                  << ", 边界矩形: " << bounds.boundsRect.width << "x" << bounds.boundsRect.height << std::endl;
    } else {
        std::cout << "直接检测 - 未找到白色像素" << std::endl;
    }
    
    return bounds;
}

void ContourAnalysis::drawSimpleBounds(cv::Mat& image, const SimpleBounds& bounds) {
    if (!bounds.found) {
        cv::putText(image, "No white pixels", 
                   cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, 
                   cv::Scalar(0, 0, 255), 2);
        return;
    }
    
    // 绘制边界矩形
    cv::rectangle(image, bounds.boundsRect, cv::Scalar(0, 255, 0), 2);

    
    std::string info = "Pixels: " + std::to_string(bounds.whitePixelCount) + 
                      " Rect: " + std::to_string(bounds.boundsRect.width) + 
                      "x" + std::to_string(bounds.boundsRect.height);
    
    cv::putText(image, info, cv::Point(10, 30), 
               cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
}