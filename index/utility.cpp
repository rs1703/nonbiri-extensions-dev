#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  include <windows.h>
#else
#  include <dlfcn.h>
#endif
#include <index/utility.h>

namespace Utils
{
void *loadLibrary(const std::string &path)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  return LoadLibrary(path.c_str());
#else
  return dlopen(path.c_str(), RTLD_LAZY);
#endif
}

void *getSymbol(void *handle, const std::string &symbol)
{
  if (handle == nullptr)
    return nullptr;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  return GetProcAddress((HMODULE)handle, symbol.c_str());
#else
  return dlsym(handle, symbol.c_str());
#endif
}

void freeLibrary(void *handle)
{
  if (handle == nullptr)
    return;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  FreeLibrary((HMODULE)handle);
#else
  dlclose(handle);
#endif
}
}  // namespace Utils
