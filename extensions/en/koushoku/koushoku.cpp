#include <iostream>

#include <koushoku/koushoku.h>

const char *thumbnailSelector = ".thumbnail img";

RegisterExtension(Koushoku);

Koushoku::Koushoku() : Extension {}
{
  client.setRateLimiter(new Http::RateLimiter(5));

  filters.add(new Filter::Select {{
    .key   = "sort",
    .title = "Sort",
    .options =
      {
        {"ID", "id"},
        {"Title", "title"},
        {"Created Date", "created_at", true},
        {"Uploaded Date", "published_at"},
        {"Pages", "pages"},
      },
  }});

  filters.add(new Filter::Select {{
    .key   = "order",
    .title = "Order",
    .options =
      {
        {"Ascending", "asc"},
        {"Descending", "desc", true},
      },
  }});
}

std::string Koushoku::latestsSelector() const
{
  return "#archives.feed .entries > .entry";
}

std::string Koushoku::latestsNextSelector() const
{
  return "#archives.feed .pagination .next";
}

std::shared_ptr<Http::Response> Koushoku::latestsRequest(int page) const
{
  return client.get(baseUrl + "/?page=" + std::to_string(page));
}

std::shared_ptr<Manga_t> Koushoku::parseLatestEntry(Element &element) const
{
  auto manga      = std::make_shared<Manga_t>();
  manga->path     = Utils::stripDomain(element.selectFirst("a")->attr("href"));
  manga->coverUrl = element.selectFirst(thumbnailSelector)->attr("src");
  manga->title    = element.selectFirst(".title")->text();
  return manga;
}

std::string Koushoku::searchMangaSelector() const
{
  return latestsSelector();
}

std::string Koushoku::searchMangaNextSelector() const
{
  return latestsNextSelector();
}

std::shared_ptr<Http::Response> Koushoku::searchMangaRequest(
  int page, const std::string &query, const std::vector<std::pair<std::string, std::string>> &filters) const
{
  SearchParams searchParams {{
    {"page", std::to_string(page)},
  }};

  for (const auto &[key, value] : filters)
    searchParams.add(key, value);

  if (!query.empty())
    searchParams.add("q", query);

  return client.get(baseUrl + "/search?" + searchParams.toString());
}

std::shared_ptr<Manga_t> Koushoku::parseSearchEntry(Element &element) const
{
  return parseLatestEntry(element);
}

std::shared_ptr<Http::Response> Koushoku::mangaRequest(const std::string &path) const
{
  return client.get(prependBaseUrl(path));
}

std::shared_ptr<Manga_t> Koushoku::parseManga(HTML &html) const
{
  auto manga      = std::make_shared<Manga_t>();
  manga->path     = Utils::stripDomain(html.selectFirst("link[rel=canonical]")->attr("href"));
  manga->coverUrl = html.selectFirst(thumbnailSelector)->attr("src");
  manga->title    = html.selectFirst(".metadata .title")->text();
  manga->status   = MangaStatus::Completed;

  auto artistElements = html.select(".metadata .artists a");
  for (auto &artistElement : artistElements)
    manga->artists.push_back(artistElement->text());
  auto circleElements = html.select(".metadata .circles a");
  for (auto &circleElement : circleElements)
    manga->authors.push_back(circleElement->text());
  auto tagElements = html.select(".metadata .tags a");
  for (auto &genreElement : tagElements)
    manga->genres.push_back(genreElement->text());

  return manga;
}

std::shared_ptr<Http::Response> Koushoku::chaptersRequest(const Manga_t &manga) const
{
  return client.get(prependBaseUrl(manga.path));
}

std::vector<std::shared_ptr<Chapter_t>> Koushoku::parseChapterEntries(const Manga_t &manga, HTML &html) const
{
  auto chapter = std::make_shared<Chapter_t>();
  std::vector<std::shared_ptr<Chapter_t>> chapters {chapter};

  chapter->path        = manga.path;
  chapter->name        = "Chapter";
  chapter->publishedAt = std::stoll(html.selectFirst(".metadata .published td:nth-child(2)")->attr("data-unix")) * 1000;
  return chapters;
}

std::shared_ptr<Http::Response> Koushoku::pagesRequest(const std::string &path) const
{
  return client.get(prependBaseUrl(path) + "/1");
}

std::vector<std::string> Koushoku::parsePages(HTML &html) const
{
  auto total = std::stoi(html.selectFirst(".total")->text());
  if (total == 0)
    throw std::runtime_error("No pages found");
  auto id = html.selectFirst("#reader")->attr("data-id");
  if (id.empty())
    throw std::runtime_error("Unknown archive id");

  std::vector<std::string> pages {};
  auto origin = Utils::stripPath(html.selectFirst(".page img")->attr("src"));
  for (int i = 1; i <= total; i++)
    pages.push_back(origin + "/data/" + id + "/" + std::to_string(i) + ".jpg");
  return pages;
}