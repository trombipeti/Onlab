#include "imagematcher.h"

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <vector>
#include <algorithm>

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
    return maxFeatureDist;
}

void ImageMatcher::setMaxFeatureDist(float value)
{
    maxFeatureDist = value;
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
    // Filter only matches that are much better than the others, and that are cross match between images
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
        std::stringstream sstr;
        sstr << "Good matches size too small (" << good_matches.size() << " < " << minGoodMatchSize << ")";
        failCause = sstr.str();
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
        std::stringstream sstr;
        sstr << "Valid matches size too small (" << valid_matches.size() << " < " << minValidMatchSize << ")";
        failCause = sstr.str();
        featureDist = INFINITY;
        return false;
    }
    return true;
}

std::vector<cv::Point2f> ImageMatcher::filterOutlierMatches(const std::vector<cv::Point2f>& matchesToFilter)
{
    std::vector<double> xcoords;
    std::vector<float> ycoords;
    for(cv::Point2f p : matchesToFilter)
    {
        xcoords.push_back(p.x);
        ycoords.push_back(p.y);
    }

    double xmean = std::accumulate(xcoords.begin(), xcoords.end(), 0) / xcoords.size();
    double ymean = std::accumulate(ycoords.begin(), ycoords.end(), 0) / ycoords.size();

    double xacc = 0;
    std::for_each(xcoords.begin(), xcoords.end(), [&](const double f) {
       xacc += (f-xmean) * (f-xmean);
    });


    double yacc = 0;
    std::for_each(ycoords.begin(), ycoords.end(), [&](const double f) {
       yacc += (f-ymean) * (f-ymean);
    });

    double xsd = sqrt(xacc / xcoords.size());
    double ysd = sqrt(yacc / ycoords.size());

    std::vector<cv::Point2f> filtered;
    for(auto& pt : matchesToFilter)
    {
        if(
            (sqrt( (pt.x - xmean) * (pt.x - xmean)) < 3*xsd) &&
            (sqrt( (pt.y - ymean) * (pt.y - ymean)) < 3*ysd)
            )
        {
            filtered.push_back(pt);
        }
    }
    return filtered;



}

bool ImageMatcher::classify(cv::Mat& drawnMatches)
{
    featureDist = INFINITY;
    good_matches.clear();
    valid_matches.clear();
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
//        std::cout << "Filter matches returned false" << std::endl;
//        failCause = "Filter matches returned false";
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
//        std::cout << "Validate matches returned false" << std::endl;
//        failCause = "Validate matches returned false";
        return false;
    }


    featureDist = featureDist / (valid_matches.size() * valid_matches.size() * valid_matches.size());

    if(featureDist < maxFeatureDist)
    {
        std::vector<cv::Point2f> matched_points;

        for(size_t i = 0;i<valid_matches.size();++i)
        {
            matched_points.push_back(testImage.keyPoints[valid_matches[i].trainIdx].pt);
        }

        matched_points = filterOutlierMatches(matched_points);

        cv::Rect bbox = cv::boundingRect(cv::Mat(matched_points).reshape(2));
        cv::rectangle(testImage.img, bbox, cv::Scalar(50,0,160), 3);

        cv::drawMatches(refImage.img, refImage.keyPoints, testImage.img, testImage.keyPoints, valid_matches, drawnMatches,
                        cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
                        cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS | cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        return true;
    }
    else
    {
        std::stringstream sstr;
        sstr << "Feature distance too big: (" << featureDist << " > " << maxFeatureDist << ")";
        failCause = sstr.str();
    }
    return false;
}
