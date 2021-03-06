#ifndef MANGADEX_MANGADEX_H_
#define MANGADEX_MANGADEX_H_

#include <core/extension.h>
#include <core/filters.h>
#include <core/prefs.h>
#include <json/json.h>

struct MangaDex : public HttpExtension, Filters, Prefs
{
  MangaDex();

  std::shared_ptr<Http::Response> latestsRequest(int page) const override;
  std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> parseLatestEntries(const Http::Response &response) const override;

  std::shared_ptr<Http::Response> searchMangaRequest(
    int page, const std::string &query, const std::vector<std::pair<std::string, std::string>> &filters) const override;
  std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> parseSearchEntries(const Http::Response &response) const override;

  std::shared_ptr<Http::Response> mangaRequest(const std::string &path) const override;
  std::shared_ptr<Manga_t> parseManga(const Http::Response &response) const override;

  std::shared_ptr<Http::Response> chaptersRequest(const Manga_t &manga, int offset) const;
  std::shared_ptr<Http::Response> chaptersRequest(const Manga_t &manga) const override;
  std::vector<std::shared_ptr<Chapter_t>> parseChapterEntries(const Manga_t &manga, const Http::Response &response) const override;

  std::shared_ptr<Http::Response> pagesRequest(const std::string &path) const override;
  std::vector<std::string> parsePages(const Http::Response &response) const override;

private:
  void applyLanguagePref(SearchParams &searchParams) const;
  void applyContentRatingPref(SearchParams &searchParams) const;
  void applyTagPref(SearchParams &searchParams) const;
  void applyPrefs(SearchParams &searchParams) const;
};

#endif  // MANGADEX_MANGADEX_H_