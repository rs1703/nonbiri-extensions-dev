#ifndef EHENTAI_MODELS_H_
#define EHENTAI_MODELS_H_

#include <core/models.h>
#include <core/parser.h>

struct Manga : public Manga_t
{
  Manga(HTML &html, const std::string &path);
  Manga(Element &element);
};

struct Chapter : public Chapter_t
{
  Chapter(HTML &html, const std::string &path);
};

#endif  // EHENTAI_MODELS_H_