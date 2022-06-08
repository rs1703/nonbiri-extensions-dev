#include <iostream>

#include <core/utility.h>
#include <mangadex/constants.h>
#include <mangadex/mangadex.h>
#include <mangadex/models.h>

RegisterExtension(MangaDex);

MangaDex::MangaDex() : Extension {}, Prefs {*this, {Language::pref, ContentRating::pref}}
{
  client.setRateLimiter(new Http::RateLimiter(5));
  client.setDefaultHeader("Referer", baseUrl + "/");

  filters.add(ExcludableCheckbox {
    Language::key,  // key
    Language::excludedKey,  // excludedKey
    Language::title,  // title
    Language::options,  // options
  });

  filters.add(Checkbox {
    ContentRating::key,  // key
    ContentRating::title,  // title
    ContentRating::options,  // options
  });

  filters.add(Checkbox {
    Demographic::key,  // key
    Demographic::title,  // title
    Demographic::options,  // options
  });

  filters.add(Checkbox {
    Status::key,  // key
    Status::title,  // title
    Status::options,  // options
  });

  filters.add(Select {
    Sort::key,  // key
    Sort::title,  // title
    Sort::options,  // options
  });

  filters.add(Select {
    Order::key,  // key
    Order::title,  // title
    Order::options,  // options
  });

  filters.add(ExcludableCheckbox {
    Tag::key,  // key
    Tag::excludedKey,  // excludedKey
    Tag::title,  // title
    Tag::options,  // options
  });
}

std::shared_ptr<Http::Response> MangaDex::latestsRequest(int page) const
{
  SearchParams searchParams {{
    {"offset", std::to_string(Constants::latestChaptersLimit * (page - 1))},
    {"limit", std::to_string(Constants::latestChaptersLimit)},
    {"order[publishAt]", "desc"},
    {"includeFutureUpdates", "0"},
  }};
  {
    const Pref *pref = getPref(Language::prefKey);
    if (pref != nullptr)
      for (const auto &value : pref->values)
        searchParams.add(Language::key, value.asString());
  }
  {
    const Pref *pref = getPref(ContentRating::prefKey);
    if (pref != nullptr)
      for (const auto &value : pref->values)
        searchParams.add(ContentRating::key, value.asString());
  }
  return client.get(Constants::apiChapterUrl + "?" + searchParams.toString());
}

std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> MangaDex::parseLatestEntries(
  const Http::Response &response) const
{
  std::vector<std::string> mangaIds;
  bool hasNext {};
  {
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(response.body, root))
      return {};

    Results<Chapter> result {root};
    hasNext = result.limit + result.offset < result.total;
    for (const auto &entry : result.entries)
      mangaIds.push_back(entry->mangaId);
  }

  SearchParams searchParams {{
    {"includes[]", Constants::coverArt},
    {"limit", std::to_string(mangaIds.size())},
  }};
  {
    const Pref *pref = getPref(Language::prefKey);
    if (pref != nullptr)
      for (const auto &value : pref->values)
        searchParams.add(Language::key, value.asString());
  }
  {
    const Pref *pref = getPref(ContentRating::prefKey);
    if (pref != nullptr)
      for (const auto &value : pref->values)
        searchParams.add(ContentRating::key, value.asString());
  }
  for (const auto &id : mangaIds)
    searchParams.add("ids[]", id);

  const auto mangaResponse = client.get(Constants::apiMangaUrl + "?" + searchParams.toString());
  if (mangaResponse == nullptr)
    return {};

  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(mangaResponse->body, root))
    return {};

  Results<Manga, Manga_t> result {root};
  return {result.entries, hasNext};
}

std::shared_ptr<Http::Response> MangaDex::searchMangaRequest(
  int page, const std::string &query, const std::vector<Filter::Pair> &filters) const
{
  SearchParams searchParams {{
    {"offset", std::to_string(Constants::mangaLimit * (page - 1))},
    {"limit", std::to_string(Constants::mangaLimit)},
    {"includes[]", Constants::coverArt},
  }};

  if (!query.empty())
    searchParams.add("title", query);

  // todo: apply filters to searchParams
  return client.get(Constants::apiMangaUrl + "/?" + searchParams.toString());
}

std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> MangaDex::parseSearchEntries(
  const Http::Response &response) const
{
  if (response.statusCode != 200)
    throw std::runtime_error("Failed to get search results: HTTP " + std::to_string(response.statusCode));

  if (response.statusCode == 204)
    return {};

  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(response.body, root))
    return {};

  Results<Manga, Manga_t> result {root};
  return {result.entries, result.limit + result.offset < result.total};
}

std::shared_ptr<Http::Response> MangaDex::mangaRequest(const std::string &path) const
{
  SearchParams searchParams {{
    {"includes[]", Constants::coverArt},
    {"includes[]", Constants::author},
    {"includes[]", Constants::artist},
  }};
  return client.get(Constants::apiBaseUrl + path + "?" + searchParams.toString());
}

std::shared_ptr<Manga_t> MangaDex::parseManga(const Http::Response &response) const
{
  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(response.body, root))
    return nullptr;
  return std::make_shared<Result<Manga>>(root);
}

std::shared_ptr<Http::Response> MangaDex::chaptersRequest(const Manga_t &manga, int offset) const
{
  SearchParams searchParams {{
    {"includes[]", Constants::scanlationGroup},
    {"includes[]", Constants::uploader},
    {"limit", "500"},
    {"offset", std::to_string(offset)},
    {"order[volume]", "desc"},
    {"order[chapter]", "desc"},
  }};
  for (const auto &option : ContentRating::options)
    searchParams.add("contentRating[]", option.value);
  return client.get(Constants::apiBaseUrl + manga.path + "/feed?" + searchParams.toString());
}

std::shared_ptr<Http::Response> MangaDex::chaptersRequest(const Manga_t &manga) const
{
  return chaptersRequest(manga, 0);
}

std::vector<std::shared_ptr<Chapter_t>> MangaDex::parseChapterEntries(
  const Manga_t &manga, const Http::Response &response) const
{
  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(response.body, root))
    return {};

  Results<Chapter, Chapter_t> result {root};
  while (result.limit + result.offset < result.total) {
    result.offset += result.limit;
    const auto nextResponse = chaptersRequest(manga, result.offset);
    if (nextResponse == nullptr)
      return {};

    Json::Value nextRoot;
    if (!reader.parse(nextResponse->body, nextRoot))
      return {};

    Results<Chapter, Chapter_t> nextResult {nextRoot};
    result.entries.insert(result.entries.end(), nextResult.entries.begin(), nextResult.entries.end());
  }
  return result.entries;
}

std::shared_ptr<Http::Response> MangaDex::pagesRequest(const std::string &path) const
{
  const std::string id = path.substr(path.find_last_of('/') + 1);
  return client.get(Constants::apiBaseUrl + "/at-home/server/" + id);
}

std::vector<std::string> MangaDex::parsePages(const Http::Response &respose) const
{
  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(respose.body, root))
    return {};

  const std::string host = root["baseUrl"].asString();
  const std::string hash = root["chapter"]["hash"].asString();

  std::vector<std::string> pages;
  Json::Value &data = root["chapter"]["data"];
  if (data.isArray()) {
    for (const auto &page : data)
      pages.push_back(host + "/data/" + hash + "/" + page.asString());
  }
  return pages;
}

Prefs *MangaDex::getPrefs() const
{
  return Prefs::getInstance();
}