//! [includes]
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#include <windows.h>
#include <mmsystem.h>
#include <cstdint>

#include <dshow.h>



#pragma comment (lib, "winmm.lib")


using namespace cv;
using namespace std;
using namespace std::chrono;

//#define FRAME_STEP_X 3 //3
//#define FRAME_STEP_Y 5 //5
//#define SYMBOLS_COUNT 70

#define MAX_VALUE 770

#define NORMALIZER (MAX_VALUE / SYMBOLS_COUNT)

int SYMBOLS_COUNT = 70;


//{ ' ', '.', '\'', '`', '^', '\"', ',', ':', ';', 'I', 'l', '!', 'i', '>', '<', '~', '+', '_', '-', '?', ']', '[', '}', '{', '1', ')', '(', '|', '\\', '/ ', 't', 'f', 'j', 'r', 'r', 'n', 'u', 'v', 'c', 'z', 'X', 'Y', 'U', 'J', 'C', 'L', 'Q', '0', 'O', 'Z', 'm', 'w', 'q', 'p', 'd', 'b', 'k', 'h', 'a', 'o', '*', '#', 'M', 'W', ' & ', '8', ' % ', 'B', '@', '$' };
char symbols2[] = {' ', '.', ':', '-', '=', '+', '*', '#', '%', '@'};
char symbols [] = { ' ', '.', '\'', '`', '^', '\"', ',', ':', ';', 'I', 'l', '!', 'i', '>', '<', '~', '+', '_', '-', '?', ']', '[', '}', '{', '1', ')', '(', '|', '\\', '/ ', 't', 'f', 'j', 'r', 'r', 'n', 'u', 'v', 'c', 'z', 'X', 'Y', 'U', 'J', 'C', 'L', 'Q', '0', 'O', 'Z', 'm', 'w', 'q','p','d','b','k','h','a','o', '*','#','M','W',' & ','8',' % ','B','@','$' };
//const char symbols[] = { ' ', '.', ',', '/', 'O' };
//const char symbols[] = { ' ', '░', '▒', '▓', '█' };

//#pragma execution_character_set( "utf-8" )


class Timer{
public:
    void start() {
        m_StartTime = std::chrono::system_clock::now();
        m_bRunning = true;
    }

    void stop() {
        m_EndTime = std::chrono::system_clock::now();
        m_bRunning = false;
    }

    double elapsedMilliseconds() {
        std::chrono::time_point<std::chrono::system_clock> endTime;

        if (m_bRunning) {
            endTime = std::chrono::system_clock::now();
        }
        else {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    double elapsedSeconds() {
        return elapsedMilliseconds() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool                                               m_bRunning = false;
};

wchar_t* ret;
double timePerFrame;
IGraphBuilder* pGraph = NULL;

int FRAME_STEP_X = 3;
int FRAME_STEP_Y = 5;

bool doImage = false;

void frameTo(int rows, int cols, Mat *frame) {
    for (int i = 0; i < rows; i += 1) {
        for (int j = 0; j < cols; j += 1) {
            Vec3b bgrPixel = frame->at<Vec3b>(i * FRAME_STEP_Y, j * FRAME_STEP_X);
            ret[i * cols + i + j] = symbols[(bgrPixel.val[0] + bgrPixel.val[1] + bgrPixel.val[2]) / NORMALIZER];
        }
    }
}

int getSizeX() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return columns;
}
int getSizeY() {

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return rows;
}

int main(int argc, char* argv[]){
    std::string videoPath = "E:\\Samples\\LagTrain.mp4";
    if (argc == 1) {
        //string str(argv);
        cout << argv[0] << endl;
        cout << "no video path?";
        getch();
        return 0;
    }
    else if (argc >= 2) {
        videoPath = argv[1];
        if (videoPath[videoPath.length() - 1] != '4') {
            doImage = true;
            
        }
    }
    VideoCapture cap(videoPath);
    double fps = cap.get(cv::CAP_PROP_FPS);
    timePerFrame = ((double)1000.0) / fps;
    if (!cap.isOpened()) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

    //int i = 0;
    Mat frame;
    cap >> frame;

    //CMDMP4.exe E:\Samples\LagTrain.mp4
    system("CLS");
    FRAME_STEP_Y = frame.rows / getSizeY() + 1;
    FRAME_STEP_X = frame.cols / getSizeX() + 1;

    //
    if (argc >= 3) {
        SYMBOLS_COUNT = 10;
        for (int i = 0; i < SYMBOLS_COUNT; i++) {
            symbols[i] = symbols2[i];
        }
        
    }
    if (argc >= 5) {
        FRAME_STEP_X = atoi(argv[2]);
        FRAME_STEP_Y = atoi(argv[3]);
    
    }
    
    


    int rows = frame.rows / FRAME_STEP_Y;
    int cols = frame.cols / FRAME_STEP_X;

    ret = new wchar_t[rows * cols + rows];
    for (int i = 1; i < rows; i++) {
        ret[i * cols + i-1] = '\n';
    }
    ret[cols*rows + rows -1] = 0;

    system("CLS");
    int f = -1;
    HANDLE const output{ GetStdHandle(STD_OUTPUT_HANDLE) };
    COORD cPos;
    cPos.X = 0;
    cPos.Y = 0;


    if (doImage) {
        Mat img = imread(videoPath, IMREAD_COLOR);
        frameTo(rows, cols, &frame);
        SetConsoleCursorPosition(output, cPos);
        WriteConsoleW(output, ret, cols * rows + rows, {}, {});
        getch();
        return 0;
    }

    Timer timer;
    timer.start();

    while (1) {
        f++;
        cap >> frame;
        
        frameTo(rows, cols, &frame);
        /*
        for (int i = 0; i < rows; i += 1) {
            for (int j = 0; j < cols; j += 1){
                Vec3b bgrPixel = frame.at<Vec3b>(i * FRAME_STEP_Y, j * FRAME_STEP_X);
                ret[i * cols + i  + j] = symbols[(bgrPixel.val[0] + bgrPixel.val[1] + bgrPixel.val[2]) / NORMALIZER];
            }
        }
        */
        while (timer.elapsedMilliseconds() < f * timePerFrame) {
            _sleep(1);
        }
        SetConsoleCursorPosition(output, cPos);
        WriteConsoleW(output, ret, cols * rows + rows, {}, {});
        //cout << '\n' << f << '\n';
        //cout << '\n' << fps << '\n';
        //cout << '\n' << FRAME_STEP_X << '\n';
        //cout << '\n' << FRAME_STEP_Y << '\n';
        if (frame.empty())
            break;
        //char c = (char)waitKey(25);
        //if (c == 'w' || c == 'W' || c == 'ц' || c == 'Ц')
        //    break;
    }
    timer.stop();
    // When everything done, release the video capture object
    cap.release();

    // Closes all the frames
    destroyAllWindows();
    

    return 0;
}