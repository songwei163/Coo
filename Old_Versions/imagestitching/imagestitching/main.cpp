#include "imageStitch.h"

int main()
{
	StitchImage is;
	is.LoadImage("C:/Users/S/Desktop/≤‚ ‘Õº∆¨/”“3.jpg", "C:/Users/S/Desktop/≤‚ ‘Õº∆¨/◊Û3.jpg");

	is.PreProcess();
	is.DetectFeaturePoint();
	is.CalcuFeatureVector();
	is.ImageMatch();
	is.FindHomography();
	is.WrapImage();
	
	waitKey(0);
	return 0;
}