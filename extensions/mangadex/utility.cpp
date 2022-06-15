#include <mangadex/utility.h>

std::vector<std::pair<std::string, Json::Value>> optionsToPairs(const std::vector<Filter::Option> &options)
{
  std::vector<std::pair<std::string, Json::Value>> result;
  for (const auto &option : options)
    result.push_back({option.key, option.value});
  return result;
}

std::map<std::string, size_t> createOptionsIndex(const std::vector<Filter::Option> &options)
{
  std::map<std::string, size_t> result;
  for (size_t i = 0; i < options.size(); i++)
    result[options[i].key] = i;
  return result;
}

std::map<std::string, std::string> optionsToMap(const std::vector<Filter::Option> &options)
{
  std::map<std::string, std::string> result;
  for (auto &filter : options)
    result.emplace(filter.value, filter.key);
  return result;
}

Json::Value stringsToJson(const std::vector<std::string> &arr)
{
  Json::Value result;
  for (const auto &str : arr)
    result.append(str);
  return result;
}

Json::Value stringsToJson(const std::vector<std::vector<std::string>> &arrs)
{
  Json::Value result;
  for (const auto &arr : arrs) {
    Json::Value item;
    for (const auto &str : arr)
      item.append(str);
    result.append(item);
  }
  return result;
}