//
// Created by Nathan  Pahucki on 10/29/18.
//

#include <ThemeManager.h>


#include "ThemeManager.h"

ThemeManager::ThemeManager(const std::string &themeName, const cv::Size &outputSize) {
    themes_["space"] = std::make_unique<ThemeManager::ThemeInfo>(1, 1);
    themes_["bean"] = std::make_unique<ThemeManager::ThemeInfo>(1, 1);
    themes_["halloween"] = std::make_unique<ThemeManager::ThemeInfo>(5, 9);

    if(themes_.find(themeName) == themes_.end()) {
        throw std::runtime_error("Invalid theme name");
    }
    themeName_ = themeName;
    loadBackgrounds(outputSize);
    loadHeads();
}

const std::string &ThemeManager::getThemeName() const {
    return themeName_;
}

int ThemeManager::getNumberOfAvailableHeads() {
    return static_cast<int>(cachedHeads_.size());
}

int ThemeManager::getNextHeadNumber() {
    auto retVal = currentHeadIdx;
    if(currentHeadIdx++ >= getNumberOfAvailableHeads()) {
        currentHeadIdx = 0;
        retVal = 0;
    }
    return retVal;
}

cv::Mat & ThemeManager::getHeadImage(const int &number) {
    assert(number == -1 || number < cachedHeads_.size() && "Requested a head that does not exist");
    return number < 0 || number >= cachedHeads_.size() ? nullHead_ : cachedHeads_[number];
}

int ThemeManager::getNumberOfAvailableBackgrounds() {
    return static_cast<int>(cachedBackgrounds_.size());
}

int ThemeManager::getNextBackgroundNumber() {
    auto retVal = currentBackgroundIdx;

    if(currentBackgroundIdx++ >= getNumberOfAvailableBackgrounds()) {
        currentBackgroundIdx = 0;
        retVal = 0;
    }

    return retVal;
}

cv::Mat & ThemeManager::getBackgroundImage(const int &number) {
    assert(number < cachedBackgrounds_.size() && "Requested a head that does not exist");
    return cachedBackgrounds_[number];
}

const ThemeManager::ThemeInfo *ThemeManager::getThemeInfo() {
    return themes_[themeName_].get();
}

void ThemeManager::loadHeads() {
    const ThemeInfo* info = getThemeInfo();
    for(int i=1; i <= info->numHeads; i++) {
        const auto fileName = "./cpp/assets/" + themeName_ + "/heads/" + std::to_string(i) + ".png";
        const auto image = cv::imread(fileName, CV_LOAD_IMAGE_UNCHANGED);
        if (!image.data) throw std::runtime_error("Could not load head image: " + fileName);
        cachedHeads_.push_back(image);
    }

    const auto nullHeadFileName = "./cpp/assets/" + themeName_ + "/heads/null.png";
    nullHead_ = cv::imread(nullHeadFileName, CV_LOAD_IMAGE_UNCHANGED);
    if (!nullHead_.data) {
        // TODO: Take the first one and dim it out.
        nullHead_ = cachedHeads_[0];
    }
}

void ThemeManager::loadBackgrounds(const cv::Size &size) {
    const ThemeInfo* info = getThemeInfo();
    for(int i=0; i < info->numBackgrounds; i++) {
        const auto fileName = "./cpp/assets/" + themeName_ + "/backgrounds/" + std::to_string(i + 1) + ".jpg";
        auto image = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);
        if (!image.data) throw std::runtime_error("Could not load background image: " + fileName);
        if (image.size() != size) {
            cv::resize(image, image, size, 0, 0, CV_INTER_AREA);
        }
        cachedBackgrounds_.push_back(image);
    }
}