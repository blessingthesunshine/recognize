#include <iostream>
#include <string>
#include <fstream>
#include "VideoProcessor.h"

bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

int main(int argc, char** argv) {
    std::string videoPath;
    int brightnessThreshold = 100;
    VideoProcessor::ProcessingMode mode = VideoProcessor::DIRECT_BOUNDS;
    int minContourArea = 100;  

    if (argc > 1) {
        videoPath = argv[1];
        if (argc > 2) {
            brightnessThreshold = std::stoi(argv[2]);
        }
        if (argc > 3) {
            minContourArea = std::stoi(argv[3]);
        }
    } 
    else {
        videoPath = "/home/gsz/北洋机甲/recognize/videos/装甲板视频.mp4";
        
        if (!fileExists(videoPath)) {
            std::cout << "错误：视频文件不存在: " << videoPath << std::endl;
            std::cout << "请使用命令行参数指定视频文件路径：" << std::endl;
            std::cout << "用法: " << argv[0] << " <视频文件路径> [亮度阈值] [最小轮廓面积]" << std::endl;
            std::cout << "示例: " << argv[0] << " /home/gsz/北洋机甲/recognize/videos/装甲板视频.mp4 180 100" << std::endl;
            return -1;
        }
    }
    
    if (!fileExists(videoPath)) {
        std::cout << "错误：视频文件不存在: " << videoPath << std::endl;
        return -1;
    }
    
    VideoProcessor processor;
    processor.setBrightnessThreshold(brightnessThreshold);
    processor.setProcessingMode(mode);
    processor.setMinContourArea(minContourArea);
    
    std::cout << "开始处理视频: " << videoPath << std::endl;
    std::cout << "使用亮度阈值: " << processor.getBrightnessThreshold() << std::endl;
    
    std::string modeStr;
    switch (mode) {
        case VideoProcessor::BINARY_ONLY: modeStr = "二值化显示"; break;
        case VideoProcessor::CONTOUR_ANALYSIS: modeStr = "轮廓分析"; break;
        case VideoProcessor::DIRECT_BOUNDS: modeStr = "直接边界检测"; break;
    }
    std::cout << "处理模式: " << modeStr << std::endl;
    std::cout << "最小轮廓面积: " << minContourArea << std::endl;
    
    std::cout << "按键说明:" << std::endl;
    std::cout << "  '+' : 增加亮度阈值" << std::endl;
    std::cout << "  '-' : 减少亮度阈值" << std::endl;
    std::cout << "  'm' : 切换处理模式" << std::endl; 
    std::cout << "  'p' : 暂停/继续" << std::endl;
    std::cout << "  'q' 或 ESC : 退出" << std::endl;
    
    processor.processVideo(videoPath);
    
    return 0;
}