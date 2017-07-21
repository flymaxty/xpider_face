#include <iostream>

#include <pthread.h>
#include <signal.h>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

#define FACE_DETECT_PERIOD 5
#define FACE_UI_WIDTH_OFFSET 1300
#define FACE_UI_HEIGHT_OFFSET 680

bool g_running = false;

// pthread_t g_grab_thread;
// pthread_mutex_t g_mutex_lock;



cv::VideoCapture camera;

void clear(int signo) {
  std::cout << "Get exit signal" << std::endl;
  g_running = false;
}

// void* GrabFunc(void* in_data) {
//   while(g_running) {
//     pthread_mutex_lock(&g_mutex_lock);
//     if(camera.isOpened()) {
//       camera.grab();
//     }
//     pthread_mutex_unlock(&g_mutex_lock);
//   }

//   std::cout << "Grab thread exit." << std::endl;
// }

int main() {
  cv::Mat raw_image, scaled_image, gray_image;

  std::string name = "Jiqiang Song";
  cv::Scalar color(0, 255, 0);

  int key=255;
  cv::Mat ui_face_image[6], face_on_rect;
  cv::Rect ui_face_rect1(FACE_UI_WIDTH_OFFSET, FACE_UI_HEIGHT_OFFSET, 195, 195),
           ui_face_rect2(FACE_UI_WIDTH_OFFSET+195, FACE_UI_HEIGHT_OFFSET, 195, 195),
           ui_face_rect3(FACE_UI_WIDTH_OFFSET+390, FACE_UI_HEIGHT_OFFSET, 195, 195),
           ui_face_rect4(FACE_UI_WIDTH_OFFSET, FACE_UI_HEIGHT_OFFSET+195, 195, 195),
           ui_face_rect5(FACE_UI_WIDTH_OFFSET+195, FACE_UI_HEIGHT_OFFSET+195, 195, 195),
           ui_face_rect6(FACE_UI_WIDTH_OFFSET+390, FACE_UI_HEIGHT_OFFSET+195, 195, 195);
  std::vector<cv::Rect> faces;

  for(int i=0; i<6; i++) {
    ui_face_image[i] = cv::imread("face.png");
    if(ui_face_image[i].empty()) {
      std::cout << "no face" << std::endl;
    }
  }

  cv::CascadeClassifier face_cascade;
  std::string face_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml";
  if(!face_cascade.load(face_cascade_name)) {
    std::cout << "can not find face_cascade_file!" << std::endl;
    exit(-1);
  }

  /* face train */
  // cv::Ptr<cv::face::LBPHFaceRecognizer> face_model;
  // face_model = cv::face::createLBPHFaceRecognizer();
  // face_model->load("face_result.xml");
  // std::vector<cv::Mat> face_train_list(6);
  // std::vector<int> name_index(6);

  cv::namedWindow("XpiderFace", CV_WINDOW_FREERATIO | CV_WINDOW_NORMAL);

  g_running = true;

  // pthread_mutex_init(&g_mutex_lock, NULL);
  // pthread_create(&g_grab_thread, NULL, GrabFunc, NULL);

  // signal(SIGINT, clear);
  // signal(SIGTERM, clear);

  camera.open("rtsp://admin:admin@192.168.1.83:554/cam1/h264");
  // camera.open("rtsp://admin:admin@192.168.100.1:554/cam1/h264");
  // camera.open(0);

  int frame_count = FACE_DETECT_PERIOD;
  bool has_face = false;

  cv::Rect face_rect(0, 0, 0, 0);

  while(g_running) {
    camera >> raw_image;
    // camera.retrieve(raw_image);

    if(frame_count > 0) {
      frame_count -= FACE_DETECT_PERIOD;
    } else {
      frame_count = FACE_DETECT_PERIOD;
      cv::resize(raw_image, scaled_image, cv::Size(320, 180), 0, 0, CV_INTER_LINEAR);

      cv::cvtColor(scaled_image, gray_image, cv::COLOR_BGR2GRAY);
      cv::equalizeHist(gray_image, gray_image);

      faces.clear();
      face_cascade.detectMultiScale(gray_image, faces, 1.1,
          2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

      if(!faces.empty()) {
        has_face = true;
        face_rect = cv::Rect(faces[0].x*4, faces[0].y*4, faces[0].width*4, faces[0].height*4);
          //cv::Rect rect();
      } else {
        has_face = false;
        // std::cout << "No face" << std::endl;
      }
    }

    if(has_face) {
      cv::putText(raw_image,
                  name,
                  cv::Point(face_rect.x, face_rect.y-20),
                  cv::FONT_HERSHEY_PLAIN,
                  3,
                  color,
                  3,
                  cv::LineTypes::LINE_AA);
      cv::rectangle(raw_image, face_rect, color, 3, 4, 0);
      if(key > 48 && key < 55) {
        cv::resize(raw_image(face_rect), face_on_rect, cv::Size(195, 195), 0, 0, CV_INTER_LINEAR);
        face_on_rect.copyTo(ui_face_image[key-49]);
      }
    }

      // cv::rectangle(raw_image, face_rect, cv::Scalar(0, 255, 0), 3, 4, 0);

    cv::resize(raw_image, raw_image, cv::Size(1920, 1080), 0, 0, CV_INTER_LINEAR);
    ui_face_image[0].copyTo(raw_image(ui_face_rect1));
    ui_face_image[1].copyTo(raw_image(ui_face_rect2));
    ui_face_image[2].copyTo(raw_image(ui_face_rect3));
    ui_face_image[3].copyTo(raw_image(ui_face_rect4));
    ui_face_image[4].copyTo(raw_image(ui_face_rect5));
    ui_face_image[5].copyTo(raw_image(ui_face_rect6));
    cv::imshow("XpiderFace", raw_image);

    key = cv::waitKey(25);
    std::cout << "key: " << key << std::endl;

    if(key == 113) {
      name = "Jiqiang Song";
      color = cv::Scalar(0, 255, 0);
    }
    if(key == 119) {
      name = "?_?";
      color = cv::Scalar(0, 0, 255);
    }

    // cv::imshow("XpiderFace", raw_image);
    // cv::waitKey(1);

    // gettimeofday(&stopTime, NULL);
    // timeUse = stopTime.tv_sec - startTime.tv_sec + (stopTime.tv_usec - startTime.tv_usec)/1000000.0;
    // if(timeUse < 0.25)
    //     usleep((0.25 - timeUse) * 1000000);

    // outStream << "Time use: " << timeUse << "s, " << logStream.str();
    // std::cout << outStream.str() << std::endl;
  }
  return 0;
}