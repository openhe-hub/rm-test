#include <iostream>
#include <cmath>
#include<opencv2/opencv.hpp>

#define DURATION 500
#define MIN_AREA 100
#define MAX_AREA 400
#define MAX_CIRCUMSTANCE 400
#define MIN_CIRCUMSTANCE 50

struct Point_Data {
    cv::Point center_point;
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};

cv::Mat &recognize(cv::Mat src);

double calc_threshold(const std::vector<std::vector<cv::Point>> &contours);

bool judge_counter(const std::vector<cv::Point> &contour);

void print_all_counter_area(const std::vector<std::vector<cv::Point>> &contours);

void print_all_counter_circumstance(const std::vector<std::vector<cv::Point>> &contours);

std::string point_to_str(const cv::Point &p, std::string name);

std::vector<Point_Data> desc_contours(const std::vector<std::vector<cv::Point>> &contours);

double dist(cv::Point p1, cv::Point p2);

std::vector<std::pair<cv::Point, cv::Point>> filter(std::vector<Point_Data> point_data);

int main() {
    cv::VideoCapture capture(R"(D:\program\cpp\rm-test\ex4\res\d.mp4)");
    long frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
    double fps = capture.get(cv::CAP_PROP_FPS);

    cv::Mat frame;
    capture >> frame;
    cv::Mat res = recognize(frame);
//    while (true){
//        capture>>frame;
//        if (frame.empty()) break;
//        recognize(frame);
//        imshow("origin-frame",frame);
////        imshow("res",res);
//        cv::waitKey(DURATION);
//    }

    capture.release();
    return 0;
}

cv::Mat &recognize(cv::Mat src) {
    cv::imshow("src", src);

    //1. graying
    cv::Mat grey;
    cv::cvtColor(src, grey, cv::COLOR_BGR2GRAY);
//    cv::imshow("grey", grey);

    //2. binaryzation
    cv::Mat binary_img;
    cv::threshold(grey, binary_img, 100, 200, cv::THRESH_BINARY);
//    cv::imshow("binary_img", binary_img);

    //3. outline recognition
    cv::Mat canny_result;
    cv::Canny(binary_img, canny_result, 50, 100, 3);
//    cv::imshow("canny_result", canny_result);

    //4. get contour
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierachy;
    cv::findContours(canny_result, contours, hierachy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);


    //5. remove the disturbance
    std::vector<std::vector<cv::Point>> res;
    for (auto it = contours.begin(); it != contours.end(); it++) {
        if (judge_counter(*it)) {
            res.push_back(*it);
        }
    }

    //6. draw the contour
    cv::Mat drawer = cv::Mat::zeros(cv::Size(src.cols, src.rows), CV_8UC3);
    cv::drawContours(drawer, res, -1, {255, 255, 255}, 3);
    cv::imshow("contours", drawer);

    auto desc= desc_contours(contours);
    filter(desc);


//
//    //7. draw the rectangle
//    int x1 = 1e4, x2 = -1, y1 = 1e4, y2 = -1; // x1=min x,x2= max x,y1=min y,y2=max y
//    for (auto sub: res) {
//        for (auto point: sub) {
//            x1 = std::min(x1, point.x);
//            x2 = std::max(x2, point.x);
//            y1 = std::min(y1, point.y);
//            y2 = std::max(y2, point.y);
//        }
//    }
//    cv::Point p1(x1, y1);
//    cv::Point p2(x2, y1);
//    cv::Point p3(x1, y2);
//    cv::Point p4(x2, y2);
//    cv::rectangle(src, p1, p4, cv::Scalar(255, 255, 0));
//    cv::rectangle(drawer, p1, p4, cv::Scalar(255, 255, 0));
//
//    //8. draw the text
//    cv::putText(src, "TARGETING AREA:", {5, 20}, cv::FONT_HERSHEY_TRIPLEX, 0.8, {255, 0, 255}, 1, false);
//    cv::putText(src, point_to_str(p1, "p1"), {10, 50}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
//    cv::putText(src, point_to_str(p2, "p2"), {10, 80}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
//    cv::putText(src, point_to_str(p3, "p3"), {10, 110}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
//    cv::putText(src, point_to_str(p4, "p4"), {10, 140}, cv::FONT_HERSHEY_TRIPLEX, 0.6, {0, 255, 255}, 1, false);
//
//    cv::imshow("src", src);
////    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\result.jpg)",src);
//    cv::imshow("contours", drawer);
////    cv::imwrite(R"(D:\program\cpp\rm-test\ex4\output\contours.jpg)",drawer);
//    std::cout<<R"(you can find the final output at :\ex4\output\contours.jpg)"<<std::endl;
    cv::waitKey(0);
}

bool judge_counter(const std::vector<cv::Point> &contour) {
    return cv::contourArea(contour) >= MIN_AREA &&
           cv::contourArea(contour) <= MAX_AREA &&
           cv::arcLength(contour, true) <= MAX_CIRCUMSTANCE &&
           cv::arcLength(contour, true) >= MIN_CIRCUMSTANCE;
}

std::vector<std::pair<cv::Point, cv::Point>> filter(std::vector<Point_Data> point_data) {
    std::vector<std::pair<cv::Point, cv::Point>> res;
    for (int i = 0; i < point_data.size(); ++i) {
        for (int j = i+1; j < point_data.size(); ++j) {
            double dis=dist(point_data[i].center_point,point_data[j].center_point);
            std::cout<<dis<<std::endl;
        }
    }
    return res;
}

std::vector<Point_Data> desc_contours(const std::vector<std::vector<cv::Point>> &contours) {
    std::vector<Point_Data> point_data;
    for (const auto &contour: contours) {
        int x1 = 1e4, x2 = -1, y1 = 1e4, y2 = -1, x0 = 0, y0 = 0;
        for (const auto &point: contour) {
            x1 = std::min(x1, point.x);
            x2 = std::max(x2, point.x);
            y1 = std::min(y1, point.y);
            y2 = std::max(y2, point.y);
        }
        x0=(x1+x2)>>1;
        y0=(y1+y2)>>1;
        point_data.push_back({{x0,y0},x1,x2,y1,y2});
    }
}

double dist(cv::Point p1, cv::Point p2) {
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

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

void print_all_counter_circumstance(const std::vector<std::vector<cv::Point>> &contours) {
    std::priority_queue<double> circumstances;
    for (const auto &item: contours) {
        circumstances.push(cv::arcLength(item, true));
    }
    while (!circumstances.empty()) {
        std::cout << circumstances.top() << std::endl;
        circumstances.pop();
    }
}
