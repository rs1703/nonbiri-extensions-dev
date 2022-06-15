#ifndef NONBIRI_INDEX_EXTENSION_H_
#define NONBIRI_INDEX_EXTENSION_H_

#include <string>

#include <json/json.h>

struct ExtensionInfo
{
  std::string domain {};
  std::string baseUrl {};
  std::string name {};
  std::string language {};
  std::string version {};
  bool isNsfw {};

  std::string path {};

  Json::Value toJson() const
  {
    Json::Value root {};
    root["domain"] = domain;
    root["name"] = name;
    root["language"] = language;
    root["version"] = version;
    root["path"] = path;
    root["isNsfw"] = isNsfw;
    return root;
  }
};

class Extension : public ExtensionInfo
{
public:
  Extension() = default;
  virtual ~Extension() = default;
};

typedef Extension *(*create_t)();
#define RegisterExtension(extension) \
  EXPORT_API extension *create() \
  { \
    return new extension(); \
  }

#endif  // NONBIRI_INDEX_EXTENSION_H_