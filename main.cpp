// Main Code File

#include "cv.h"
#include"highgui.h"
#include"resize.h"
#include"serial.h"
#include<stdio.h>
#include<math.h>
#include<time.h>
CvPoint red_ball;
CvPoint blue_ball;
CvPoint bot1;
CvPoint bot2;
CvPoint rgc1;
CvPoint rgc2;
CvPoint rgc3;

CvCapture* capture;

IplImage* img;
IplImage* crop_img=NULL;
IplImage* enl_img=NULL;
IplImage* result1=NULL;
IplImage* canny=NULL;

IplImage *filter_red(const IplImage *origimg,int red_pixel,int green_pixel,int blue_pixel)
{
 IplImage* red_img=cvCreateImage(cvGetSize(origimg),8,1);
 for(int y=0;y<origimg->height;y++)
	{
	 uchar* ptr = (uchar*) (origimg->imageData + y * origimg->widthStep);
     for( int x=0; x<origimg->width; x++ )
      {
       if(ptr[3*x+2]>=red_pixel && ptr[3*x]<=blue_pixel && ptr[3*x+1]<=green_pixel)
        {
         ptr[3*x] = 255;
         ptr[3*x+1] = 255;
         ptr[3*x+2] = 255;
        }
       else
       {
        ptr[3*x]=0;
        ptr[3*x+1] = 0;
        ptr[3*x+2] = 0;
       } 
      }
     }
cvCvtColor(origimg,red_img,CV_BGR2GRAY);
cvThreshold( red_img,red_img, 10, 255, CV_THRESH_BINARY );
return red_img;
}

void find_red(IplImage* rimg)
{
   
  IplImage* result=filter_red(rimg,240,210,210);
	//cvShowImage("Crop",crop_img);
  cvShowImage("Red",result);
  //IplImage* rcanny=cvCreateImage(cvGetSize(enl_rimg),8,1);
  //cvCanny(result,rcanny,10,100,3);
  CvMemStorage* storage = cvCreateMemStorage(0);
	
	CvSeq* results = cvHoughCircles(
	  result,
	  storage,
	  CV_HOUGH_GRADIENT,
	  2,
	  result->width/25,
      2,40	  
	  );
  //if(results->total ==0)
  //{printf("No circles\n"); break;}
  if(results->total )
  {
    for( int i = 0; i < results->total; i++ ) 
    {
      float* p = (float*) cvGetSeqElem( results, i );
      if((p[0])>170 && (p[0]<270))
      {
      red_ball = cvPoint( cvRound( p[0] ), cvRound( p[1] ) );
      cvCircle(
       result,
       red_ball,
       cvRound( p[2] ),
       CV_RGB(0xff,0xff,0x00)
       );
      //printf(" %d  , %d  ,%d\n",pt.x,pt.y,cvRound(p[2]));
     }
    }
  }
}

void find_red_gate1(IplImage* rimg)
{   
  IplImage* rdst    = cvCreateImage( cvGetSize(rimg), 8, 1 );
	IplImage* rsrcg   = cvCreateImage( cvGetSize(rimg), 8, 1 );
  IplImage* color_rdst  = cvCreateImage( cvGetSize(rimg), 8, 3 );
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* lines = 0;
  
	int i=0;
	int points=0;
	
	rsrcg=filter_red(rimg,220,200,200);
  //cvCanny( rsrcg, rdst, 30, 100, 3 );
  //cvCvtColor( rdst, color_rdst, CV_GRAY2BGR );
  lines = cvHoughLines2( rsrcg, storage, CV_HOUGH_PROBABILISTIC, 0.1, 0.001,10, 10, 10 );
	printf("Number of lines:%d\n",lines->total);
		
	float gate_centre_x=0;
	float gate_centre_y=0;

	for( i = 0; i < lines->total; i++ )
  {
    CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
    cvLine( color_rdst, line[0], line[1], CV_RGB(255,255,0), 1, 8 );
    //printf("%d   %d     %d    %d\n",line[0].x,line[0].y,line[1].x,line[1].y);
    if(line[0].x >=100 && line[0].x<=140) //rsrcg->width/2 )
		{
			gate_centre_x= 	gate_centre_x +  (line[0].x + line[1].x)/2;
		  gate_centre_y= 	gate_centre_y +  (line[0].y + line[1].y)/2;
		  points++;
		}
	}

	rgc1.x=gate_centre_x/points;//(lines->total + 1);
	rgc1.y=gate_centre_y/points;//(lines->total + 1);
  printf("Center of gate:%d   %d",rgc1.x,rgc1.y);
  // cvShowImage("gate",rdst);

}   


