#include "extract_utils.h"
#include <algorithm>
#include <cmath>

//// Compute L2 or Cosine distance
float computeDistance(const op::Array<float>& keypoints, const int person, const int elementA, const int elementB, bool _l2_distance)
{
    try
    {

        const int X = 0;
        const int Y = 1;

        const auto x = keypoints[{person, elementA, X}];
        const auto y = keypoints[{person, elementA, Y}];
        const auto a = keypoints[{person, elementB, X}];
        const auto b = keypoints[{person, elementB, Y}];

        if(!x || !y || !a || !b) return 0;

        if (_l2_distance) // Euclidean Distance
            return (std::sqrt((x-a)*(x-a) + (y-b)*(y-b)));
        else // Cosine Distance
            return 1 - ((a*x + b*y)/((std::sqrt(std::abs(a*a)+std::abs(b*b)))*(std::sqrt(std::abs(x*x)+std::abs(y*y)))));
    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        return -1.f;
    }
}

void flattenKeyPoints(const op::Array<float> &keypoints, const int &person, const std::vector<int> &onlyThesePoints,
                      std::vector<float> &flattened) {
    const auto bodyPartsSize = keypoints.getSize(1);
    for (auto bodyPart = 0; bodyPart < bodyPartsSize; bodyPart++) {
        bool shouldOutputParts = onlyThesePoints.empty() ||
                                 std::find(onlyThesePoints.begin(), onlyThesePoints.end(), bodyPart) != onlyThesePoints.end();

        //const auto confidence = keypoints[{person, bodyPart, 2}];
        // TODO: Discard low confidence?
        if(shouldOutputParts) {
            flattened.push_back(keypoints[{person, bodyPart, 0}]); // X Coord
            flattened.push_back(keypoints[{person, bodyPart, 1}]); // Y Coord
        }
    }
}

std::string keypointsAsNumpyTextString(const op::Array<float> &keypoints, const std::vector<int> &onlyThesePoints) {
    const int person = 0; // For now!
    std::vector<float> flattened;
    std::ostringstream stringStream;

    flattenKeyPoints(keypoints, person, onlyThesePoints, flattened);
    for (auto i = 0; i < flattened.size(); i++) stringStream << flattened[i] << " ";

    return stringStream.str();
}

void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
                  cv::Mat &output, cv::Point2i location)
{
    background.copyTo(output);


    // start at the row indicated by location, or at row 0 if location.y is negative.
    for(int y = std::max(location.y , 0); y < background.rows; ++y)
    {
        int fY = y - location.y; // because of the translation

        // we are done of we have processed all rows of the foreground image.
        if(fY >= foreground.rows)
            break;

        // start at the column indicated by location,

        // or at column 0 if location.x is negative.
        for(int x = std::max(location.x, 0); x < background.cols; ++x)
        {
            int fX = x - location.x; // because of the translation.

            // we are done with this row if the column is outside of the foreground image.
            if(fX >= foreground.cols)
                break;

            // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
            double opacity =
                    ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

                    / 255.;


            // and now combine the background and foreground pixel, using the opacity,

            // but only if opacity > 0.
            for(int c = 0; opacity > 0 && c < output.channels(); ++c)
            {
                unsigned char foregroundPx =
                        foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                unsigned char backgroundPx =
                        background.data[y * background.step + x * background.channels() + c];
                output.data[y*output.step + output.channels()*x + c] =
                        backgroundPx * (1.-opacity) + foregroundPx * opacity;
            }
        }
    }
}




