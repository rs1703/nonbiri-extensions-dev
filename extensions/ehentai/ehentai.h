#ifndef EHENTAI_EHENTAI_H_
#define EHENTAI_EHENTAI_H_

#include <core/extension.h>
#include <core/filters.h>
#include <core/prefs.h>

struct EHentai : public Extension, Filters, Prefs
{
  EHentai();

  std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> parseEntries(HTML &html) const;

  std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> getLatests(int page) const override;
  std::tuple<std::vector<std::shared_ptr<Manga_t>>, bool> searchManga(
    int page, const std::string &query, const std::vector<std::pair<std::string, std::string>> &filters) const override;
  std::shared_ptr<Manga_t> getManga(const std::string &path) const override;
  std::vector<std::shared_ptr<Chapter_t>> getChapters(const Manga_t &manga) const override;
  std::vector<std::string> getPages(const std::string &path) const override;
};

#endif  // EHENTAI_EHENTAI_H_