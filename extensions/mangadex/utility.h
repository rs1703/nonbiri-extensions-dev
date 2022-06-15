#ifndef MANGADEX_UTILITY_H_
#define MANGADEX_UTILITY_H_

#include <map>
#include <string>
#include <vector>

#include <core/filter.h>
#include <json/json.h>

std::vector<std::pair<std::string, Json::Value>> optionsToPairs(const std::vector<Filter::Option> &options);
std::map<std::string, size_t> createOptionsIndex(const std::vector<Filter::Option> &options);
std::map<std::string, std::string> optionsToMap(const std::vector<Filter::Option> &filters);
Json::Value stringsToJson(const std::vector<std::string> &arr);
Json::Value stringsToJson(const std::vector<std::vector<std::string>> &arr);

#endif  // MANGADEX_UTILITY_H_