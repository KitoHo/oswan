#include <string.h>

typedef struct tagSYMBOLENTRY
{
    unsigned long address;
    const char* name;
} SYMBOLENTRY;

#define DL_EXPORT_SYM(sym) \
__attribute__ ((section (".export_string"))) \
static const char _string_##sym[] = #sym;\
__attribute__ ((section (".export_table"))) \
static const SYMBOLENTRY _sym_##sym = { (unsigned long)&sym, _string_##sym };

/* file extension name */
int GetFileType(char* pname)
{
    if (pname)
        strcpy(pname, "WSC");
    return 0;
}

/* to get defualt path */
int GetDefaultPath(char* path)
{
    if (path)
        strcpy(path, "A:\\GAME");
    return 0;
}
/* module description, optional */
int GetModuleName(char* name, int code_page)
{
    if (name && (0 == code_page)) // ansi
        strcpy(name, "Oswan.SIM");
    return 0;
}
DL_EXPORT_SYM(GetFileType)
DL_EXPORT_SYM(GetDefaultPath)
DL_EXPORT_SYM(GetModuleName)

