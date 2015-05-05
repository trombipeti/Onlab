#include "imagematcher.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>

float ImageMatcher::getMinSecondTestRatio() const
{
    return minSecondTestRatio;
}

void ImageMatcher::setMinSecondTestRatio(float value)
{
    minSecondTestRatio = value;
}

float ImageMatcher::getMinFeatureDist() const
{
    return minFeatureDist;
}

void ImageMatcher::setMinFeatureDist(float value)
{
    minFeatureDist = value;
}

size_t ImageMatcher::getMinValidMatchSize() const
{
    return minValidMatchSize;
}

void ImageMatcher::setMinValidMatchSize(const size_t &value)
{
    minValidMatchSize = value;
}

size_t ImageMatcher::getMinGoodMatchSize() const
{
    return minGoodMatchSize;
}

void ImageMatcher::setMinGoodMatchSize(const size_t &value)
{
    minGoodMatchSize = value;
}
void ImageMatcher::detectKeypoints()
{
    cv::SiftFeatureDetector detector{};

    detector.detect(refImage.img, refImage.keyPoints);
    detector.detect(testImage.img, testImage.keyPoints);
}

void ImageMatcher::extractDescriptors()
{
    cv::SiftDescriptorExtractor extractor;

    extractor.compute(refImage.img, refImage.keyPoints, refImage.descriptor);
    extractor.compute(testImage.img, testImage.keyPoints, testImage.descriptor);
}

void ImageMatcher::matchFeatures(std::vector<std::vector<cv::DMatch> > &matches,
                                 std::vector<std::vector<cv::DMatch> > &matchesReverse)
{
    cv::FlannBasedMatcher matcher;

    matcher.knnMatch(refImage.descriptor, testImage.descriptor, matches, 2);
    matcher.knnMatch(testImage.descriptor, refImage.descriptor, matchesReverse, 1);
}

bool ImageMatcher::filterMatches(std::vector<std::vector<cv::DMatch> >& matches, std::vector<std::vector<cv::DMatch> >& matchesReverse)
{
    // Filter only matches that are much better than the others, and the are cross match between images
    for(auto knn : matches)
    {
        cv::DMatch& best = knn[0];
        cv::DMatch& second = knn[1];
        // Ha a legjobb nem "sokkal jobb", mint a masodik legjobb, akkor figyelmen kivul hagyjuk
        if(best.distance * minSecondTestRatio >= second.distance)
        {
            continue;
        }

        bool crossMatch = false;
        for(auto knnRev : matchesReverse)
        {
            if(knnRev[0].queryIdx == best.trainIdx &&
                    knnRev[0].trainIdx == best.queryIdx)
            {
                crossMatch = true;
                break;
            }
        }

        if(crossMatch)
        {
            good_matches.push_back(best);
        }

    }
    if(good_matches.size() < minGoodMatchSize)
    {
        std::cout << "Good matches size too small (" << good_matches.size() << " < " << minGoodMatchSize << ")" << std::endl;
        return false;
    }

    return true;
}

bool ImageMatcher::validateMatches()
{
    std::vector<cv::Point2f> ref;
    std::vector<cv::Point2f> test;

    for(size_t i = 0;i<good_matches.size();++i)
    {
        ref.push_back(refImage.keyPoints[good_matches[i].queryIdx].pt);
        test.push_back(testImage.keyPoints[good_matches[i].trainIdx].pt);
    }

    std::vector <uchar> inliers(good_matches.size(), 0);
    // 3D fundamental matrix, also calculates inliers
    cv::findFundamentalMat(ref, test, CV_FM_RANSAC, 2.5, 0.99, inliers);

    featureDist = 0;
    for(size_t i = 0; i < inliers.size(); ++i)
    {
        if(inliers[i])
        {
            valid_matches.push_back(good_matches[i]);
            featureDist += good_matches[i].distance;
        }
    }

    if(valid_matches.size() < minValidMatchSize)
    {
        std::cout << "Valid matches size too small (" << valid_matches.size() << " < " << minValidMatchSize << ")" << std::endl;
        featureDist = INFINITY;
        return false;
    }
    return true;
}

bool ImageMatcher::classify(cv::Mat& drawnMatches)
{
    if(refImage.img.empty())
    {
        throw "Reference image not set!";
    }
    if(testImage.img.empty())
    {
        throw "Test image not set!";
    }

    detectKeypoints();

    extractDescriptors();

    std::vector< std::vector< cv::DMatch> > knnMatches;
    std::vector< std::vector< cv::DMatch> > knnMatchesReverse;

    matchFeatures(knnMatches, knnMatchesReverse);

    if( !filterMatches(knnMatches, knnMatchesReverse))
    {
        std::cout << "Filter matches returned false" << std::endl;
        return false;
    }

//    if(display_matches)
//    {
//        cv::Mat goodImg;
//        cv::drawMatches(refImage.img, refImage.keyPoints, testImage.img, testImage.keyPoints, good_matches, goodImg,
//                        cv::Scalar::all(-1), cv::Scalar::all(-1),  std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
//        cv::namedWindow("Good matches", CV_WINDOW_NORMAL);
//        cv::imshow("Good matches", goodImg);
//    }

    if(!validateMatches())
    {
        std::cout << "Validate matches returned false" << std::endl;
        return false;
    }


    featureDist = featureDist / (valid_matches.size() * valid_matches.size() * valid_matches.size());

    if(featureDist < minFeatureDist)
    {
        std::vector<cv::Point2f> matched_points;

        for(size_t i = 0;i<valid_matches.size();++i)
        {
            matched_points.push_back(testImage.keyPoints[valid_matches[i].trainIdx].pt);
        }

        cv::Rect bbox = cv::boundingRect(cv::Mat(matched_points).reshape(2));
        cv::rectangle(testImage.img, bbox, cv::Scalar(0,200,10), 2);

        cv::drawMatches(refImage.img, refImage.keyPoints, testImage.img, testImage.keyPoints, valid_matches, drawnMatches );
        return true;
    }
    else
    {
        std::cout << "Feature distance too big: (" << featureDist << " > " << minFeatureDist << ")" << std::endl;
    }
    return false;
}
