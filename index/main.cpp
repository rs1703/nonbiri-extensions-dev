#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <index/extension.h>
#include <index/utility.h>
#include <json/json.h>

namespace fs = std::filesystem;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
static const std::string platform {"windows"};
#else
static const std::string platform {"linux"};
#endif

std::vector<ExtensionInfo> loadExtensions()
{
  if (!fs::exists(platform))
    throw std::runtime_error("Platform directory does not exist");

  std::vector<ExtensionInfo> extensions;
  for (const auto &entry : fs::directory_iterator(platform)) {
    if (!entry.is_regular_file())
      continue;

    const std::string path = entry.path().string();
    if (!fs::exists(path))
      throw std::runtime_error("Extension file does not exist");

    auto handle = Utils::loadLibrary(path);
    if (handle == nullptr)
      throw std::runtime_error("Unable to load extension");
    auto create = (create_t)Utils::getSymbol(handle, "create");
    if (create == nullptr)
      throw std::runtime_error("Unable to get fn symbol 'create' from extension");
    Extension *ext = create();
    if (ext == nullptr)
      throw std::runtime_error("Unable to create extension");

    extensions.push_back({
      .domain = ext->domain,
      .baseUrl = ext->baseUrl,
      .name = ext->name,
      .description = ext->description,
      .language = ext->language,
      .version = ext->version,
      .isNsfw = ext->isNsfw,
      .path = entry.path().generic_string(),
    });

    delete ext;
    Utils::freeLibrary(handle);
  }
  return extensions;
}

Json::Value readIndex()
{
  static const std::string path {platform + ".json"};
  if (!fs::exists(path))
    return Json::Value(Json::objectValue);

  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("Unable to open index file");

  Json::Value root {};
  file >> root;
  file.close();
  return root;
}

void writeIndex(const Json::Value &root)
{
  std::ofstream out {platform + ".json"};
  Json::StyledWriter writer;
  out << writer.write(root);
  out.close();

  std::ofstream out2 {platform + ".min.json"};
  Json::FastWriter writer2;
  out2 << writer2.write(root);
  out2.close();
}

int main()
{
  std::vector<ExtensionInfo> extensions {};
  try {
    auto extensions = loadExtensions();

    Json::Value root {};
    for (const auto &info : extensions)
      root[info.domain] = info.toJson();

    writeIndex(root);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    throw;
  }
}