#include "CamVideoStitch.h"

int main()
{
	CamVideoStitch d;
	d.CalcHMatrix();
	d.VideoStitch();
	return 0;
}