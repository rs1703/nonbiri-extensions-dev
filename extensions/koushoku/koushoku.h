#ifndef KOUSHOKU_KOUSHOKU_H_
#define KOUSHOKU_KOUSHOKU_H_

#include <core/extension.h>

class Koushoku : public Extension
{
  std::vector<Filter> filters;

public:
  Koushoku();

  std::string latestsSelector();
  std::string latestsNextSelector();
  std::string latestsRequest(int page);
  Manga *parseLatestEntry(Element &element);

  std::string searchMangaSelector();
  std::string searchMangaNextSelector();
  std::string searchMangaRequest(int page, const std::string &query, const std::vector<FilterKV> &filters);
  Manga *parseSearchEntry(Element &element);

  Manga *parseManga(HTML &html);

  std::string chaptersRequest(const Manga &manga);
  std::vector<Chapter *> parseChapterEntries(const Manga &manga, HTML &html);

  std::string pagesRequest(const std::string &path);
  std::vector<std::string> parsePages(HTML &html);

  const std::vector<Filter> &getFilters();
};

#endif  // KOUSHOKU_KOUSHOKU_H_