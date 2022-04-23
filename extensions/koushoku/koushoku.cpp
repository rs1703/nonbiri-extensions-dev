#include <core/utils.h>
#include <koushoku/koushoku.h>

const char *thumbnailSelector = ".thumbnail img";

RegisterExtension(Koushoku);

Koushoku::Koushoku() : Extension()
{
  filters = {
    {
      "Sort",
      "sort",
      {
        {"ID", "id"},
        {"Title", "title"},
        {"Created date", "created_at"},
        {"Published date", "published_at"},
      },
    },
    {
      "Order",
      "order",
      {
        {"Ascending", "asc"},
        {"Descending", "desc"},
      },
    },
  };
}

std::string Koushoku::latestsSelector()
{
  return "#archives.feed .entries > .entry";
}

std::string Koushoku::latestsNextSelector()
{
  return "#archives.feed .pagination .next";
}

std::string Koushoku::latestsRequest(int page)
{
  return http::get(baseUrl + "/?page=" + std::to_string(page));
}

Manga *Koushoku::parseLatestEntry(Element &element)
{
  auto manga = new Manga();

  manga->path = stripDomain(element.selectFirst("a")->attr("href"));
  manga->coverUrl = element.selectFirst(thumbnailSelector)->attr("src");
  manga->title = element.selectFirst(".title")->text();

  return manga;
}

std::string Koushoku::searchMangaSelector()
{
  return latestsSelector();
}

std::string Koushoku::searchMangaNextSelector()
{
  return latestsNextSelector();
}

std::string Koushoku::searchMangaRequest(int page, const std::string &query, const std::vector<Filter> &filters)
{
  std::string url {baseUrl + "/search?page=" + std::to_string(page) + "&q=" + query};
  for (auto &filter : filters)
    url += "&" + filter.key + "=" + filter.value;

  return http::get(url);
}

Manga *Koushoku::parseSearchEntry(Element &element)
{
  return parseLatestEntry(element);
}

Manga *Koushoku::parseManga(HTML &html)
{
  auto manga = new Manga();

  manga->path = stripDomain(html.selectFirst("link[rel=canonical]")->attr("href"));
  manga->coverUrl = html.selectFirst(thumbnailSelector)->attr("src");
  manga->title = html.selectFirst(".metadata .title")->text();
  manga->status = MangaStatus::Completed;

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

std::string Koushoku::chaptersRequest(const Manga &manga)
{
  return http::get(prependBaseUrl(manga.path));
}

std::vector<Chapter *> Koushoku::parseChapterEntries(const Manga &manga, HTML &html)
{
  auto chapter = new Chapter();
  std::vector<Chapter *> chapters {chapter};

  chapter->path = manga.path;
  chapter->name = "Chapter";
  chapter->publishedAt = std::stoi(html.selectFirst(".metadata .published td:nth-child(2)")->attr("data-unix")) * 1000;

  return chapters;
}

std::string Koushoku::pagesRequest(const std::string &path)
{
  return http::get(prependBaseUrl(path) + "/1");
}

std::vector<std::string> Koushoku::parsePages(HTML &html)
{
  auto total = std::stoi(html.selectFirst(".total")->text());
  if (total == 0)
    throw std::runtime_error("No pages found");

  auto id = html.selectFirst("#reader")->attr("data-id");
  if (id.empty())
    throw std::runtime_error("Unknown archive id");

  std::vector<std::string> pages;
  auto origin = stripPath(html.selectFirst(".page img")->attr("src"));

  for (int i = 1; i <= total; i++)
    pages.push_back(origin + "/data/" + id + "/" + std::to_string(i) + ".jpg");
  return pages;
}

const std::vector<FilterInfo> &Koushoku::getFilters()
{
  return filters;
}