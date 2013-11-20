#ifndef MAIN_H_SENTRY
#define MAIN_H_SENTRY

#define ARRAYSIZE(arr) (sizeof (arr) / sizeof((arr)[0]))

#ifdef DEBUG
#define DEBUG_LOG(str) \
    do { \
        printf("DEBUG: %s\n", str); \
        fflush(stdout); \
    } while(0)
#else
#define DEBUG_LOG(str) do {} while(0)
#endif

#endif /* MAIN_H_SENTRY */
