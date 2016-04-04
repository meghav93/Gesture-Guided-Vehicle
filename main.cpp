#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <windows.h>
#include <dos.h>
#include<String.h>
using namespace cv;
using namespace std;
bool pr=true;
bool sclick=false;
int i=0;
int j=0;
int n=0;
int k=0;
int back=0;
int slopeint=0;
double area1=0,area2=0,avgarea=0;
float a=1366/450;
float b=768/315;
int c=0,d=0;
char key=0;
float p1=0,q1=0,p2=0,q2=0;
int dis=0;
char test1[3]="";
bool objectFound = false;
bool objectFoundred= false;
bool objectFoundblue= false;
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_NUM_OBJECTS=50;
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";

string intToString(int number)
{	std::stringstream ss;
	ss << number;
	return ss.str();
}
void inttochar(int dis,int slopeint,int back)
{
    int p;

    for(int k=5;k>=3;k--)
    {    p=slopeint%10;
         slopeint/=10;
         test1[k]=p+'0';
    }

    for(int k=2;k>=0;k--)
    {    p=dis%10;
         dis/=10;
         test1[k]=p+'0';
    }


    test1[6]=back+'0';
    test1[7]='\0';
}

void drawObject(int x, int y,Mat &frame)
{
	circle(frame,Point(x,y),20,Scalar(0,255,0),2);
    if(y-25>0)
    line(frame,Point(x,y),Point(x,y-25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,0),Scalar(0,255,0),2);
    if(y+25<FRAME_HEIGHT)
    line(frame,Point(x,y),Point(x,y+25),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(x,FRAME_HEIGHT),Scalar(0,255,0),2);
    if(x-25>0)
    line(frame,Point(x,y),Point(x-25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(0,y),Scalar(0,255,0),2);
    if(x+25<FRAME_WIDTH)
    line(frame,Point(x,y),Point(x+25,y),Scalar(0,255,0),2);
    else line(frame,Point(x,y),Point(FRAME_WIDTH,y),Scalar(0,255,0),2);
	if(i==1)
    putText(frame,"BLUE",Point(x,y+30),1,1,Scalar(0,255,0),2);
    if(i==2)
    putText(frame,"RED",Point(x,y+30),1,1,Scalar(0,255,0),2);
}
void morphOps(Mat &thresh)
{
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
{
	Mat temp;
	threshold.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
	double refArea = 0;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();

        if(numObjects<MAX_NUM_OBJECTS){
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

                if(area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
					x = moment.m10/area;
					y = moment.m01/area;
					if(i==1)
					objectFoundblue = true,objectFound=true,p1=x,q1=y,area1=area;
					else if(i==2)
					objectFoundred = true,objectFound=true,p2=x,q2=y,area2=area;



                    avgarea=(area1+area2)/2;

				}else
				{objectFoundred = false,objectFoundblue = false,objectFound =false;}


			}

			}
			if(objectFoundblue ==true)
			{

				putText(cameraFeed,"Tracking Object blue",Point(0,50),2,1,Scalar(0,255,0),2);
				drawObject(x,y,cameraFeed);
            }
            if(objectFoundred ==true)
			{

				putText(cameraFeed,"Tracking Object Red",Point(0,50),2,1,Scalar(0,255,0),2);
				drawObject(x,y,cameraFeed);
            }

	}
}

