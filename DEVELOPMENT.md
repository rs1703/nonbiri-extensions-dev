# Development

This document describes how to create an extension for [name]. If you plan to contribute by creating or fixing the existing extension, please read it carefully before you start.

Note that this document is not complete and may change in the future.

## Prerequisites

The following tools are required to write and compile an extension:

- CMake 3.15+
- Linux: GCC 9+
- Windows: MSVC 14.29 (Visual Studio 2019 Version 16.10 + 16.11)

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

| Name     | Description                                                                                   |
| -------- | --------------------------------------------------------------------------------------------- |
| id       | Must be unique. Use domain name to ensure uniqueness, otherwise [we] will manually review it. |
| name     |                                                                                               |
| language | ISO 639-1 compliant language code.                                                            |
| version  | Must be a positive integer and incremented with any change to the code.                       |
| baseUrl  |                                                                                               |
| isNsfw   | Optional.                                                                                     |

## Core Library

The core library is automatically included in the extension and can be used anywhere in the code by including one of the header files. It contains the base classes and models, http and html parsing library, and some other helper functions.

### Base Classes

> #include <core/extension.h>

There are two recommended base classes which can be inherited from:

1. If your goal is to obtain the content by parsing the response - `HttpExtension`
2. If your goal is to obtain the content by parsing the web page - `ParsedExtension`

If these are not enough, you can inherit from `Extension` and implement your own logic.

### Http

> #include <core/http.h>

Http is a wrapper around the CURL library.

- `Http::Headers` - A header container
  - `set(string name, string value)`
  - `remove(string name)`
  - `has(string name)` - returns bool
  - `empty()`
  - `clear()`
- `Http::Request` - A request object
  - `string url`
  - `string method`
  - `string body`
  - `Headers headers`
- `Http::Response` - A response object
  - `string body`
  - `long statusCode`
- `Http::Interceptor` - A base class for intercepting requests and responses
  - Derived class should implement `intercept(Http::Interceptor::Chain&)` method
- `Http::Interceptor::Chain`
  - `request()` - returns a reference to the original `Http::Request`
    - Any changes to the object will be reflected in the original request
  - `proceed()` - will process the `Http::Request` and return a pointer to `Http::Response`
    - Can only be called once, otherwise it will do nothing and will always return `nullptr`
- `Http::RateLimiter`
  - The constructor takes two parameters:
    - `int64_t size` - the maximum number of requests
    - `int64_t duration` - the time period in seconds, default to 1s
    - example: `RateLimiter(10, 1)` - which means 10 requests per second
- `Http::Client` - A class which provides a simple interface for making HTTP requests
  - `get(string url, Http::Headers = {})` - returns a pointer to `Http::Response`
  - `post(string url, string body, Http::Headers = {})` - returns a pointer to `Http::Response`
  - `send(Http::Request&)` - returns a pointer to `Http::Response`
  - `download(string url, string outputPath)` - returns a `long` which is the status code of the request
  - `setRateLimiter(Http::RateLimiter)`
  - `setDefaultHeader(string name, string value)`
  - `removeDefaultHeader(string name)`
  - `addInterceptor(Http::Interceptor)`

## Call Flow

#### 1. Latest Manga

Invoked when the user opens the extensions. The app calls `Extension::getLatests(int page)`, which should return `tuple<vector<Manga_t>, bool hasNextPage>`. The method has a different implementation depending on the base class.

- HttpExtension will call `latestsRequest(int page)`, which should return a pointer to `Http::Response`
  - `Http::Response` will be passed to `parseLatestEntries(Http::Response&)`, which should return `tuple<vector<Manga_t>, bool hasNextPage>`
- ParsedExtension will call `latestsRequest(int page)`, which should return a pointer to `Http::Response`
  - `Http::Response` will be parsed as HTML and will either
    - Call `parseLatestEntries(HTML&)`, which should return `tuple<vector<Manga_t>, bool hasNextPage>` - _if the method has been implemented_
    - Or call `latestsSelector()`, which should return a CSS selector
      - The CSS selector will be used to extract the elements from the HTML, which then will be iterated over and passed to `parseLatestEntry(Element&)`, which should return a pointer to `Manga_t`, resulting in `vector<Manga_t>`
      - Then call `latestsNextSelector()` and check if there is a next page, resulting in `hasNextPage`

#### 2. Search Manga

Invoked when the user searches for a manga or applies a filter. The app calls `Extension::searchManga(string page, string query, vector<pair<string, string> filters)`, which should return `tuple<vector<Manga_t>, bool hasNextPage>`. The method has a different implementation depending on the base class, but the flow should be similar to _Latest Manga_.

#### 3. Get Manga

Invoked when the user opens the manga page. The app calls `Extension::getManga(string path)`, which should return a pointer to `Manga_t`. The method has a different implementation depending on the base class.

- HttpExtension will call `mangaRequest(string path)`, which should return a pointer to `Http::Response`
  - `Http::Response` will be passed to `parseManga(Http::Response&)`, which should return a pointer to `Manga_t`
- ParsedExtension will call `mangaRequest(string path)` which should return a `Http::Response`
  - `Http::Response` will be parsed as HTML and passed to `parseManga(HTML&)`, which should return a pointer to `Manga_t`

#### 4. Get Chapters

Also invoked when the user opens the manga page, usually invoked after _Get Manga_. The app calls `Extension::getChapters(Manga_t&)` or `Extension::getChapters(string path)`, which should return `vector<Chapter_t>`. The method has a different implementation depending on the base class, but the flow should be similar to _Latest Manga_ and _Search Manga_ except the _hasNextPage_ and _nextSelector_ parts.

#### 5. Get Pages

Invoked when the user opens the chapter page. The app calls `Extension::getPages(string path)`, which should return `vector<string>`. The method has a different implementation depending on the base class.

- HttpExtension will call `pagesRequest(string path)`, which should return a pointer to `Http::Response`
  - `Http::Response` will be passed to `parsePages(Http::Response)`, which should return `vector<string>`
- ParsedExtension will call `pagesRequest(string path)` which should return a pointer to `Http::Response`
  - `Http::Response` will be parsed as HTML and passed to `parsePages(HTML&)`, which should return `vector<string>`

## Filters

TODO.

## Prefs

TODO.
