#include "cpu.h"
#include "common.h"

Point getRotatedPoint(Point point, int angle){
    Point result;
    result.x = point.x * cos(angle * CV_PI / 180) - point.y * sin(angle * CV_PI / 180);
    result.y = point.y * cos(angle * CV_PI / 180) + point.x * sin(angle * CV_PI / 180);
    return result;
}

template <typename T>
pictureParams pictureSize(const std::vector<Point> v, const T& functor) {
    const auto minValue = *std::min_element(v.begin(), v.end(), [=](const auto& x, const auto& y){
        return functor(x) < functor(y);
    });
    const auto maxValue = *std::max_element(v.begin(), v.end(), [=](const auto& x, const auto& y) {
		return functor(x) < functor(y);
	});
	return { functor(maxValue), functor(minValue) };
}

cv::Mat rotatePicture182(const cv::Mat& src){
    std::cout << src.rows << std :: endl;
    std::cout << src.cols << std :: endl;

    std::vector<Point> anglePoints = {getRotatedPoint({0, 0}, ANGLE), getRotatedPoint({0, src.cols + 1}, ANGLE),
        getRotatedPoint({src.rows, 0}, ANGLE), getRotatedPoint({src.rows, src.cols + 1}, ANGLE)};

    pictureParams pictureWidth = pictureSize(anglePoints, [](const Point& v){
        return v.x;
    });

    pictureParams pictureHeight = pictureSize(anglePoints, [](const Point& v){
        return v.y;
    });

    std::cout << pictureHeight.getSize() << std :: endl;
    std::cout << pictureWidth.getSize() << std :: endl;

    cv::Mat out(pictureHeight.getSize() * 2, pictureWidth.getSize() * 2, CV_8UC3);
    //cv::Mat out(2 * src.rows, 2 * src.cols, CV_8UC3);
    out = 0;
    for(int i = 0; i < src.rows; i++){
        
        for(int j = 0; j < src.cols; j++){
           
            Point point = {i - pictureHeight.getSize() / 16, j - pictureWidth.getSize()};

            Point procPoint = getRotatedPoint(point, ANGLE);
            out.at<uchar3>(procPoint.x + pictureWidth.getSize() / 16 - 150, procPoint.y + pictureHeight.getSize()) =
                src.at<uchar3>(i, j); 
        }
    }
    //cv ::namedWindow("Spinned", cv ::WINDOW_AUTOSIZE);
    //cv :: imshow("Spinned", out);
    //cv :: waitKey(0);
    return out;
}

cv::Mat rotatePicturePikcha(const cv::Mat& src){

    std::cout << src.rows << std :: endl;
    std::cout << src.cols << std :: endl;

    std::vector<Point> anglePoints = {getRotatedPoint({0, 0}, ANGLE), getRotatedPoint({src.cols, 0}, ANGLE),
        getRotatedPoint({0, src.rows + 1}, ANGLE), getRotatedPoint({src.cols, src.rows + 1}, ANGLE)};

    pictureParams pictureHeight = pictureSize(anglePoints, [](const Point& v){
        return v.y;
    });

    pictureParams pictureWidth = pictureSize(anglePoints, [](const Point& v){
        return v.x;
    });

    std::cout << pictureHeight.getSize() << std :: endl;
    std::cout << pictureWidth.getSize() << std :: endl;
    
    cv::Mat out(pictureHeight.getSize() * 2, pictureWidth.getSize() * 2, CV_8UC3);
    out = 0;
    for(int i = 0; i < src.rows; i++){
        
        for(int j = 0; j < src.cols; j++){
            Point point = {j - pictureWidth.getSize(), i - pictureHeight.getSize()};
            Point procPoint = getRotatedPoint({point.x, point.y}, ANGLE);
            out.at<uchar3>(procPoint.y + pictureHeight.getSize() + 400, procPoint.x + pictureWidth.getSize() - 200) =//+600 -600 (30 deg)
                src.at<uchar3>(i, j);
        }
    }
    //cv ::namedWindow("Spinned", cv ::WINDOW_AUTOSIZE);
    //cv :: imshow("Spinned", out);
    //cv :: waitKey(0);
    return out;
}

