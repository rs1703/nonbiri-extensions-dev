
#include <ehentai/constants.h>
#include <ehentai/ehentai.h>
#include <ehentai/models.h>

RegisterExtension(EHentai);

EHentai::EHentai() : Extension(), Filters(), Prefs(domain)
{
  const auto cookies = new Http::Cookies(domain);
  client.headers.set("Referer", baseUrl + "/");
  client.setCookies(cookies);

  const auto inputOnChange = [&](const InputOption *pref)
  {
    if (pref->value.empty()) {
      cookies->remove(pref->key);
    } else {
      cookies->add({
        .name = pref->key,
        .value = pref->value,
      });
    }
  };

  for (const std::string &key : {"ipb_member_id", "ipb_pass_hash"}) {
    Prefs::add(new Pref::Input {
      {
        .key = key,
        .title = "Cookie: " + key,
      },
      inputOnChange,
    });
  }

  Filters::add(new Checkbox({
    .key = Constants::ck,
    .title = "Categories",
    .options =
      {
        {.title = "Doujinshi", .value = "2", .defaultState = State::ON},
        {.title = "Manga", .value = "4", .defaultState = State::ON},
        {.title = "Artist CG", .value = "8"},
        {.title = "Game CG", .value = "16"},
        {.title = "Western", .value = "512"},
        {.title = "Non-H", .value = "256"},
        {.title = "Image Set", .value = "32"},
        {.title = "Cosplay", .value = "64"},
        {.title = "Asia Porn", .value = "128"},
        {.title = "Misc", .value = "1"},
      },
  }));

  Filters::add(new Checkbox({
    .key = Constants::ak,
    .title = "Advanced Search",
    .options =
      {
        {.title = "Search Gallery Name", .value = "f_sname", .defaultState = State::ON},
        {.title = "Search Gallery Tags", .value = "f_stags", .defaultState = State::ON},
        {.title = "Search Low-Power Tags", .value = "f_sdt1"},
        {.title = "Search Gallery Description", .value = "f_sdesc"},
        {.title = "Only Show Galleries With Torrents", .value = "f_sto"},
        {.title = "Show Downvoted Tags", .value = "f_sdt2"},
      },
  }));

  Filters::add(new Input({.key = Constants::pk, .title = "Minimum Pages"}));
  Filters::add(new Input({.key = Constants::pk, .title = "Maximum Pages"}));

  Filters::add(new Select({
    .key = Constants::rdk,
    .title = "Minimum Rating",
    .options =
      {
        {.title = "Any", .value = "0"},
        {.title = "2 stars", .value = "2"},
        {.title = "3 stars", .value = "3"},
        {.title = "4 stars", .value = "4"},
        {.title = "5 stars", .value = "5"},
      },
  }));
}

std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> EHentai::parseEntries(HTML &html) const
{
  auto elements = html.select(".glname");
  std::vector<std::shared_ptr<Manga_t>> entries;
  for (const auto &element : elements)
    entries.push_back(std::make_shared<Manga>(*element->parent().get()));

  const bool hasNext = html.selectLast("a[onclick='return false']")->text() == ">";
  return {entries, hasNext};
}

std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> EHentai::getLatests(int page) const
{
  SearchParams searchParams({
    {"page", std::to_string(page)},
    {Constants::ck, "1017"},
  });

  auto res = client.get(prependBaseUrl("?" + searchParams.toString()));
  if (res->statusCode != 200)
    return {};

  HTML html {res->body};
  return parseEntries(html);
}
std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> EHentai::searchManga(
  int page, const std::string &query, const std::vector<std::pair<std::string, std::string>> &filters) const
{
  SearchParams searchParams;
  if (!query.empty())
    searchParams.set(Constants::sk, query);
  if (page > 1)
    searchParams.set("page", std::to_string(page - 1));

  int category {};
  for (const auto &[key, value] : filters) {
    if (key == Constants::ck) {
      category += std::stoi(value);
    } else if (key == Constants::ak) {
      searchParams.set(Constants::ak, "1");
      searchParams.set(value, "on");
    } else {
      if ((key == Constants::rdk || key == Constants::pk) && std::stoi(value) <= 0)
        continue;
      if (key == Constants::rdk)
        searchParams.set(Constants::rk, "on");
      searchParams.add(key, value);
    }
  }

  if (category > 0)
    searchParams.set(Constants::ck, std::to_string(Constants::ct - category));

  auto res = client.get(prependBaseUrl("?" + searchParams.toString()));
  if (res->statusCode != 200)
    return {};

  HTML html {res->body};
  return parseEntries(html);
}

std::shared_ptr<Manga_t> EHentai::getManga(const std::string &path) const
{
  auto res = client.get(prependBaseUrl(path));
  if (res->statusCode != 200)
    return nullptr;

  HTML html {res->body};
  return std::make_shared<Manga>(html, path);
}

std::vector<std::shared_ptr<Chapter_t>> EHentai::getChapters(const Manga_t &manga) const
{
  auto res = client.get(prependBaseUrl(manga.path));
  if (res->statusCode != 200)
    return {};

  HTML html {res->body};
  return {std::make_shared<Chapter>(html, manga.path)};
}

std::vector<std::string> EHentai::getPages(const std::string &) const
{
  // TODO
}
