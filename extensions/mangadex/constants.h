#ifndef MANGADEX_CONSTANTS_H_
#define MANGADEX_CONSTANTS_H_

#include <map>
#include <string>
#include <vector>

#include <core/object.h>
#include <json/json.h>

namespace Constants
{
const std::string cdnBaseUrl {"https://uploads.mangadex.org"};
const std::string apiBaseUrl {"https://api.mangadex.org"};
const std::string apiMangaUrl {apiBaseUrl + "/manga"};
const std::string apiChapterUrl {apiBaseUrl + "/chapter"};
const std::string atHomePostUrl {"https://api.mangadex.network/report"};

constexpr const uint16_t mangaLimit {25};
constexpr const uint16_t latestChaptersLimit {100};

constexpr const char *manga {"manga"};
constexpr const char *coverArt {"cover_art"};
constexpr const char *scanlationGroup {"scanlation_group"};
constexpr const char *uploader {"uploader"};
constexpr const char *author {"author"};
constexpr const char *artist {"artist"};
}  // namespace Constants

namespace Language
{
const std::string title {"Original Language(s)"};
const std::string key {"originalLanguage[]"};
const std::string excludedKey {"excludedOriginalLanguage[]"};

const std::vector<StateOption> options {
  {.title = "Japanese (Manga)", .value = "ja"},
  {.title = "Chinese (Manhua)", .value = "zh"},
  {.title = "Korean (Manhwa)", .value = "ko"},
};

const std::map<std::string, std::string> map {
  {"en", "English"},
  {"ja", "Japanese"},
  {"jp", "Japanese"},
  {"zh", "Chinese"},
  {"ko", "Korean"},
};
}  // namespace Language

namespace ContentRating
{
const std::string title {"Content Rating"};
const std::string key {"contentRating[]"};

const std::vector<StateOption> options {
  {.title = "Safe", .value = "safe", .defaultState = State::ON},
  {.title = "Suggestive", .value = "suggestive", .defaultState = State::ON},
  {.title = "Erotica", .value = "erotica", .defaultState = State::ON},
  {.title = "Pornographic", .value = "pornographic"},
};
}  // namespace ContentRating

namespace Demographic
{
const std::string title {"Publication Demographic"};
const std::string key {"publicationDemographic[]"};

const std::vector<StateOption> options {
  {.title = "None", .value = "none"},
  {.title = "Shounen", .value = "shounen"},
  {.title = "Shoujo", .value = "shoujo"},
  {.title = "Seinen", .value = "seinen"},
  {.title = "Josei", .value = "josei"},
};
}  // namespace Demographic

namespace Status
{
const std::string title {"Status"};
const std::string key {"status[]"};

const std::vector<StateOption> options {
  {.title = "Ongoing", .value = "ongoing"},
  {.title = "Completed", .value = "completed"},
  {.title = "Hiatus", .value = "hiatus"},
  {.title = "Cancelled", .value = "cancelled"},
};
}  // namespace Status

namespace Sort
{
const std::string title {"Sort"};
const std::string key {"sort"};

const std::vector<StateOption> options {
  {.title = "Title", .value = "title"},
  {.title = "Updated Date", .value = "updatedAt"},
  {.title = "Created Date", .value = "createdAt"},
  {.title = "Latest Uploaded Chapter Date", .value = "latestUploadedChapter", .defaultState = State::ON},
  {.title = "Number of follows", .value = "followedCount"},
  {.title = "Relevance", .value = "relevance"},
  {.title = "Year", .value = "year"},
};
}  // namespace Sort

namespace Order
{
const std::string title {"Order"};
const std::string key {"order"};

const std::vector<StateOption> options {
  {.title = "Ascending", .value = "asc"},
  {.title = "Descending", .value = "desc", .defaultState = State::ON},
};
}  // namespace Order

