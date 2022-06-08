#include <mangadex/utility.h>

std::map<std::string, Json::Value> transform(const std::vector<Filter::Option> &filters)
{
  std::map<std::string, Json::Value> options;
  for (const auto &filter : filters)
    options.emplace(filter.key, filter.value);
  return options;
};

std::map<std::string, std::string> flip(const std::vector<Filter::Option> &filters)
{
  std::map<std::string, std::string> options;
  for (auto &filter : filters)
    options.emplace(filter.value, filter.key);
  return options;
};