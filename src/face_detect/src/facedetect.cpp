#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> //用于防御式编程
#include <math.h>
#include <float.h>//<float.h>与<limits.h>一样是定义边界值的,<float.h>定义的是浮点数的边界值
#include <limits.h>
#include <time.h>
#include <ctype.h>//在调用字符函数时，在源文件中包含的头文件

static CvMemStorage *storage = 0;
static CvHaarClassifierCascade *cascade = 0;
void detect_and_draw( IplImage *image);
const char *cascade_name = "haarcascade_frontalface_alt.xml";
int main1( int argc, char **argv)
{
    CvCapture *capture = 0;
    IplImage *frame, *frame_copy = 0;
    int optlen = strlen("--cascade=");
    const char *input_name;
    if (argc > 1 && strncmp ( argv[1], "--cascade=", optlen ) == 0 ) //如果有图像或者视频输入执行
    {
        cascade_name = argv[1] + optlen;
        input_name =  argc > 2 ? argv[2] : 0;
    }
    else   //实时视频
    {
        cascade_name = "haarcascade_frontalface_alt.xml";
        input_name = argc > 1 ? argv[1] : "test.png";
    }
    cascade_name = "haarcascade_frontalface_alt2.xml";
    IplImage * img0;
    CvSeq* faces = cvHaarDetectObjects( img0, cascade, storage,
                                                1.1, 2, 0,
                                                cvSize(30, 30) );
    cvReleaseHaarClassifierCascade(&cascade);
    cascade = (CvHaarClassifierCascade*)cvLoad( "haarcascade_frontalface_alt2.xml", 0, 0, 0);
    if ( !cascade )
    {
        fprintf( stderr, "ERROR:could not load classifier cascade\n");
        fprintf( stderr, "Usage:facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n");
        return -1;
    }
    storage = cvCreateMemStorage(0);
    if ( !input_name || (isdigit(input_name[0]) && input_name[1] == '\0'))  //extern int isdigit(char c)判断字符c是否为数字
        capture = cvCaptureFromCAM( !input_name ? 0 : input_name[0] - '0');
    else
        capture = cvCaptureFromAVI( input_name );   //如果有视频文件，就读视频文件
    cvNamedWindow( "result", 1);
    if (capture)
    {
        for(;;)
        {
            if (!cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture ); //获得由cvGrabFrame函数抓取的图片
            if (!frame)
                break;
            if (!frame_copy)
                frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
                IPL_DEPTH_8U,frame->nChannels);
            if (frame->origin == IPL_ORIGIN_TL)

                cvCopy (frame, frame_copy, 0);
            else
                cvFlip (frame, frame_copy, 0);////反转图像,沿着x-axis
            detect_and_draw( frame_copy ); // 检测并且画出人脸
            if(cvWaitKey (10) >= 0)
                break;
        }

        //释放指针
        cvReleaseImage( &frame_copy );
        cvReleaseCapture( &capture);
    }
    else  //如果不是视频，那就是图像呗！
    {

        const char *filename = input_name ? input_name : (char*)"test.png";
        IplImage *image = cvLoadImage(filename, 1);
        if (image)
        {
            detect_and_draw(image);
            cvWaitKey(0);
            cvReleaseImage(&image);
        }
        else
        {


            FILE *f = fopen(filename, "rt");
            if (f)
            {
                char buf[1000+1];
                while (fgets(buf, 1000 , f))
                {
                    int len = (int)strlen(buf);

                    while ( len > 0 && isspace(buf[len-1]))
                        len--;
                    buf[len] = '\0';
                    image = cvLoadImage(buf, 1);
                    if (image)
                    {
                        detect_and_draw(image);
                        cvWaitKey(0);
                        cvReleaseImage(&image);
                    }
                }
                fclose(f);
            }
        }
    }

    cvDestroyWindow("result");
    return 0;
}

void detect_and_draw(IplImage *img) //检测和画出人脸的函数体
{

    static CvScalar colors[] =
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}}
    };

    double scale = 1.3;
    IplImage *gray = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);
    IplImage *small_img = cvCreateImage(cvSize(cvRound(img->width/scale),
        cvRound(img->height/scale)),
        8, 1);

    int i;
    cvCvtColor(img, gray, CV_BGR2GRAY);//把输入的彩色图像转化为灰度图像

    cvResize(gray, small_img,CV_INTER_LINEAR);
    cvEqualizeHist(small_img, small_img);//灰度图象直方图均衡化
    cvClearMemStorage(storage);
    if (cascade)
    {
        double t = (double)cvGetTickCount();
        CvSeq *faces = cvHaarDetectObjects(small_img, cascade,storage,1.1,2,0,cvSize(30, 30));
        t = (double)cvGetTickCount() - t; //计算的时间
        printf("detection time = %gms\n",t/((double)cvGetTickFrequency()*1000.));
        for (i = 0; i < (faces ? faces->total : 0); i++)
        {
            //返回索引所指定的元素指针
            CvRect *r = (CvRect*)cvGetSeqElem(faces, i);

            //用矩形
            //确定两个点来确定人脸位置，因为用cvRetangle嘛
            CvPoint pt1, pt2;
            //找到画矩形的两个点
            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;
            //画出矩形
            cvRectangle( img, pt1, pt2, colors[i%8], 3, 8, 0 );
        }
    }
    cvShowImage("result",img);
    cvReleaseImage(&gray);
    cvReleaseImage(&small_img);
}
