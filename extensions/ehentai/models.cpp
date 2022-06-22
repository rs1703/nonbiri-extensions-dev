#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

#include <core/utility.h>
#include <ehentai/constants.h>
#include <ehentai/models.h>
#include <string.h>

int64_t parseDateString(const std::string &str)
{
  std::tm tm {};
  std::istringstream ss {str};
  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
  return std::mktime(&tm);
}

void fixCover(std::string &url)
{
  if (url.empty())
    return;

  static constexpr const char *exh = "exhentai.org";
  static constexpr const size_t exhLen = strlen(exh);
  if (!url.empty()) {
    if (auto pos = url.find(exh); pos != std::string::npos)
      url.replace(pos, exhLen, "ehgt.org");
  }
}

Manga::Manga(HTML &html, const std::string &path)
{
  const auto titleRo = html.selectFirst("#gj")->text();
  const auto category = html.selectFirst("#gdc")->text();

  if (const auto rows = html.select("#gdd .gdt2"); !rows.empty()) {
    if (!title.empty())
      description = "Title: " + title + "\n";
    if (!titleRo.empty())
      description += "Romaji: " + titleRo + "\n";

    auto language = rows[3]->text();
    language = language.substr(0, language.find(" "));

    description += "Category: " + category + "\n" 
                    "Uploader: " + html.selectFirst("#gdn")->firstChild()->text() + "\n" 
                    "Posted: " + rows[0]->text() + "\n" 
                    "Visible: " + rows[2]->text() + "\n" 
                    "Language: " + language + "\n"
                    "File Size: " + rows[4]->text() + "\n"
                    "Length: " + rows[5]->text() + "\n"
                    "Favorited: " + rows[6]->text();
  }

  for (const auto &row : html.select("#taglist .tc")) {
    const auto section = row->text();
    const bool isArtists = section.find(NS::artist) != std::string::npos;
    const bool isGroups = !isArtists && section.find(NS::group) != std::string::npos;
    const std::string &ns = isArtists || isGroups        ? ""
      : section.find(NS::language) != std::string::npos  ? NS::language
      : section.find(NS::parody) != std::string::npos    ? NS::parody
      : section.find(NS::character) != std::string::npos ? NS::character
      : section.find(NS::female) != std::string::npos    ? NS::female
                                                         : NS::male;

    auto &target = isArtists ? artists : isGroups ? authors : genres;
    for (const auto &child : row->next()->children()) {
      auto name = child->text();
      auto pos = name.find(Constants::delim);
      if (pos == std::string::npos) {
        target.push_back(ns + name);
      } else {
        do {
          const auto str = name.substr(0, pos);
          name.erase(0, pos + Constants::delimLen);
          pos = name.find(Constants::delim);

          target.push_back(ns + str);
          if (pos == std::string::npos)
            target.push_back(ns + name);
        } while (pos != std::string::npos);
      }
    }
  }

  this->path = path;
  if (const auto div = html.selectFirst("#gd1 div")) {
    const auto style = div->attr("style");
    coverUrl = style.substr(style.find("(") + 1, style.find(")") - style.find("(") - 1);
    fixCover(coverUrl);
  }
  title = html.selectFirst("#gj")->text();
  status = MangaStatus::Completed;
}

Manga::Manga(Element &element)
{
  path = Utils::stripDomain(element.selectFirst(".glname a")->attr("href"));
  const auto thumbnail = element.selectFirst(".glthumb img");
  coverUrl = thumbnail->attr("data-src");
  if (coverUrl.empty())
    coverUrl = thumbnail->attr("src");
  title = element.selectFirst(".glink")->text();
  fixCover(coverUrl);
}

Chapter::Chapter(HTML &html, const std::string &path)
{
  publishedAt = parseDateString(html.selectFirst("#gdd .gdt2")->text()) * 1000;
  this->path = path;
  this->name = "Chapter";
}