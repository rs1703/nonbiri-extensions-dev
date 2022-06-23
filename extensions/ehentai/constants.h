#ifndef EHENTAI_CONSTANTS_H_
#define EHENTAI_CONSTANTS_H_

#include <string>

namespace Constants
{
constexpr const char *sk {"f_search"};
constexpr const char *ck {"f_cats"};
constexpr const char *ak {"advsearch"};
constexpr const char *pk {"f_spf"};
constexpr const char *rk {"f_sr"};
constexpr const char *rdk {"f_srdd"};

constexpr const char* delim = " | ";
constexpr const size_t delimLen = 3;
const int ct {1023};
}  // namespace Constants

namespace NS
{
constexpr const char* language {"language:"};
constexpr const char* parody {"parody:"};
constexpr const char* character {"character:"};
constexpr const char* group {"group:"};
constexpr const char* artist {"artist:"};
constexpr const char* male {"male:"};
constexpr const char* female {"female:"};
}  // namespace NS

#endif  // EHENTAI_CONSTANTS_H_