#ifndef IMAGEMATCHER_H
#define IMAGEMATCHER_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>
#include <cmath>


class ImageMatcher
{
private:

    float featureDist = INFINITY;
    static size_t minGoodMatchSize;
    static size_t minValidMatchSize;
    static float minFeatureDist;
    static float minSecondTestRatio;

    struct ImageData
    {
        cv::Mat img;
        cv::Mat descriptor;

        std::vector<cv::KeyPoint> keyPoints;

        std::string fileName;
        std::string saveFileName;

        explicit ImageData(cv::Mat const& _img, std::string _fileToSave = "")
        {
            img = _img.clone();
            saveFileName = _fileToSave;
        }

        explicit ImageData(std::string _readFrom, std::string _saveTo = "")
        {
            img = cv::imread(_readFrom);
            saveFileName = _saveTo;
        }
    };

    ImageData refImage;
    ImageData testImage;

    std::vector< cv::DMatch > good_matches;
    std::vector< cv::DMatch > valid_matches;

    void detectKeypoints();
    void extractDescriptors();
    void matchFeatures(std::vector<std::vector<cv::DMatch> >& matches, std::vector<std::vector<cv::DMatch> >& matchesReverse);

    bool filterMatches(std::vector<std::vector<cv::DMatch> > &matches, std::vector<std::vector<cv::DMatch> > &matchesReverse );
    bool validateMatches();
    void sumFeatureDist();

public:
    ImageMatcher(cv::Mat const& _refImg, cv::Mat const& _testImg) :
        refImage{_refImg}, testImage{_testImg}
    {}

    ImageMatcher(std::string _refFile, std::string _testFile) :
        refImage{_refFile}, testImage{_testFile}
    {}

    bool classify();
};

#endif // IMAGEMATCHER_H
