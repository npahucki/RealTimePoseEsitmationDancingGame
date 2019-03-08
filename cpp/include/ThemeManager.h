//
// Created by Nathan  Pahucki on 10/29/18.
//

#ifndef GESTURES_THEMEMANAGER_H
#define GESTURES_THEMEMANAGER_H


// TODO: For now these are hardcoded, we will want to read the directory on the disk to load this stuff automatically.

#include <string>
#include <map>
#include <memory>
#include <opencv/cv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

class ThemeManager {

private:

    class ThemeInfo {
    public:
        ThemeInfo(int numHeads, int numBackgrounds) : numHeads(numHeads), numBackgrounds(numBackgrounds) {}
        int numHeads = 0;
        int numBackgrounds = 0;
    };

    int currentBackgroundIdx = 0;
    int currentHeadIdx = 0;
    std::map<std::string,std::unique_ptr<ThemeInfo>> themes_;
    cv::Mat nullHead_;
    std::vector<cv::Mat> cachedHeads_;
    std::vector<cv::Mat> cachedBackgrounds_;
    std::string themeName_ = "";

    const ThemeInfo* getThemeInfo();
    void loadHeads();
    void loadBackgrounds(const cv::Size &outputSize);

public:
    const std::string &getThemeName() const;

public:
    ThemeManager(const std::string &themeName, const cv::Size &outputSize);


public:
    int getNumberOfAvailableHeads();
    int getNextHeadNumber();

    /**
     * @param number less than 0 to use the null image, or 0 to less than getNumberOfAvailableHeads()
     * @return The image for the head at the index provided.
     */
    cv::Mat & getHeadImage(const int & number);

    int getNumberOfAvailableBackgrounds();
    int getNextBackgroundNumber();
    cv::Mat & getBackgroundImage(const int & number);



};


#endif //GESTURES_THEMEMANAGER_H
