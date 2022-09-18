#include <iostream>
#include <vector>
#include <queue>
#include<opencv2/opencv.hpp>

#define AREA_THRESHOLD 1500
#define N 2

bool judge_counter(const std::vector<cv::Point> &contour, double threshold);

double calc_threshold(const std::vector<std::vector<cv::Point>> &contours);

void print_all_counter_area(const std::vector<std::vector<cv::Point>> &contours);

std::string point_to_str(const cv::Point &p, std::string name);


int main() {
    cv::Mat src = cv::imread(R"(D:\program\cpp\rm-test\ex4\res\d.jpg)");
    cv::imshow("src", src);
//    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\src.jpg)",src);

    //1. graying
    cv::Mat grey;
    cv::cvtColor(src, grey, cv::COLOR_BGR2GRAY);
//    cv::imshow("grey", grey);
//    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\grey.jpg)",grey);

    //2. binaryzation
    cv::Mat binary_img;
    cv::threshold(grey, binary_img, 100, 200, cv::THRESH_BINARY);
//    cv::imshow("binary_img", binary_img);
//    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\binary_img.jpg)",binary_img);

    //3. outline recognition
    cv::Mat canny_result;
    cv::Canny(binary_img, canny_result, 50, 100, 3);
//    cv::imshow("canny_result", canny_result);
//    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\canny_result.jpg)",canny_result);

    //4. get contour
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierachy;
    cv::findContours(canny_result, contours, hierachy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    //5. calc threshold area
    double threshold = calc_threshold(contours);

    //6. remove the disturbance
    std::vector<std::vector<cv::Point>> res;
    for (auto it = contours.begin(); it != contours.end(); it++) {
        if (judge_counter(*it, threshold)) {
            res.push_back(*it);
        }
    }

    //7. draw the contour
    cv::Mat drawer = cv::Mat::zeros(cv::Size(src.cols, src.rows), CV_8UC3);
    cv::drawContours(drawer, res, -1, {255, 255, 255}, 3);


    //8. draw the rectangle
    int x1 = 1e4, x2 = -1, y1 = 1e4, y2 = -1; // x1=min x,x2= max x,y1=min y,y2=max y
    for (auto sub: res) {
        for (auto point: sub) {
            x1 = std::min(x1, point.x);
            x2 = std::max(x2, point.x);
            y1 = std::min(y1, point.y);
            y2 = std::max(y2, point.y);
        }
    }
    cv::Point p1(x1, y1);
    cv::Point p2(x2, y1);
    cv::Point p3(x1, y2);
    cv::Point p4(x2, y2);
    cv::rectangle(src, p1, p4, cv::Scalar(255, 255, 0));
    cv::rectangle(drawer, p1, p4, cv::Scalar(255, 255, 0));

    //9. draw the text
    cv::putText(src, "TARGETING AREA:", {5, 20}, cv::FONT_HERSHEY_TRIPLEX, 0.8, {255, 0, 255}, 1, false);
    cv::putText(src, point_to_str(p1, "p1"), {10, 50}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, point_to_str(p2, "p2"), {10, 80}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, point_to_str(p3, "p3"), {10, 110}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, point_to_str(p4, "p4"), {10, 140}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, "p1", p1, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, "p2", p2, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, "p3", p3, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
    cv::putText(src, "p4", p4, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);

    cv::imshow("result", src);
    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\d-result.jpg)",src);
    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\res\d-result.jpg)",src);
//    cv::imshow("contours", drawer);
//    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\contours.jpg)",drawer);
    std::cout<<R"(you can find the final output at :\ex4\output\d-result.jpg)"<<std::endl;
    cv::waitKey(0);
    return 0;
}

double calc_threshold(const std::vector<std::vector<cv::Point>> &contours) {
    std::priority_queue<double> areas;
    for (const auto &item: contours) {
        areas.push(cv::contourArea(item));
    }
    int cnt = 0;
    double threshold = 1e7;
    while (!areas.empty() && (++cnt) <= N) {
        threshold = std::min(threshold, areas.top());
        areas.pop();
    }
    return threshold;
}

bool judge_counter(const std::vector<cv::Point> &contour, double threshold) {
    return cv::contourArea(contour) >= threshold;
}

// note that this function is just for viewing all the areas
// 1. calc contour areas
// 2. sort
// 3. get the threshold, here the threshold chosen is 1500
void print_all_counter_area(const std::vector<std::vector<cv::Point>> &contours) {
    std::priority_queue<double> areas;
    for (const auto &item: contours) {
        areas.push(cv::contourArea(item));
    }
    while (!areas.empty()) {
        std::cout << areas.top() << std::endl;
        areas.pop();
    }
}

std::string point_to_str(const cv::Point &p, std::string name) {
    std::string res = name + "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")";
    return res;
}