//x * cos(f) + y * sin(f) = R                уравнение прямой
/*cv::Mat houghTransformCPU(const cv::Mat& src){
    std::cout << src.rows << std :: endl;
    std::cout << src.cols << std :: endl;

    int RMax = cvRound(sqrt((double) (src.cols * src.cols + src.rows * src.rows)));

    cv::Mat phase(RMax, 360, CV_8UC1);
    phase = 0;
    
    for(int y = 0; y < src.rows; y++){
        for(int x = -180; x < src.cols; x++){
            if(src.at<uint8_t>(y, x) > 0){
                for(int f = -180; f < 180; f++){
                    int theta = f * CV_PI / 180;
                    int r = abs((y * cos(theta) + x * sin(theta))); 
                    phase.at<uint8_t>(r, 180 + f) += 1;    
                }
            }
        }
    }

    unsigned int maxPhaseValue = 0;
    float theta = 0;
    int R = 0;

    for(int r = 0; r < RMax; r++){
        for(int f = -180; f < 180; f++){
            if(phase.at<uint8_t>(r, 180 + f) > maxPhaseValue){
                maxPhaseValue = phase.at<uint8_t>(r, 180 + r);
                theta = 180 + f;
                R = r;
            }
        }
    }
    std::cout << maxPhaseValue << "\t" << theta << "\t" << R << std::endl;

    //cv::Mat phaseImage(phase.rows, phase.cols * 3, CV_8UC1);
    //cv::resize(phase, phaseImage, {phase.rows, phase.cols * 3});
    //cv ::namedWindow("PhaseImage Image", cv ::WINDOW_AUTOSIZE);
    //imshow("PhaseImage Image", phaseImage);
    //cv :: waitKey(0);
    return phase;
}*/

template <class Value>
int Sign(Value Val) {
    if (Val == 0.)  return 0;
    if (Val > 0.)  return 1;
    else return -1;
}

void incrementAccumulator(int*** accumulator, int y, int x, int h, int accumulatorHeight, int accumulatorWidth){
    if(y < 0 || y > accumulatorHeight || x < 0 || x > accumulatorWidth){
        return;
    }
    accumulator[y][x][h] += 1;
}

void checkSample(int*** accumulator, int y, int x, int h, int ratio, int accumulatorHeight, int accumulatorWidth){
    int step = 512;
    float dAngle = 2 * CV_PI / step;
    int width = h * ratio;
    int r = sqrt(width * width + h * h);
   
    for(int i = 0; i < step; i++){
        float angle = i * dAngle;
        int dh = r * sin(angle);
        dh = cv::min(abs(dh), h) *  Sign(dh);

        int dWidth = r * cos(angle);
        dWidth = cv::min(abs(dWidth), width) * Sign(dWidth);

        int x0 = x + dWidth;
        int y0 = y + dh;
        incrementAccumulator(accumulator, y0, x0, h, accumulatorHeight, accumulatorWidth);
    }
    
}

