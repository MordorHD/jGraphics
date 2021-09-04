#include "jgtext.h"

#include <string.h>

char *JGGetString(JGTEXT *text, char *restrict buf, int bufLen)
{
    if(buf == NULL || bufLen == 0)
        return NULL;
    int len = bufLen <= text->len ? bufLen - 1 : text->len;
    memcpy(buf, text->text, len);
    buf[len] = 0;
    return buf;
}

bool JGSetString(JGTEXT *text, string_t str)
{
    int len;
    if(str && (len = strlen(str)))
    {
        text->len = text->cap = len;
        strcpy(text->text = malloc(len), str);
        return 1;
    }
    text->cap = 12;
    text->len = 0;
    text->text = NULL;
    return 0;
}

bool JGAppendString(JGTEXT *text, string_t apd)
{
    return JGInsertString(text, apd, text->len);
}

bool JGInsertString(JGTEXT *text, string_t ins, int index)
{
    int len;
    if(ins == NULL || (len = strlen(ins)) == 0)
        return 0;
    int textLen = text->len;
    int newLen = textLen + len;
    if(newLen > text->cap)
    {
        text->text = realloc(text->text, len + (text->cap *= 2));
        text->len = newLen;
    }
    memmove(text->text + index + len, text->text + index, textLen - index);
    memcpy(text->text + index, ins, len);
    return 1;
}

bool JGRemoveCharAt(JGTEXT *text, int index)
{
    return JGRemoveRangeAt(text, index, 1);
}

bool JGRemoveRangeAt(JGTEXT *text, int fromIndex, int removeLen)
{
    if(removeLen == 0)
        return 0;
    memmove(text->text + fromIndex, text->text + fromIndex + removeLen, text->len - fromIndex - removeLen);
    text->len -= removeLen;
    return 1;
}

int JGFindChar(JGTEXT *text, char ch, int fromIndex)
{
    char *str = text->text + fromIndex;
    int len = text->len - fromIndex;
    while(len--)
        if(*(str++) == ch)
            return text->len - len - 1;
    return -1;
}

static inline int JGFindString0(char *restrict str, int len, string_t restrict find, int findLen, int fromIndex)
{
    int sLen = len;
    int matchCnt = 0;
    while(len--)
    {
        if(*(find + matchCnt) == *str)
        {
            matchCnt++;
            if(matchCnt == findLen)
                return sLen - len - matchCnt;
        }
        else if(matchCnt != 0)
        {
            if(*find == *str)
                matchCnt = 1;
            else
                matchCnt = 0;
        }
        str++;
    }
    return -1;
}

int JGFindString(JGTEXT *text, string_t restrict find, int fromIndex)
{
    int fLen;
    if(find == NULL || (fLen = strlen(find)) == 0)
        return -1;
    return JGFindString0(text->text, text->len, find, fLen, fromIndex);
}

int JGReplaceString(JGTEXT *text, string_t restrict find, int fromIndex, string_t restrict replace)
{
    int fLen;
    if(find == NULL || (fLen = strlen(find)) == 0)
        return -1;
    int index;
    if((index = JGFindString0(text->text, text->len, find, fLen, fromIndex)) < 0)
        return -1;
    int rLen = strlen(replace);
    int aLen = fLen - rLen;
    memmove(text->text + index, text->text + index + aLen, text->len - index - aLen);
    memcpy(text->text + index, replace, rLen);
    text->len -= aLen;
    return index;
}
