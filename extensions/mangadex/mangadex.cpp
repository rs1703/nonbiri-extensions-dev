#include <core/utility.h>
#include <mangadex/constants.h>
#include <mangadex/mangadex.h>
#include <mangadex/models.h>

RegisterExtension(MangaDex);

MangaDex::MangaDex() : HttpExtension(), Filters(), Prefs(domain)
{
  client.setRateLimiter(new Http::RateLimiter(5));
  client.headers.set("Referer", baseUrl + "/");

  Prefs::add(new TriState({
    .key = Language::key,
    .excludedKey = Language::excludedKey,
    .title = Language::title,
    .options = Language::options,
  }));

  Prefs::add(new Checkbox({
    .key = ContentRating::key,
    .title = ContentRating::title,
    .options = ContentRating::options,
  }));

  Prefs::add(new TriState({
    .key = Tag::key,
    .excludedKey = Tag::excludedKey,
    .title = Tag::title,
    .options = Tag::options,
  }));

  Filters::add(new Filter::Hidden(Language::excludedKey));
  Filters::add(new TriState({
    .key = Language::key,
    .excludedKey = Language::excludedKey,
    .title = Language::title,
    .options = Language::options,
  }));

  Filters::add(new Checkbox({
    .key = ContentRating::key,
    .title = ContentRating::title,
    .options = ContentRating::options,
  }));

  Filters::add(new Checkbox({
    .key = Demographic::key,
    .title = Demographic::title,
    .options = Demographic::options,
  }));

  Filters::add(new Checkbox({
    .key = Status::key,
    .title = Status::title,
    .options = Status::options,
  }));

  Filters::add(new Select({
    .key = Sort::key,
    .title = Sort::title,
    .options = Sort::options,
  }));

  Filters::add(new Select({
    .key = Order::key,
    .title = Order::title,
    .options = Order::options,
  }));

  Filters::add(new Filter::Hidden(Tag::excludedKey));
  Filters::add(new TriState({
    .key = Tag::key,
    .excludedKey = Tag::excludedKey,
    .title = Tag::title,
    .options = Tag::options,
  }));
}

std::shared_ptr<Http::Response> MangaDex::latestsRequest(int page) const
{
  SearchParams searchParams {{
    {"includes[]", Constants::coverArt},
    {"limit", std::to_string(Constants::mangaLimit)},
    {"offset", std::to_string(Constants::mangaLimit * (page - 1))},
    {"order[latestUploadedChapter]", "desc"},
  }};
  applyPrefs(searchParams);

  return client.get(Constants::apiMangaUrl + "?" + searchParams.toString());
}

std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> MangaDex::parseLatestEntries(const Http::Response &response) const
{
  Json::Value root;
  Json::Reader reader;
  if (!reader.parse(response.body, root))
    return {};

  Results<Manga, Manga_t> result {root};
  return {result.entries, result.offset + result.limit < result.total};
}

std::shared_ptr<Http::Response> MangaDex::searchMangaRequest(
  int page, const std::string &query, const std::vector<std::pair<std::string, std::string>> &filters) const
{
  SearchParams searchParams {{
    {"offset", std::to_string(Constants::mangaLimit * (page - 1))},
    {"limit", std::to_string(Constants::mangaLimit)},
    {"includes[]", Constants::coverArt},
  }};

  if (!query.empty())
    searchParams.add("title", query);

  std::string sort {"latestUploadedChapter"};
  std::string order {"desc"};

  for (const auto &[key, value] : filters) {
    if (key == Sort::key)
      sort = value;
    else if (key == Order::key)
      order = value;
    else
      searchParams.add(key, value);
  }

  searchParams.add("order[" + sort + "]", order);
  return client.get(Constants::apiMangaUrl + "?" + searchParams.toString());
}

std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> MangaDex::parseSearchEntries(const Http::Response &response) const
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

std::vector<std::shared_ptr<Chapter_t>> MangaDex::parseChapterEntries(const Manga_t &manga, const Http::Response &response) const
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

void applyState(SearchParams &searchParams, const StateObject *pref)
{
  if (pref != nullptr) {
    for (const auto &option : pref->options) {
      if (option.state == State::ON)
        searchParams.add(option.key.empty() ? pref->key : option.key, option.value);
    }
  }
}

void applyTriState(SearchParams &searchParams, const TriState *pref)
{
  if (pref != nullptr) {
    for (const auto &option : pref->options) {
      if (option.state == State::ON) {
        searchParams.add(option.key.empty() ? pref->key : option.key, option.value);
      } else if (option.state == State::EX) {
        searchParams.add(pref->excludedKey, option.value);
      }
    }
  }
}

void MangaDex::applyLanguagePref(SearchParams &searchParams) const
{
  auto pref = Prefs::get<TriState>(Language::key);
  if (pref != nullptr)
    applyTriState(searchParams, pref);
}

void MangaDex::applyContentRatingPref(SearchParams &searchParams) const
{
  const auto pref = Prefs::get<Checkbox>(ContentRating::key);
  if (pref != nullptr)
    applyState(searchParams, pref);
}

void MangaDex::applyTagPref(SearchParams &searchParams) const
{
  const auto pref = Prefs::get<TriState>(Tag::key);
  if (pref != nullptr)
    applyTriState(searchParams, pref);
}

void MangaDex::applyPrefs(SearchParams &searchParams) const
{
  applyLanguagePref(searchParams);
  applyContentRatingPref(searchParams);
  applyTagPref(searchParams);
}
