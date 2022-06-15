#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <core/utility.h>
#include <mangadex/constants.h>
#include <mangadex/models.h>

int64_t parseDateString(const std::string &str)
{
  std::tm tm {};
  std::istringstream ss {str};
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S%z");
  return std::mktime(&tm);
}

Manga::Manga(Json::Value &json, bool full)
{
  auto &attributes = json["attributes"];
  if (attributes.empty())
    throw std::runtime_error("'attributes' not found");

  auto &rels = json["relationships"];
  if (rels.empty())
    throw std::runtime_error("'relationships' not found");
  else if (!rels.isArray())
    throw std::runtime_error("'relationships' is not an array");

  const std::string id = json["id"].asString();
  if (id.empty())
    throw std::runtime_error("'id' not found");

  path = "/manga/" + id;

  auto &titleMap = attributes["title"];
  if (titleMap.empty())
    throw std::runtime_error("'title' not found");

  if (titleMap.isMember("en")) {
    title = titleMap["en"].asString();
  } else if (titleMap.isMember("ja-ro")) {
    title = titleMap["ja-ro"].asString();
  } else if (titleMap.isMember("ja")) {
    title = titleMap["ja"].asString();
  } else {
    for (auto &v : titleMap) {
      if (!title.empty())
        break;
      title = v.asString();
    }
  }

  if (title.empty())
    throw std::runtime_error("'title' is empty");

  for (auto &rel : rels) {
    const std::string type = rel["type"].asString();
    auto &attrs            = rel["attributes"];

    if (type == Constants::coverArt) {
      const auto fileName = attrs["fileName"].asString();
      if (!fileName.empty())
        coverUrl = Constants::cdnBaseUrl + "/covers/" + id + "/" + fileName + ".512.jpg";
      if (!full)
        break;
    } else if (type == Constants::author) {
      const auto name = attrs["name"].asString();
      if (!name.empty())
        authors.push_back(name);
    } else if (type == Constants::artist) {
      const auto name = attrs["name"].asString();
      if (!name.empty())
        artists.push_back(name);
    }
  }

  if (coverUrl.empty())
    throw std::runtime_error("'coverUrl' is empty");

  if (!full)
    return;

  auto &descriptionMap = attributes["description"];
  if (!descriptionMap.empty() && descriptionMap.isMember("en"))
    description = descriptionMap["en"].asString();

  auto &tags = attributes["tags"];
  if (tags.isArray()) {
    for (auto &tag : tags) {
      const std::string id = tag["id"].asString();
      if (!id.empty()) {
        const auto &it = Tag::map.find(id);
        if (it != Tag::map.end())
          genres.push_back(it->second);
      }
    }
  }
  std::sort(genres.begin(), genres.end());

  std::string contentRating = attributes["contentRating"].asString();
  if (contentRating == "safe") {
    contentRating = "";
  } else if (!contentRating.empty()) {
    contentRating = "Content rating: " + Utils::capitalize(contentRating);
    genres.push_back(contentRating);
  }

  const std::string demographic = Utils::capitalize(attributes["publicationDemographic"].asString());
  if (!demographic.empty())
    genres.push_back(demographic);

  const auto &it = Language::map.find(attributes["originalLanguage"].asString());
  if (it != Language::map.end())
    genres.push_back(it->second);

  const std::string publicationStatus = attributes["status"].asString();
  if (publicationStatus == "ongoing")
    status = MangaStatus::Ongoing;
  else if (publicationStatus == "completed")
    status = MangaStatus::Completed;
  else if (publicationStatus == "hiatus")
    status = MangaStatus::Hiatus;
  else if (publicationStatus == "cancelled")
    status = MangaStatus::Cancelled;
}

Chapter::Chapter(Json::Value &json)
{
  auto &attributes = json["attributes"];
  if (attributes.empty())
    throw std::runtime_error("'attributes' not found");

  auto &rels = json["relationships"];
  if (rels.empty())
    throw std::runtime_error("'relationships' not found");
  if (!rels.isArray())
    throw std::runtime_error("'relationships' is not an array");

  const std::string id = json["id"].asString();
  if (id.empty())
    throw std::runtime_error("'id' not found");

  path        = "/chapter/" + id;
  publishedAt = parseDateString(attributes["publishAt"].asString()) * 1000;

  const std::string volume  = attributes["volume"].asString();
  const std::string chapter = attributes["chapter"].asString();
  const std::string title   = attributes["title"].asString();

  if (!volume.empty())
    name = "Vol. " + volume;
  if (!chapter.empty())
    name += " Ch. " + chapter;
  if (!title.empty()) {
    if (!name.empty())
      name += " - ";
    name += title;
  }
  if (name.empty())
    name = "Oneshot";

  for (auto &rel : rels) {
    const std::string type = rel["type"].asString();
    auto &attrs            = rel["attributes"];

    if (type == Constants::manga) {
      mangaId = rel["id"].asString();
    } else if (type == Constants::scanlationGroup) {
      const std::string name = attrs["name"].asString();
      if (!name.empty())
        groups.push_back(name);
    }
  }
}

template class Result<Manga>;
template class Results<Manga>;
template class Results<Manga, Manga_t>;
template class Result<Chapter>;
template class Results<Chapter>;
template class Results<Chapter, Chapter_t>;

template<>
Result<Manga>::Result(Json::Value &json) : Manga {json["data"], true}
{
  if (json.isMember("result"))
    result = json["result"].asString();
  else
    throw std::runtime_error("field 'result' not found");
}

template<typename T>
Result<T>::Result(Json::Value &json) : T {json["data"]}
{
  if (json.isMember("result"))
    result = json["result"].asString();
  else
    throw std::runtime_error("field 'result' not found");
}

template<typename T, typename U>
Results<T, U>::Results(Json::Value &json)
{
  if (json.isMember("limit"))
    limit = json["limit"].asInt();
  else
    throw std::runtime_error("field 'limit' not found");

  if (json.isMember("offset"))
    offset = json["offset"].asInt();
  else
    throw std::runtime_error("field 'offset' not found");

  if (json.isMember("total"))
    total = json["total"].asInt();
  else
    throw std::runtime_error("field 'total' not found");

  if (json.isMember("data")) {
    if (!json["data"].isArray())
      throw std::runtime_error("field 'data' is not an array");

    for (auto &entry : json["data"]) {
      entries.push_back(std::make_shared<T>(entry));
    }
  } else {
    throw std::runtime_error("field 'data' not found");
  }
}