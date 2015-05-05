#include "mainwindow.h"
#include <QApplication>

#include <iostream>
#include "videoptd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

//    const char* filename = "/home/trombipeti/Videók/Webcam/onlab/2015-05-01-230730.webm";

//    if(argc < 2)
//    {
//        std::cout << "Need at least one argument!" << std::endl;
//        std::cin >> filename;
//    }
//    else
//    {
//        filename = argv[1];
//    }
//    VideoPTD vptd(filename);
//    vptd.start();
//    vptd.videoProcessThread();

}
