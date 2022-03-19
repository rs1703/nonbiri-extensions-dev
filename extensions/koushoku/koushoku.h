#ifndef KOUSHOKU_KOUSHOKU_H_
#define KOUSHOKU_KOUSHOKU_H_

#include <core/extension/extension.h>

class Koushoku : public Extension
{
public:
  std::string latestsSelector();
  std::string latestsNextSelector();
  std::string latestsRequest(int page);
  Manga *parseLatestEntry(CElement &element);

  std::string searchMangaSelector();
  std::string searchMangaNextSelector();
  std::string searchMangaRequest(int page, const char *query);
  Manga *parseSearchEntry(CElement &element);

  Manga *parseManga(CHtml &html);

  std::string chaptersRequest(const Manga &manga);
  std::vector<Chapter *> parseChapterEntries(const Manga &manga, CHtml &html);

  std::string pagesRequest(const Chapter &chapter);
  std::vector<std::string> parsePages(const Chapter &chapter, CHtml &html);
};

#endif  // KOUSHOKU_KOUSHOKU_H_