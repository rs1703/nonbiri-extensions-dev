#ifndef KOUSHOKU_KOUSHOKU_H_
#define KOUSHOKU_KOUSHOKU_H_

#include <core/extension.h>

struct Koushoku : public Extension, public Prefs
{
  Koushoku();

  std::string latestsSelector() const override;
  std::string latestsNextSelector() const override;
  std::shared_ptr<Http::Response> latestsRequest(int page) const override;
  std::shared_ptr<Manga_t> parseLatestEntry(Element &element) const override;

  std::string searchMangaSelector() const override;
  std::string searchMangaNextSelector() const override;
  std::shared_ptr<Http::Response> searchMangaRequest(
    int page, const std::string &query, const std::vector<Filter::Pair> &filters) const;
  std::shared_ptr<Manga_t> parseSearchEntry(Element &element) const override;

  std::shared_ptr<Http::Response> mangaRequest(const std::string &path) const override;
  std::shared_ptr<Manga_t> parseManga(HTML &html) const override;

  std::shared_ptr<Http::Response> chaptersRequest(const Manga_t &manga) const override;
  std::vector<std::shared_ptr<Chapter_t>> parseChapterEntries(const Manga_t &manga, HTML &html) const override;

  std::shared_ptr<Http::Response> pagesRequest(const std::string &path) const override;
  std::vector<std::string> parsePages(HTML &html) const override;

  Prefs *getPrefs() const override;
};

#endif  // KOUSHOKU_KOUSHOKU_H_