void find_red_gate2(IplImage* rimg)
{   
	int count=0;
    
  IplImage* rdst  = cvCreateImage( cvGetSize(rimg), 8, 1 );
	IplImage* rsrcg = cvCreateImage( cvGetSize(rimg), 8, 1 );
  IplImage* color_rdst  = cvCreateImage( cvGetSize(rimg), 8, 3 );
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* lines = 0;
     
	int i=0;
	rsrcg=filter_red(rimg,240,210,210);
  cvCanny( rsrcg, rdst, 30, 100, 3 );
  cvCvtColor( rdst, color_rdst, CV_GRAY2BGR );
  lines = cvHoughLines2( rsrcg, storage, CV_HOUGH_PROBABILISTIC, 0.1, 0.001,10, 10, 10 );
	printf("Number of lines:%d\n",lines->total);
		
	float gate_centre_x=0;
	float gate_centre_y=0;

	for( i = 0; i < lines->total; i++ )
  {
    CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
    cvLine( color_rdst, line[0], line[1], CV_RGB(255,255,0), 1, 8 );
    //  printf("%d   %d     %d    %d\n",line[0].x,line[0].y,line[1].x,line[1].y);
    if(line[0].x >=340 && line[0].x<=370) //rsrcg->width/2 )
		{ 
			 gate_centre_x= 	gate_centre_x +  (line[0].x + line[1].x)/2;
			 gate_centre_y= 	gate_centre_y +  (line[0].y + line[1].y)/2;
			 count++;
		}
	}

	rgc2.x=gate_centre_x/count;//(lines->total + 1);
	rgc2.y=gate_centre_y/count;//(lines->total + 1);
        
	printf("Center of gate2:%d   %d",rgc2.x,rgc2.y);
  cvShowImage("gate",rdst);

}

void find_red_gate3(IplImage* rimg)
{   
	int count=0;
    
  IplImage* rdst    = cvCreateImage( cvGetSize(rimg), 8, 1 );
	IplImage* rsrcg   = cvCreateImage( cvGetSize(rimg), 8, 1 );
  IplImage* color_rdst  = cvCreateImage( cvGetSize(rimg), 8, 3 );
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* lines = 0;
     
	int i=0;
		
	rsrcg=filter_red(rimg,240,200,200);
  cvCanny( rsrcg, rdst, 30, 100, 3 );
  cvCvtColor( rdst, color_rdst, CV_GRAY2BGR );
  lines = cvHoughLines2( rsrcg, storage, CV_HOUGH_PROBABILISTIC, 0.1, 0.001,10, 10, 10 );
	printf("Number of lines:%d\n",lines->total);
		
	float gate_centre_x=0;
	float gate_centre_y=0;

	for( i = 0; i < lines->total; i++ )
  {
    CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
    cvLine( color_rdst, line[0], line[1], CV_RGB(255,255,0), 1, 8 );
    //  printf("%d   %d     %d    %d\n",line[0].x,line[0].y,line[1].x,line[1].y);
    if(line[0].x >=370  && line[0].x<=470) //rsrcg->width/2 )
		{ 
		  gate_centre_x= 	gate_centre_x +  (line[0].x + line[1].x)/2;
		  gate_centre_y= 	gate_centre_y +  (line[0].y + line[1].y)/2;
		  count++;
		}
	}

	rgc2.x=gate_centre_x/count;//(lines->total + 1);
	rgc2.y=gate_centre_y/count;//(lines->total + 1);
        
	printf("Center of gate2:%d   %d",rgc2.x,rgc2.y);
  cvShowImage("gate",rdst);

}


void main()
{
	IplImage* img = cvLoadImage("d:\\i17.jpg");
	IplImage* test = cvCreateImage(cvGetSize(img),8,3);
	cvCopy(img,test,0);
	CvRect region1= cvRect(75,45,475,360);
	IplImage* crop_img=cropImage(test,region1);
	cvSaveImage("d:\\i18.jpg",crop_img);
	cvShowImage("img",crop_img);
  IplImage* test1 = cvCreateImage(cvGetSize(crop_img),8,3);
  IplImage* test2 = cvCreateImage(cvGetSize(crop_img),8,3);
  cvCopy(crop_img,test1,0);
  cvCopy(crop_img,test2,0);
  IplImage* test3 = cvCreateImage(cvGetSize(crop_img),8,3);
  cvCopy(crop_img,test3,0);
  IplImage* test4 = cvCreateImage(cvGetSize(crop_img),8,3);
  cvCopy(crop_img,test4,0);


  find_red_gate1(test1);
	find_red(test2);
  find_red_gate2(test3);
  find_red_gate3(test4);
	cvSaveImage("d:\\i18.jpg",crop_img);
	cvShowImage("img",crop_img);
  IplImage* test1 = cvCreateImage(cvGetSize(crop_img),8,3);
  IplImage* test2 = cvCreateImage(cvGetSize(crop_img),8,3);
  cvCopy(crop_img,test1,0);
  cvCopy(crop_img,test2,0);
  IplImage* test3 = cvCreateImage(cvGetSize(crop_img),8,3);
  cvCopy(crop_img,test3,0);
  IplImage* test4 = cvCreateImage(cvGetSize(crop_img),8,3);
  cvCopy(crop_img,test4,0);


  find_red_gate1(test1);
	find_red(test2);
  find_red_gate2(test3);
  find_red_gate3(test4);
	cvSaveImage("d:\\i18.jpg",crop_img);
	cvShowImage("img",crop_img);
	printf("Gate1: %d  %d",rgc1.x,rgc1.y);
	printf("\nRed_ball: %d   %d",red_ball.x,red_ball.y);
	cvWaitKey(0);
}