namespace Tag
{
const std::string title {"Tag"};
const std::string key {"includedTags[]"};
const std::string excludedKey {"excludedTags[]"};

const std::vector<StateOption> options {
  {.title = "Oneshot", .value = "0234a31e-a729-4e28-9d6a-3f87c4966b9e"},
  {.title = "Thriller", .value = "07251805-a27e-4d59-b488-f0bfbec15168"},
  {.title = "Award Winning", .value = "0a39b5a1-b235-4886-a747-1d05d216532d"},
  {.title = "Reincarnation", .value = "0bc90acb-ccc1-44ca-a34a-b9f3a73259d0"},
  {.title = "Sci-Fi", .value = "256c8bd9-4904-4360-bf4f-508a76d67183"},
  {.title = "Time Travel", .value = "292e862b-2d17-4062-90a2-0356caa4ae27"},
  {.title = "Genderswap", .value = "2bd2e8d0-f146-434a-9b51-fc9ff2c5fe6a"},
  {.title = "Loli", .value = "2d1f5d56-a1e5-4d0d-a961-2193588b08ec"},
  {.title = "Traditional Games", .value = "31932a7e-5b8e-49a6-9f12-2afa39dc544c"},
  {.title = "Official Colored", .value = "320831a8-4026-470b-94f6-8353740e6f04"},
  {.title = "Historical", .value = "33771934-028e-4cb3-8744-691e866a923e"},
  {.title = "Monsters", .value = "36fd93ea-e8b8-445e-b836-358f02b3d33d"},
  {.title = "Action", .value = "391b0423-d847-456f-aff0-8b0cfc03066b"},
  {.title = "Demons", .value = "39730448-9a5f-48a2-85b0-a70db87b1233"},
  {.title = "Psychological", .value = "3b60b75c-a2d7-4860-ab56-05f391bb889c"},
  {.title = "Ghosts", .value = "3bb26d85-09d5-4d2e-880c-c34b974339e9"},
  {.title = "Animals", .value = "3de8c75d-8ee3-48ff-98ee-e20a65c86451"},
  {.title = "Long Strip", .value = "3e2b8dae-350e-4ab8-a8ce-016e844b9f0d"},
  {.title = "Romance", .value = "423e2eae-a7a2-4a8b-ac03-a8351462d71d"},
  {.title = "Ninja", .value = "489dd859-9b61-4c37-af75-5b18e88daafc"},
  {.title = "Comedy", .value = "4d32cc48-9f00-4cca-9b5a-a839f0764984"},
  {.title = "Mecha", .value = "50880a9d-5440-4732-9afb-8f457127e836"},
  {.title = "Anthology", .value = "51d83883-4103-437c-b4b1-731cb73d786c"},
  {.title = "Boys' Love", .value = "5920b825-4181-4a17-beeb-9918b0ff7a30"},
  {.title = "Incest", .value = "5bd0e105-4481-44ca-b6e7-7544da56b1a3"},
  {.title = "Crime", .value = "5ca48985-9a9d-4bd8-be29-80dc0303db72"},
  {.title = "Survival", .value = "5fff9cde-849c-4d78-aab0-0d52b2ee1d25"},
  {.title = "Zombies", .value = "631ef465-9aba-4afb-b0fc-ea10efe274a8"},
  {.title = "Reverse Harem", .value = "65761a2a-415e-47f3-bef2-a9dababba7a6"},
  {.title = "Sports", .value = "69964a64-2f90-4d33-beeb-f3ed2875eb4c"},
  {.title = "Superhero", .value = "7064a261-a137-4d3a-8848-2d385de3a99c"},
  {.title = "Martial Arts", .value = "799c202e-7daa-44eb-9cf7-8a3c0441531e"},
  {.title = "Fan Colored", .value = "7b2ce280-79ef-4c09-9b58-12b7c23a9b78"},
  {.title = "Samurai", .value = "81183756-1453-4c81-aa9e-f6e1b63be016"},
  {.title = "Magical Girls", .value = "81c836c9-914a-4eca-981a-560dad663e73"},
  {.title = "Mafia", .value = "85daba54-a71c-4554-8a28-9901a8b0afad"},
  {.title = "Adventure", .value = "87cc87cd-a395-47af-b27a-93258283bbc6"},
  {.title = "User Created", .value = "891cf039-b895-47f0-9229-bef4c96eccd4"},
  {.title = "Virtual Reality", .value = "8c86611e-fab7-4986-9dec-d1a2f44acdd5"},
  {.title = "Office Workers", .value = "92d6d951-ca5e-429c-ac78-451071cbf064"},
  {.title = "Video Games", .value = "9438db5a-7e2a-4ac0-b39e-e0d95a34b8a8"},
  {.title = "Post-Apocalyptic", .value = "9467335a-1b83-4497-9231-765337a00b96"},
  {.title = "Sexual Violence", .value = "97893a4c-12af-4dac-b6be-0dffb353568e"},
  {.title = "Crossdressing", .value = "9ab53f92-3eed-4e9b-903a-917c86035ee3"},
  {.title = "Magic", .value = "a1f53773-c69a-4ce5-8cab-fffcd90b1565"},
  {.title = "Girls' Love", .value = "a3c67850-4684-404e-9b7f-c69850ee5da6"},
  {.title = "Harem", .value = "aafb99c1-7f60-43fa-b75f-fc9502ce29c7"},
  {.title = "Military", .value = "ac72833b-c4e9-4878-b9db-6c8a4a99444a"},
  {.title = "Wuxia", .value = "acc803a4-c95a-4c22-86fc-eb6b582d82a2"},
  {.title = "Isekai", .value = "ace04997-f6bd-436e-b261-779182193d3d"},
  {.title = "4-Koma", .value = "b11fda93-8f1d-4bef-b2ed-8803d3733170"},
  {.title = "Doujinshi", .value = "b13b2a48-c720-44a9-9c77-39c9979373fb"},
  {.title = "Philosophical", .value = "b1e97889-25b4-4258-b28b-cd7f4d28ea9b"},
  {.title = "Gore", .value = "b29d6a3d-1569-4e7a-8caf-7557bc92cd5d"},
  {.title = "Drama", .value = "b9af3a63-f058-46de-a9a0-e0c13906197a"},
  {.title = "Medical", .value = "c8cbe35b-1b2b-4a3f-9c37-db84c4514856"},
  {.title = "School Life", .value = "caaa44eb-cd40-4177-b930-79d3ef2afe87"},
  {.title = "Horror", .value = "cdad7e68-1419-41dd-bdce-27753074a640"},
  {.title = "Fantasy", .value = "cdc58593-87dd-415e-bbc0-2ec27bf404cc"},
  {.title = "Villainess", .value = "d14322ac-4d6f-4e9b-afd9-629d5f4d8a41"},
  {.title = "Vampires", .value = "d7d1730f-6eb0-4ba6-9437-602cac38664c"},
  {.title = "Delinquents", .value = "da2d50ca-3018-4cc0-ac7a-6b7d472a29ea"},
  {.title = "Monster Girls", .value = "dd1f77c5-dea9-4e2b-97ae-224af09caf99"},
  {.title = "Shota", .value = "ddefd648-5140-4e5f-ba18-4eca4071d19b"},
  {.title = "Police", .value = "df33b754-73a3-4c54-80e6-1a74a8058539"},
  {.title = "Web Comic", .value = "e197df38-d0e7-43b5-9b09-2842d0c326dd"},
  {.title = "Slice of Life", .value = "e5301a23-ebd9-49dd-a0cb-2add944c7fe9"},
  {.title = "Aliens", .value = "e64f6742-c834-471d-8d72-dd51fc02b835"},
  {.title = "Cooking", .value = "ea2bc92d-1c26-4930-9b7c-d5c0dc1b6869"},
  {.title = "Supernatural", .value = "eabc5b4c-6aff-42f3-b657-3e90cbd00b75"},
  {.title = "Mystery", .value = "ee968100-4191-4968-93d3-f82d72be7e46"},
  {.title = "Adaptation", .value = "f4122d1c-3b44-44d0-9936-ff7502c39ad3"},
  {.title = "Music", .value = "f42fbf9e-188a-447b-9fdc-f19dc1e4d685"},
  {.title = "Full Color", .value = "f5ba408b-0e7a-484d-8d49-4e9125ac96de"},
  {.title = "Tragedy", .value = "f8f62932-27da-4fe4-8ee1-6779a8c5edba"},
  {.title = "Gyaru", .value = "fad12b5e-68ba-460e-b933-9ae8318f5b65"},
};
}  // namespace Tag

#endif  // MANGADEX_CONSTANTS_H_