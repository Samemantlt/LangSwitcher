/*
 * converter.c - EN <-> RU keyboard layout conversion implementation
 *
 * Layout: standard QWERTY (EN) <-> JCUKEN / ЙЦУКЕН (RU)
 *
 * Mapping convention (indices are parallel across en_* and ru_* arrays):
 *
 *  EN lower:  q w e r t y u i o p [ ] a s d f g h j k l ; ' z x c v b n m , . / `
 *  RU lower:  й ц у к е н г ш щ з х ъ ф ы в а п р о л д ж э я ч с м и т ь б ю . ё
 *
 *  EN upper:  Q W E R T Y U I O P { } A S D F G H J K L : " Z X C V B N M < > ? ~
 *  RU upper:  Й Ц У К Е Н Г Ш Щ З Х Ъ Ф Ы В А П Р О Л Д Ж Э Я Ч С М И Т Ь Б Ю , Ё
 */

#include "converter.h"

#include <wchar.h>

/* ---------------------------------------------------------------------------
 * Mapping tables
 * --------------------------------------------------------------------------- */

/* EN lower-case keys, left-to-right across keyboard rows */
static const wchar_t k_en_lower[] =
    L"qwertyuiop[]asdfghjkl;'zxcvbnm,./`";

/* Corresponding RU characters (same indices) */
static const wchar_t k_ru_lower[] =
    /* й  ц  у  к  е  н  г  ш  щ  з  х  ъ */
    L"\u0439\u0446\u0443\u043a\u0435\u043d\u0433\u0448\u0449\u0437\u0445\u044a"
    /* ф  ы  в  а  п  р  о  л  д  ж  э */
    L"\u0444\u044b\u0432\u0430\u043f\u0440\u043e\u043b\u0434\u0436\u044d"
    /* я  ч  с  м  и  т  ь  б  ю  .  ё */
    L"\u044f\u0447\u0441\u043c\u0438\u0442\u044c\u0431\u044e."
    L"\u0451";

/* EN upper-case / shifted keys */
static const wchar_t k_en_upper[] =
    L"QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?~";

/* Corresponding RU upper-case characters */
static const wchar_t k_ru_upper[] =
    /* Й  Ц  У  К  Е  Н  Г  Ш  Щ  З  Х  Ъ */
    L"\u0419\u0426\u0423\u041a\u0415\u041d\u0413\u0428\u0429\u0417\u0425\u042a"
    /* Ф  Ы  В  А  П  Р  О  Л  Д  Ж  Э */
    L"\u0424\u042b\u0412\u0410\u041f\u0420\u041e\u041b\u0414\u0416\u042d"
    /* Я  Ч  С  М  И  Т  Ь  Б  Ю  ,  Ё */
    L"\u042f\u0427\u0421\u041c\u0418\u0422\u042c\u0411\u042e,"
    L"\u0401";

/* ---------------------------------------------------------------------------
 * Public API
 * --------------------------------------------------------------------------- */

void convert_layout(wchar_t *text, size_t len)
{
    static size_t lower_len = 0;
    static size_t upper_len = 0;

    /* Compute lengths once */
    if (lower_len == 0) lower_len = wcslen(k_en_lower);
    if (upper_len == 0) upper_len = wcslen(k_en_upper);

    for (size_t i = 0; i < len; i++) {
        wchar_t c = text[i];

        /* Try EN lower -> RU lower */
        for (size_t j = 0; j < lower_len; j++) {
            if (c == k_en_lower[j]) { text[i] = k_ru_lower[j]; goto next; }
        }
        /* Try EN upper -> RU upper */
        for (size_t j = 0; j < upper_len; j++) {
            if (c == k_en_upper[j]) { text[i] = k_ru_upper[j]; goto next; }
        }
        /* Try RU lower -> EN lower */
        for (size_t j = 0; j < lower_len; j++) {
            if (c == k_ru_lower[j]) { text[i] = k_en_lower[j]; goto next; }
        }
        /* Try RU upper -> EN upper */
        for (size_t j = 0; j < upper_len; j++) {
            if (c == k_ru_upper[j]) { text[i] = k_en_upper[j]; goto next; }
        }
        /* No match — leave character unchanged */
        next:;
    }
}
