#ifndef MANGADEX_UTILITY_H_
#define MANGADEX_UTILITY_H_

#include <map>
#include <string>
#include <vector>

#include <core/filters.h>
#include <json/json.h>

std::map<std::string, Json::Value> transform(const std::vector<Filter::Option> &filters);
std::map<std::string, std::string> flip(const std::vector<Filter::Option> &filters);

#endif  // MANGADEX_UTILITY_H_