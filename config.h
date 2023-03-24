#ifndef CONFIG_H
#define CONFIG_H

static const char *const fontRelativePath = "resources/MiSans-Medium.ttf";

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STRING "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#endif

extern const char *executablePath;

extern struct nk_font *fontLarge;
extern struct nk_font *fontMedium;
extern struct nk_font *fontSmall;

#endif
