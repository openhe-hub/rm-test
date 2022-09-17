#include <iostream>
#include <cmath>
#include <utility>
#include<opencv2/opencv.hpp>

#define DURATION 100
#define MIN_AREA1 90
#define MAX_AREA1 900
#define MAX_CIRCUMSTANCE1 800
#define MIN_CIRCUMSTANCE1 50
#define MAX_DISTANCE1 110

#define MIN_AREA2 600
#define MAX_AREA2 1200
#define MAX_CIRCUMSTANCE2 400
#define MIN_CIRCUMSTANCE2 50
#define MAX_DISTANCE2 220
#define MIN_DISTANCE2 150

#define MAX_DELTA_HEIGHT 50

#define Segment 67

struct Point_Data {
    cv::Point center_point;
    int x_min;
    int x_max;
    int y_min;
    int y_max;
};

void recognize(cv::Mat &src);

double calc_threshold(const std::vector<std::vector<cv::Point>> &contours);

bool judge_counter(const std::vector<cv::Point> &contour);

void print_all_counter_area(const std::vector<std::vector<cv::Point>> &contours);

void print_all_counter_circumstance(const std::vector<std::vector<cv::Point>> &contours);

std::string point_to_str(const cv::Point &p, std::string name);

std::vector<Point_Data> desc_contours(const std::vector<std::vector<cv::Point>> &contours);

double dist(cv::Point p1, cv::Point p2);

std::vector<std::vector<cv::Point>> filter(std::vector<Point_Data> point_data);

bool check_delta_height(Point_Data pd1,Point_Data pd2);

int cnt=1;
long frame_num;

int main() {
    cv::VideoCapture capture(R"(D:\program\cpp\rm-test\ex4\res\d.mp4)");
    frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
    long frame_width=capture.get(cv::CAP_PROP_FRAME_WIDTH);
    long frame_height=capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::Size size=cv::Size(frame_width,frame_height);
    double fps = capture.get(cv::CAP_PROP_FPS);
    cv::VideoWriter output=cv::VideoWriter(R"(D:\program\cpp\rm-test\ex4\output\d_res.avi)",
                                           cv::VideoWriter::fourcc('P','I','M','1'),
                                           fps,
                                           size);

    cv::Mat frame;

    while (true){
        capture>>frame;
        if (frame.empty()) break;
//        imshow("origin-frame",frame);
        recognize(frame);
        imshow("recognized-frame",frame);
//        cv::waitKey(DURATION);
        cnt++;
        output<<frame;
    }

    output.release();
    capture.release();
    return 0;
}

void recognize(cv::Mat &src) {
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

//    std::cout<<"Area:"<<std::endl;
//    print_all_counter_area(contours);
//    std::cout<<std::endl;
//    std::cout<<"Circumstance:"<<std::endl;
//    print_all_counter_circumstance(contours);
//    std::cout<<std::endl;


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
    cv::imshow("contours",drawer);

    //7. draw rectangle
    std::vector<Point_Data> desc= desc_contours(res);
    std::vector<std::vector<cv::Point>> targets=filter(desc);
    for (const auto &target: targets){
        cv::rectangle(drawer,target[0],target[3],cv::Scalar(255,255,0));
        cv::rectangle(src,target[0],target[3],cv::Scalar(255,255,0));
    }
}

bool judge_counter(const std::vector<cv::Point> &contour) {
    if (cnt<=Segment){
        return cv::contourArea(contour) >= MIN_AREA1 &&
               cv::contourArea(contour) <= MAX_AREA1 &&
               cv::arcLength(contour, true) <= MAX_CIRCUMSTANCE1 &&
               cv::arcLength(contour, true) >= MIN_CIRCUMSTANCE1;
    }else{
        return cv::contourArea(contour) >= MIN_AREA2 &&
               cv::contourArea(contour) <= MAX_AREA2 &&
               cv::arcLength(contour, true) <= MAX_CIRCUMSTANCE2 &&
               cv::arcLength(contour, true) >= MIN_CIRCUMSTANCE2;
    }

}

std::vector<std::vector<cv::Point>> filter(std::vector<Point_Data> point_data) {
    std::vector<std::vector<cv::Point>> res;
    for (int i = 0; i < point_data.size(); ++i) {
        for (int j = i+1; j < point_data.size(); ++j) {
            double dis=dist(point_data[i].center_point,point_data[j].center_point);
            if ((cnt<=Segment&&dis<=MAX_DISTANCE1)||(cnt>Segment&&dis<=MAX_DISTANCE2&&dis>=MIN_DISTANCE2)) {
                if (check_delta_height(point_data[i],point_data[j])){
                    int x1=std::min(point_data[i].x_min,point_data[j].x_min);
                    int x2=std::max(point_data[i].x_max,point_data[j].x_max);
                    int y1=std::min(point_data[i].y_min,point_data[j].y_min);
                    int y2=std::max(point_data[i].y_max,point_data[j].y_max);
                    std::vector<cv::Point> tar={{x1,y1},{x2,y1},{x1,y2},{x2,y2}};
                    res.push_back(tar);
                }
            }
        }
    }
    std::cout<<std::endl;
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
    return point_data;
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

bool check_delta_height(Point_Data pd1,Point_Data pd2){
    return std::max(abs(pd1.y_min-pd2.y_min),
             abs(pd1.y_max-pd2.y_max))<=MAX_DELTA_HEIGHT;
}
