#include <regex.h>
#include "utils.h"
#include "log.h"


int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    int ret;
    /* Compile regular expression */
    reti = regcomp(&regex, expr, REG_ICASE | REG_EXTENDED);
    if (reti)
    {
        //ERROR("Could not compile regex: %s",expr);
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        ERROR("Regex match failed: %s", msgbuf);
        //return 0;
    }

    /* Execute regular expression */
    reti = regexec(&regex, search, msize, matches, 0);
    if (!reti)
    {
        //LOG("Match");
        ret = 1;
    }
    else if (reti == REG_NOMATCH)
    {
        //LOG("No match");
        ret = 0;
    }
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        //ERROR("Regex match failed: %s\n", msgbuf);
        ret = 0;
    }

    regfree(&regex);
    return ret;
}

void trim(char* str, const char delim)
{
    if(!str || strlen(str) == 0) return;
    char * p = str;
    int l = strlen(p);
    while(l > 0 && p[l - 1] == delim)
        p[--l] = 0;
    while(* p && (* p) == delim ) ++p, --l;
    memmove(str, p, l + 1);
}