cv::Mat houghTransformCPUZ(const cv::Mat& src){
    //cv::Mat accumulator(src.rows, src.cols, CV_8UC1);
    int ratio = 2;
    int maxHeightFromWidth = src.cols / 2 / ratio;
    int maxHeight = cv::min(maxHeightFromWidth, src.rows / 2);
    maxHeight /= 2;
    int*** accumulator = nullptr;

    accumulator = new int** [src.rows];
    for(int y = 0; y < src.rows; y++){
        accumulator[y] = new int* [src.cols];
        for(int x = 0; x < src.cols; x++){
            accumulator[y][x] = new int [maxHeight];
        }
    }
   
    for(int y = 0; y < src.rows; y++){
        for(int x = 0; x < src.cols; x++){
            for(int h = 0; h < maxHeight; h++){
                accumulator[y][x][h] = 0;
            }
        }
    }
    std::cout << src.rows << std::endl;
    std::cout << src.cols << std::endl;
    std::cout << maxHeight << std::endl;
   
    for(int y = 0; y < src.rows; y++){
        for(int x = 0; x < src.cols; x++){
            if(src.at<uint8_t>(y, x) > 0){
                for(int h = 2; h < maxHeight; h++){
                    checkSample(accumulator, y, x, h, ratio, src.rows, src.cols);
                    //std::cout << "Iteration " << y << "\t" << x << "\t" << h << std::endl;
                }
            }
        }
    }
   
    int maxValue = 0;
    int yValue = 0;
    int xValue = 0;
    int hValue = 0;
    
    for(int y = 0; y < src.rows; y++){
        for(int x = 0; x < src.cols; x++){
            for(int h = 2; h < maxHeight; h++){
                if(accumulator[y][x][h] > maxValue){
                    maxValue = accumulator[y][x][h];
                    yValue = y;
                    xValue = x;
                    hValue = h;
                    
                }       
            }
        }
    }

    std::cout << "X = " << xValue << std::endl;
    std::cout << "Y = " << yValue << std::endl;
    std::cout << "H = " << hValue << std::endl;
    return src;
}


cv::Mat houghTransformCPU(const cv::Mat& src){
    std::cout << src.rows << std :: endl;
    std::cout << src.cols << std :: endl;

    int RMax = cvRound(sqrt((double) (src.cols * src.cols + src.rows * src.rows)));

    cv::Mat phase(RMax, 360, CV_8UC1);
    phase = 0;
    
    for(int y = 0; y < src.rows; y++){
        for(int x = -180; x < src.cols; x++){
            if(src.at<uint8_t>(y, x) > 0){
                for(int f = -180; f < 180; f++){
                    int theta = f * CV_PI / 180;
                    int r = abs((y * cos(theta) + x * sin(theta))); 
                    phase.at<uint8_t>(r, 180 + f) += 1;    
                }
            }
        }
    }

    unsigned int maxPhaseValue = 0;
    float theta = 0;
    int R = 0;

    for(int r = 0; r < RMax; r++){
        for(int f = -180; f < 180; f++){
            if(phase.at<uint8_t>(r, 180 + f) > maxPhaseValue){
                maxPhaseValue = phase.at<uint8_t>(r, 180 + r);
                theta = 180 + f;
                R = r;
            }
        }
    }
    std::cout << maxPhaseValue << "\t" << theta << "\t" << R << std::endl;

    //cv::Mat phaseImage(phase.rows, phase.cols * 3, CV_8UC1);
    //cv::resize(phase, phaseImage, {phase.rows, phase.cols * 3});
    //cv ::namedWindow("PhaseImage Image", cv ::WINDOW_AUTOSIZE);
    //imshow("PhaseImage Image", phaseImage);
    //cv :: waitKey(0);
    return phase;
}

cv::Mat tresholdTransformCPU(const cv::Mat& src, uchar3 color, uint16_t threshold) {
    cv::Mat out3(src.rows, src.cols, CV_8UC3);
    const cv::Scalar clr{(float)color.z, (float)color.y, (float)color.x};
    cv::Mat out(src.rows, src.cols, CV_8UC1);
    out = 0;
    {
        //Timer t("CPU binarization");
        cv::absdiff(src, clr, out3);
        for (int i = 0; i < src.rows; ++i) {
            for (int j = 0; j < src.cols; ++j) {
                uchar3 p = out3.at<uchar3>(i, j);
                int val = p.x + p.y + p.z;
                out.at<uint8_t>(i, j) = val <= threshold;
            }
        }
        cv ::namedWindow("Intermediate Treshold", cv ::WINDOW_AUTOSIZE);
        cv :: imshow("Intermediate Treshold", out3);
        cv :: waitKey(0);
    
    }
    return out * 255;
}
