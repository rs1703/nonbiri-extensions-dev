#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <index/database.h>
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

std::string getExtensionId(const ExtensionInfo &info)
{
  static constexpr const char *sql {"SELECT id FROM extension WHERE domain = ?"};
  sqlite3_stmt *stmt = nullptr;

  int exit = sqlite3_prepare_v2(Database::instance, sql, -1, &stmt, nullptr);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_bind_text(stmt, 1, info.domain.c_str(), -1, SQLITE_STATIC);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_step(stmt);

  int64_t id {};
  if (exit == SQLITE_ROW)
    id = sqlite3_column_int64(stmt, 0);
  sqlite3_finalize(stmt);
  if (exit != SQLITE_ROW && exit != SQLITE_DONE)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  return std::to_string(id);
}

bool hasExtension(const ExtensionInfo &info)
{
  static constexpr const char *sql {"SELECT 1 FROM extension WHERE domain = ?"};
  sqlite3_stmt *stmt = nullptr;

  int exit = sqlite3_prepare_v2(Database::instance, sql, -1, &stmt, nullptr);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_bind_text(stmt, 1, info.domain.c_str(), -1, SQLITE_STATIC);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_step(stmt);
  if (exit != SQLITE_ROW && exit != SQLITE_DONE)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  return exit == SQLITE_ROW;
}

std::string addExtension(const ExtensionInfo &info)
{
  static constexpr const char *sql {"INSERT INTO extension (domain) VALUES (?)"};
  sqlite3_stmt *stmt = nullptr;

  int exit = sqlite3_prepare_v2(Database::instance, sql, -1, &stmt, nullptr);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_bind_text(stmt, 1, info.domain.c_str(), -1, SQLITE_STATIC);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_step(stmt);

  int64_t id {};
  if (exit == SQLITE_DONE)
    id = sqlite3_last_insert_rowid(Database::instance);
  sqlite3_finalize(stmt);
  if (exit != SQLITE_DONE)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  return std::to_string(id);
}

std::string updateExtension(const ExtensionInfo &info)
{
  static constexpr const char *sql {
    "UPDATE extension SET obsolete = 0"
    " WHERE domain = ?",
  };
  sqlite3_stmt *stmt = nullptr;

  int exit = sqlite3_prepare_v2(Database::instance, sql, -1, &stmt, nullptr);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_bind_text(stmt, 1, info.domain.c_str(), -1, SQLITE_STATIC);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (exit != SQLITE_DONE)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  return getExtensionId(info);
}

void removeExtension(const std::string &id)
{
  static constexpr const char *sql {
    "UPDATE extension SET obsolete = 1"
    " WHERE id = ?",
  };
  sqlite3_stmt *stmt = nullptr;

  int exit = sqlite3_prepare_v2(Database::instance, sql, -1, &stmt, nullptr);
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_bind_int64(stmt, 1, std::stoll(id));
  if (exit != SQLITE_OK)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
  exit = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (exit != SQLITE_DONE)
    throw std::runtime_error(sqlite3_errmsg(Database::instance));
}

int main()
{
  std::vector<ExtensionInfo> extensions {};
  try {
    Database::initialize();
    extensions = loadExtensions();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    throw;
  }

  Database::Tx t;
  try {
    Json::Value root {};
    for (const auto &info : extensions) {
      std::string id {};
      if (hasExtension(info))
        id = updateExtension(info);
      else
        id = addExtension(info);
      root[id] = info.toJson();
    }

    Json::Value index = readIndex();
    for (const auto &id : index.getMemberNames())
      if (!root.isMember(id))
        removeExtension(id);

    writeIndex(root);
  } catch (const std::exception &e) {
    t.rollback();
    std::cerr << e.what() << std::endl;
    throw;
  }
}