int main(int argc, char* argv[])
{

    HANDLE hDevice = CreateFile("COM8",GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,0);
    if (hDevice !=INVALID_HANDLE_VALUE)
    {
        printf("Port opened! \n");
        DCB lpTest;
        GetCommState(hDevice,&lpTest);
        lpTest.BaudRate = CBR_115200;
        lpTest.ByteSize = 8;
        lpTest.Parity = NOPARITY;
        lpTest.StopBits = ONESTOPBIT;
        SetCommState(hDevice,&lpTest);

        Sleep(2000);
        //std::cin>>test;
        //Sleep(2000);

       // CloseHandle(hDevice);
    }
    else
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			printf("Serial port doesn't exist! \n");
		}

		printf("Error while setting up serial port! \n");
	}




	bool nk = false;
    bool blue=false;
	Mat cameraFeed;
	Mat HSV;
    Mat threshold;
	int x=0, y=0;

	VideoCapture capture;
	capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);

	while(key!=27)
	{
		capture.read(cameraFeed);
		cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

		for(i=1;i<=2;i++)
		{

                if(i==1)
                {
                    //inRange(HSV,Scalar(96,97,97),Scalar(148,211,256),threshold);//FOR BLUE_DAY
                    inRange(HSV,Scalar(106,102,54),Scalar(157,256,256),threshold);//FOR BLUE_DAY
                   // inRange(HSV,Scalar(171,81,68),Scalar(177,245,256),threshold);//for RED_day
                }
                if(i==2)
                {
                     //inRange(HSV,Scalar(96,97,97),Scalar(148,211,256),threshold);//FOR BLUE_DAY
                  //  inRange(HSV,Scalar(171,81,68),Scalar(177,245,256),threshold);//for RED_day
                    inRange(HSV,Scalar(135,92,47),Scalar(197,181,181),threshold);
                   //inRange(HSV,Scalar(167,40,94),Scalar(218,192,158),threshold);//for RED_day

                }
//cout<<"(p1,q1) =  "<<p1<<" , "<<q1<<"     (p2,q2) =  "<<p2<<" , "<<q2<<endl;
            morphOps(threshold);
			trackFilteredObject(x,y,threshold,cameraFeed);

			if(objectFound==true)
			{         cout<<"angle = "<<slopeint<<" dis = "<<dis<<" back = "<<back<<endl;
                      inttochar(dis,slopeint,back);
                      DWORD btsIO;
                      WriteFile(hDevice,test1,strlen(test1),&btsIO,NULL);
			}

                if(objectFoundblue ==true&&i==1)
                {
                    line(cameraFeed,Point(p1,q1),Point(p2,q2),Scalar(0,255,0),2);
                    static float slope;
                    //slope=(q2-q1)/(p2-p1);
                    slope=atan2((q2-q1),(p2-p1));
                    slope=slope*180/3.14;
                    slopeint=slope+200;
                    //cout<<"slope = "<<slopeint<<endl;
                    objectFound =false;
                    nk=true;
                }
                if(objectFoundred==true&&i==2&&nk==true&&objectFoundblue ==true)//&&k>=2)
                {
                   dis=sqrt((p1-p2)*(p1-p2)+(q1-q2)*(q1-q2));
                   dis+=200;
                   //cout<<"DISTANCE = "<<dis<<endl;

                   //std::cout<<"DISTANCE = "<<dis<<endl;
                   //std::cout<<"   area = "<<avgarea<<endl;
                   static int ratio=200;
                   static int avgdis=0;
                  // ratio=avgarea/dis;
                   //std::cout<<"  avgdis = "<<avgdis<<endl;
                  avgdis=avgarea/ratio;

                           if(dis<260&&dis>0&&n>=9)
                           {

                                        if(pr==true)
                                        {   j++;
                                                if(j==1)
                                                {
                                                    std::cout<<"CLICK PRESSED"<<endl;
                                                    back=1;
                                                }
                                                if(j==2)
                                                {   std::cout<<"CLICK RELEASED"<<endl;
                                                    back=0;
                                                    j=0;
                                                }
                                            n=0;
                                        }

                           }
                           else
                           {
                               n++;
                           }


                      objectFound =false;


                }

		//show frames

		imshow(windowName2,threshold);

		imshow(windowName,cameraFeed);
		//imshow(windowName1,HSV);


		key=waitKey(30);


      }
	}
	return 0;
	CloseHandle(hDevice);
}

