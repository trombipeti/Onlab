#include "imagematcher.h"

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>

size_t ImageMatcher::minGoodMatchSize = 15;
size_t ImageMatcher::minValidMatchSize = 10;
float ImageMatcher::minSecondTestRatio = 0.95;
float ImageMatcher::minFeatureDist = 0.25;

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
//        if(second.distance * minSecondTestRatio >= best.distance)
//        {
//            continue;
//        }

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
        featureDist = INFINITY;
        return false;
    }
    return true;
}

bool ImageMatcher::classify()
{

    detectKeypoints();

    extractDescriptors();

    std::vector< std::vector< cv::DMatch> > knnMatches;
    std::vector< std::vector< cv::DMatch> > knnMatchesReverse;

    matchFeatures(knnMatches, knnMatchesReverse);

    if( !filterMatches(knnMatches, knnMatchesReverse))
    {
        return false;
    }

    if(!validateMatches())
    {
        return true;
    }

    featureDist = featureDist / valid_matches.size();

    if(featureDist < minFeatureDist)
    {
        return true;
    }
    return false;
}
