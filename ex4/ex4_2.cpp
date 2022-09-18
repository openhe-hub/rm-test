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

#define MIN_AREA2 300
#define MAX_AREA2 1200
#define MAX_CIRCUMSTANCE2 400
#define MIN_CIRCUMSTANCE2 50
#define MAX_DISTANCE2 220
#define MIN_DISTANCE2 100

#define MAX_DELTA_HEIGHT 50
#define MAX_DELTA_WIDTH1 90
#define MAX_DELTA_WIDTH2 190

#define FPS 60.0


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

bool check_delta_height(Point_Data pd1, Point_Data pd2);

bool check_delta_width(Point_Data pd1, Point_Data pd2);

int cnt = 1;
long frame_num;
int lst_frame = 0.0;
double w = 0.0;
std::vector<cv::Point> lst;

int main() {
    cv::VideoCapture capture(R"(D:\program\cpp\rm-test\ex4\res\d.mp4)");
    frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
    long frame_width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    long frame_height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::Size size = cv::Size(frame_width, frame_height);
    double fps = capture.get(cv::CAP_PROP_FPS);
    cv::VideoWriter output = cv::VideoWriter(R"(D:\program\cpp\rm-test\ex4\output\d-result.avi)",
                                             cv::VideoWriter::fourcc('P', 'I', 'M', '1'),
                                             fps,
                                             size);

    cv::Mat frame;

    while (true) {
        capture >> frame;
        if (frame.empty()) break;
        imshow("origin-frame", frame);
        recognize(frame);
        imshow("recognized-frame", frame);
        cv::waitKey(DURATION);
        cnt++;
        output<<frame;
    }

    std::cout
            << R"(you can find the final output at :\ex4\output\d-result.mp4(converted) or \ex4\output\d-result.avi(original))"
            << std::endl;
    std::cout << R"(you can find a slow version at :\ex4\output\d-result-slow.mp4(converted))" << std::endl;

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
//    cv::imshow("contours", drawer);

    //7. draw rectangle & add text
    std::vector<Point_Data> desc = desc_contours(res);
    std::vector<std::vector<cv::Point>> targets = filter(desc);
    if (!targets.empty()) {
        cv::putText(src, "TARGETING AREA", {5, 20}, cv::FONT_HERSHEY_TRIPLEX, 0.8, {255, 0, 255}, 1, false);
    }
    int group_num = 1;
    for (const auto &target: targets) {
        cv::rectangle(drawer, target[0], target[3], cv::Scalar(255, 255, 0));
        cv::rectangle(src, target[0], target[3], cv::Scalar(255, 255, 0));
        cv::putText(src, "Group " + std::to_string(group_num), {5, (group_num - 1) * 150 + 50},
                    cv::FONT_HERSHEY_TRIPLEX, 0.8, {255, 0, 255}, 1, false);
        cv::putText(src, point_to_str(target[0], "p1"), {10, group_num * 150 - 70}, cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, point_to_str(target[1], "p2"), {10, group_num * 150 - 40}, cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, point_to_str(target[2], "p3"), {10, group_num * 150 - 10}, cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, point_to_str(target[3], "p4"), {10, group_num * 150 + 20}, cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, "G" + std::to_string(group_num) + "p1", target[0], cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, "G" + std::to_string(group_num) + "p2", target[1], cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, "G" + std::to_string(group_num) + "p3", target[2], cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        cv::putText(src, "G" + std::to_string(group_num) + "p4", target[3], cv::FONT_HERSHEY_TRIPLEX, 0.6,
                    {0, 255, 255}, 1, false);
        group_num++;
    }

    //8. calc w
    if (lst.empty() && targets.size() >= 2) {
        lst_frame = cnt;
        int avg_p1_x = ((targets[0][0].x + targets[0][1].x) / 2 + (targets[0][2].x + targets[0][3].x) / 2) / 2;
        int avg_p2_x = ((targets[1][0].x + targets[1][1].x) / 2 + (targets[1][2].x + targets[1][3].x) / 2) / 2;
        int avg_p1_y = ((targets[0][0].y + targets[0][2].y) / 2 + (targets[0][1].y + targets[0][3].y) / 2) / 2;
        int avg_p2_y = ((targets[1][0].y + targets[1][2].y) / 2 + (targets[1][1].y + targets[1][3].y) / 2) / 2;
        lst.emplace_back(avg_p1_x, avg_p1_y);
        lst.emplace_back(avg_p2_x, avg_p2_y);
    } else if (!lst.empty() && targets.size() >= 2) {
        int delta_frame = cnt - lst_frame;
        double delta_t = (double)(delta_frame / FPS) * 0x01;
        cv::Point center_point = {(lst[0].x + lst[1].x + lst[0].y - lst[1].y) / 2,
                                  (lst[1].x - lst[0].x + lst[1].y + lst[1].y) / 2};
        double R = (dist(center_point, lst[0]) + dist(center_point, lst[1])) / 2;
        int avg_p1_x = ((targets[0][0].x + targets[0][1].x) / 2 + (targets[0][2].x + targets[0][3].x) / 2) / 2;
        int avg_p2_x = ((targets[1][0].x + targets[1][1].x) / 2 + (targets[1][2].x + targets[1][3].x) / 2) / 2;
        int avg_p1_y = ((targets[0][0].y + targets[0][2].y) / 2 + (targets[0][1].y + targets[0][3].y) / 2) / 2;
        int avg_p2_y = ((targets[1][0].y + targets[1][2].y) / 2 + (targets[1][1].y + targets[1][3].y) / 2) / 2;
        cv::Point p3 = {avg_p1_x, avg_p1_y};
        cv::Point p4 = {avg_p2_x, avg_p2_y};
        cv::Point p1 = lst[0];
        cv::Point p2 = dist(p1, p3) <= dist(p1, p4) ? p3 : p4;
        double dis = dist(p2, p1);
        double delta_theta = acos((2 * R * R - dis * dis) / (2 * R * R));
        w = delta_theta / delta_t;
        cv::putText(src, "Angular Velocity: w="+ std::to_string(w), {250, 20}, cv::FONT_HERSHEY_TRIPLEX, 0.8, {0, 0, 255}, 1, false);
        //update
        lst_frame=cnt;
        lst[0]=p3;
        lst[1]=p4;
    } else if (targets.size() <= 1 && w) {
        cv::putText(src, "Angular Velocity w="+ std::to_string(w), {250, 20}, cv::FONT_HERSHEY_TRIPLEX, 0.8, {0, 0, 255}, 1, false);
    }
}

bool judge_counter(const std::vector<cv::Point> &contour) {
    if (cnt <= Segment) {
        return cv::contourArea(contour) >= MIN_AREA1 &&
               cv::contourArea(contour) <= MAX_AREA1 &&
               cv::arcLength(contour, true) <= MAX_CIRCUMSTANCE1 &&
               cv::arcLength(contour, true) >= MIN_CIRCUMSTANCE1;
    } else {
        return cv::contourArea(contour) >= MIN_AREA2 &&
               cv::contourArea(contour) <= MAX_AREA2 &&
               cv::arcLength(contour, true) <= MAX_CIRCUMSTANCE2 &&
               cv::arcLength(contour, true) >= MIN_CIRCUMSTANCE2;
    }

}

std::vector<std::vector<cv::Point>> filter(std::vector<Point_Data> point_data) {
    std::vector<std::vector<cv::Point>> res;
    for (int i = 0; i < point_data.size(); ++i) {
        for (int j = i + 1; j < point_data.size(); ++j) {
            double dis = dist(point_data[i].center_point, point_data[j].center_point);
            if ((cnt <= Segment && dis <= MAX_DISTANCE1) ||
                (cnt > Segment && dis <= MAX_DISTANCE2 && dis >= MIN_DISTANCE2)) {
                if (check_delta_height(point_data[i], point_data[j]) &&
                    check_delta_width(point_data[i], point_data[j])) {
                    int x1 = std::min(point_data[i].x_min, point_data[j].x_min);
                    int x2 = std::max(point_data[i].x_max, point_data[j].x_max);
                    int y1 = std::min(point_data[i].y_min, point_data[j].y_min);
                    int y2 = std::max(point_data[i].y_max, point_data[j].y_max);
                    std::vector<cv::Point> tar = {{x1, y1},
                                                  {x2, y1},
                                                  {x1, y2},
                                                  {x2, y2}};
                    res.push_back(tar);
                }
            }
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
        x0 = (x1 + x2) >> 1;
        y0 = (y1 + y2) >> 1;
        point_data.push_back({{x0, y0}, x1, x2, y1, y2});
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

bool check_delta_height(Point_Data pd1, Point_Data pd2) {
    return std::max(abs(pd1.y_min - pd2.y_min),
                    abs(pd1.y_max - pd2.y_max)) <= MAX_DELTA_HEIGHT;
}

bool check_delta_width(Point_Data pd1, Point_Data pd2) {
    if (cnt <= Segment) {
        return std::max(abs(pd1.x_min - pd2.x_max), abs(pd2.x_min - pd2.x_max)) <= MAX_DELTA_WIDTH1;
    } else {
        return std::max(abs(pd1.x_min - pd2.x_max), abs(pd2.x_min - pd2.x_max)) <= MAX_DELTA_WIDTH2;
    }

}

std::string point_to_str(const cv::Point &p, std::string name) {
    std::string res = name + "(" + std::to_string(p.x) + "," + std::to_string(p.y) + ")";
    return res;
}
