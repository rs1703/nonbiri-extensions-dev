#ifndef MANGADEX_MODELS_H_
#define MANGADEX_MODELS_H_

#include <string>
#include <vector>

#include <core/models.h>
#include <json/json.h>

struct Manga : public Manga_t
{
  Manga(Json::Value &json, bool full = false);
};

struct Chapter : public Chapter_t
{
  std::string mangaId {};
  Chapter(Json::Value &json);
};

template<typename T>
struct Result : public T
{
  std::string result {};
  Result(Json::Value &json);
};

template<typename T, typename U = T>
struct Results
{
  int limit {};
  int offset {};
  int total {};
  std::vector<std::shared_ptr<U>> entries {};

  Results(Json::Value &json);
};

#endif  // MANGADEX_MODELS_H_