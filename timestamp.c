#include <stdio.h>
#include <time.h>
#include "timestamp.h"

#define LOCALTIME_ERROR(localtime_value) ((localtime_value) == NULL)
#define STRFTIME_ERROR(strftime_value) ((strftime_value) == 0)

uint update_time_buf(char *time_buf, uint buf_size)
{
    time_t unix_time;
    struct tm *localtime_var;
    int ok = 1;
    int strftime_value;

    if (time_buf == NULL || buf_size == 0) {
        return 0;
    }

    time(&unix_time);
    localtime_var = localtime(&unix_time);

    if (LOCALTIME_ERROR(localtime_var)) {
        perror("localtime()");
        ok = 0;
    }

    if (ok) {
        strftime_value = strftime(time_buf, buf_size,
            "[%H:%M:%S]", localtime_var);
    }

    if (ok && STRFTIME_ERROR(strftime_value)) {
#ifdef DEBUG
        fprintf(stderr, "strftime() failed. See %s:%d.\n",
            __FILE__, __LINE__);
#endif
        ok = 0;
    }

    if (!ok) {
        time_buf[0] = '\0';
    }

    return ok;
}
