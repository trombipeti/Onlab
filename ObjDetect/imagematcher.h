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

#include <string>

class ImageMatcher
{
private:


    float featureDist = INFINITY;
    size_t minGoodMatchSize = 15;
    size_t minValidMatchSize = 10;
    float maxFeatureDist = 0.25f;
    float minSecondTestRatio = 0.92f;

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

    std::string failCause = "";

    void detectKeypoints();
    void extractDescriptors();
    void matchFeatures(std::vector<std::vector<cv::DMatch> >& matches, std::vector<std::vector<cv::DMatch> >& matchesReverse);

    bool filterMatches(std::vector<std::vector<cv::DMatch> > &matches, std::vector<std::vector<cv::DMatch> > &matchesReverse );
    bool validateMatches();

    std::vector<cv::Point2f> filterOutlierMatches(const std::vector<cv::Point2f> &matchesToFilter);
public:
    ImageMatcher(cv::Mat const& _refImg = cv::Mat(), cv::Mat const& _testImg = cv::Mat()) :
        refImage{_refImg}, testImage{_testImg}
    {}

    ImageMatcher(std::string _refFile, std::string _testFile) :
        refImage{_refFile}, testImage{_testFile}
    {}

    void setRefImg(cv::Mat img)
    {
        refImage = ImageData{img};
    }

    void setTestImg(cv::Mat img)
    {
        testImage = ImageData{img};
    }

    std::string getFailCause()
    {
        return failCause;
    }

    bool classify(cv::Mat &drawnMatches);


    float getMinSecondTestRatio() const;
    void setMinSecondTestRatio(float value);
    float getMinFeatureDist() const;
    void setMaxFeatureDist(float value);
    size_t getMinValidMatchSize() const;
    void setMinValidMatchSize(const size_t &value);
    size_t getMinGoodMatchSize() const;
    void setMinGoodMatchSize(const size_t &value);
};

#endif // IMAGEMATCHER_H
