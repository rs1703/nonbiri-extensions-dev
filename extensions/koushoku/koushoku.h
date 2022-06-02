#ifndef KOUSHOKU_KOUSHOKU_H_
#define KOUSHOKU_KOUSHOKU_H_

#include <core/extension.h>

class Koushoku : public Extension
{
  std::vector<Filter> filters;

public:
  Koushoku();

  std::string latestsSelector() const;
  std::string latestsNextSelector() const;
  std::string latestsRequest(int page) const;
  std::shared_ptr<Manga_t> parseLatestEntry(Element &element) const;

  std::string searchMangaSelector() const;
  std::string searchMangaNextSelector() const;
  std::string searchMangaRequest(int page, const std::string &query, const std::vector<FilterKV> &filters) const;
  std::shared_ptr<Manga_t> parseSearchEntry(Element &element) const;

  std::shared_ptr<Manga_t> parseManga(HTML &html) const;

  std::string chaptersRequest(const Manga_t &manga) const;
  std::vector<std::shared_ptr<Chapter_t>> parseChapterEntries(const Manga_t &manga, HTML &html) const;

  std::string pagesRequest(const std::string &path) const;
  std::vector<std::string> parsePages(HTML &html) const;

  const std::vector<Filter> &getFilters() const;
};

#endif  // KOUSHOKU_KOUSHOKU_H_