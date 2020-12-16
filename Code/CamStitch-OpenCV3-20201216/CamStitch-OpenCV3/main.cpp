#include "CamVideoStitch.h"

int main()
{
	CamVideoStitch d;
	d.CalcHMatrix();
	d.CameraStitch();
	
	// d.CalcHMatrix("C:\\Users\\S\\Videos\\cam2.avi", "C:\\Users\\S\\Videos\\cam1.avi");
	// d.VideoStitch();
	return 0;
}