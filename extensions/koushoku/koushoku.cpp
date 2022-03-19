#include <stdexcept>

#include <core/utils/utils.h>
#include <koushoku/koushoku.h>

const char *thumbnailSelector = ".thumbnail img";

RegisterExtension(Koushoku);

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

Manga *Koushoku::parseLatestEntry(CElement &element)
{
  auto manga = new Manga();

  manga->url = stripDomain(element.selectFirst("a")->attr("href"));
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

std::string Koushoku::searchMangaRequest(int page, const char *query)
{
  return http::get(baseUrl + "/search?page=" + std::to_string(page) + "&q=" + query);
}

Manga *Koushoku::parseSearchEntry(CElement &element)
{
  return parseLatestEntry(element);
}

Manga *Koushoku::parseManga(CHtml &html)
{
  auto manga = new Manga();

  manga->url = stripDomain(html.selectFirst("link[rel=canonical]")->attr("href"));
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
  return http::get(baseUrl + manga.url);
}

std::vector<Chapter *> Koushoku::parseChapterEntries(const Manga &manga, CHtml &html)
{
  auto chapter = new Chapter();
  std::vector<Chapter *> chapters {chapter};

  chapter->url = stripDomain(manga.url);
  chapter->name = "Chapter";
  chapter->publishedAt = std::stoi(html.selectFirst(".metadata .publishedAt td:nth-child(2)")->text()) * 1000;

  return chapters;
}

std::string Koushoku::pagesRequest(const Chapter &chapter)
{
  return http::get(baseUrl + chapter.url + "/1");
}

std::vector<std::string> Koushoku::parsePages(const Chapter &chapter, CHtml &html)
{
  auto total = std::stoi(html.selectFirst(".total")->text());
  if (total == 0)
    throw std::runtime_error("No pages found");

  auto id = html.selectFirst(".id")->attr("value");
  if (id.empty())
    throw std::runtime_error("Unknown archive id");

  std::vector<std::string> pages;
  auto origin = stripPath(html.selectFirst(".page img")->attr("src"));

  for (int i = 1; i <= total; i++)
    pages.push_back(origin + "/data" + id + "/" + std::to_string(i) + ".jpg");
  return pages;
}