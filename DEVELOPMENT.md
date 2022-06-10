# Development

This document describes how to create an extension for [name]. If you plan to contribute by creating or fixing the existing extension, please read it carefully before you start.

Note that this document is not complete and may change in the future.

## Prerequisites

The following tools are required to write and compile an extension:

- GCC 9+
- MSVC/Visual Studio 2019 (Windows)
- CMake 3.15+

To avoid confusion about the compiler, it's recommended to just use the latest version of the above tools (except for MSVC/Visual Studio 2019).

You can use any code editor, but it's recommended to use a code editor that supports syntax hightlighting and auto-formatting, so that you can write a readable and maintainable code. If you are a beginner, you can try VSCode with C/C++ Extension Pack.

## Structure

Extension must follow the following structure:

```console
$ tree extensions/<language>/<extensionId>
extensions/<language>/<extensionId>
├── CMakeLists.txt
├── icon.png
├── <extensionId>.cpp
└── <extensionId>.h

0 directories, 4 files
```
Structure must be simple; Code can be split into several header and source files, but subdirectories are not allowed.

#### CMakeLists.txt

Basic extension information such as name, language, version and baseUrl should be stored as variables in the CMakeLists.txt, example:

```cmake
cmake_minimum_required(VERSION 3.15)

set(id extensionId)
set(name "Extension name")
set(language en)
set(version 1)
set(baseUrl https://example.com)
```

| Field | Description |
| --- | --- |
| id | Must be unique. Use domain name to ensure uniqueness, otherwise [we] will manually review it. |
| name |  |
| language | ISO 639-1 compliant language code. |
| version | Must be a positive integer and incremented with any change to the code. |
| baseUrl |  |
| isNsfw | Optional. |


## Base Classes

There are two recommended base classes which can be inherited from:

1. If your goal is to obtain the content by parsing the response - `HttpExtension`
2. If your goal is to obtain the content by parsing the web page - `ParsedExtension`

If these are not enough, you can inherit from `Extension` and implement your own logic.

## Call Flow
#### 1. Latest Manga

Invoked when the user opens the extensions. The app calls `Extension::getLatests(page)`, which should return `tuple<vector<Manga_t>, hasNextPage>`. The method has a different implementation depending on the base class.

- HttpExtension will call `latestsRequest(page)`, which should return `Response`
  - The `Response` will be passed to `parseLatestEntries(response)`, which should return `tuple<vector<Manga_t>, hasNextPage>`
- ParsedExtension will call `latestsRequest(page)`, which should return `Response`
  - The `Response` will be parsed as HTML and will either
    - Call `parseLatestEntries(html)`, which should return `tuple<vector<Manga_t>, hasNextPage>` - *if the method has been implemented*
    - Or call `latestsSelector()`, which should return a CSS selector
      - The CSS selector will be used to extract the elements from the HTML, which then will be iterated over and passed to `parseLatestEntry(element)`, which should return `Manga_t`, resulting in `vector<Manga_t>`
      - Then call `latestsNextSelector()` and check if there is a next page, resulting in `hasNextPage`

#### 2. Search Manga

Invoked when the user searches for a manga or applies a filter. The app calls `Extension::searchManga(page, query, filters)`, which should return `tuple<vector<Manga_t>, hasNextPage>`. The method has a different implementation depending on the base class, but the flow should be similar to *Latest Manga*.

#### 3. Get Manga

Invoked when the user opens the manga page. The app calls `Extension::getManga(path)`, which should return `Manga_t`. The method has a different implementation depending on the base class.

- HttpExtension will call `mangaRequest(path)`, which should return `Response`
  - The `Response` will be passed to `parseManga(response)`, which should return `Manga_t`
- ParsedExtension will call `mangaRequest(path)` which should return a `Response`
  - The `Response` will be parsed as HTML and passed to `parseManga(html)`, which should return `Manga_t`

#### 4. Get Chapters

Also invoked when the user opens the manga page, usually invoked after *Get Manga*. The app calls `Extension::getChapters(Manga_t)` or `Extension::getChapters(path)`, which should return `vector<Chapter_t>`. The method has a different implementation depending on the base class, but the flow should be similar to *Latest Manga* and *Search Manga* except the *hasNextPage* and *nextSelector* parts.

#### 5. Get Pages

Invoked when the user opens the chapter page. The app calls `Extension::getPages(path)`, which should return `vector<string>`. The method has a different implementation depending on the base class.

- HttpExtension will call `pagesRequest(path)`, which should return `Response`
  - The `Response` will be passed to `parsePages(response)`, which should return `vector<string>`
- ParsedExtension will call `pagesRequest(path)` which should return a `Response`
  - The `Response` will be parsed as HTML and passed to `parsePages(html)`, which should return `vector<string>`

## Filters

To be continuted.