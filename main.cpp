#include "cpu.h"
#include "cuda.h"
#include "kernel.h"
#include "common.h"

int main()
{
	cv ::Mat image;
	uchar3 color = {255, 255, 0};
	uint16_t threshold = 90;
  	image = cv ::imread("C:/opencv/Pikcha.jpg");
    //image = cv ::imread("C:/opencv/182.jpg");
  if (!image.data)
  {
    printf("No image data \n");
    return 0;
  }
  //cv :: Mat result = rotatePicture182(image);
  cv::Mat tresholdResult = tresholdTransformCPU(image, color, threshold);
  cv::Mat houghResult = houghTransformCPUZ(tresholdResult);
  //cv::Mat rotateResult = rotatePicturePikcha(image);
  


  cv ::namedWindow("Display Image", cv ::WINDOW_AUTOSIZE);
  imshow("Display Image", image);
  cv :: waitKey(0);

  cv ::namedWindow("Treshold Result", cv ::WINDOW_AUTOSIZE);
  cv :: imshow("Treshold Result", tresholdResult);
  cv :: waitKey(0);

  //cv ::namedWindow("Spinned", cv ::WINDOW_AUTOSIZE);
  //cv :: imshow("Spinned", rotateResult);
  //cv :: waitKey(0);


	return 0;
}
