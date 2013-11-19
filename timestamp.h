#ifndef TIMESTAMP_H_SENTRY
#define TIMESTAMP_H_SENTRY

typedef unsigned int uint;

/* Why 16? You can low reduce it. See format in comment
 * for 'update_time_buf' func. */
#define TIME_BUFFER_SIZE 16

/* Get current (local) time and place it to time_buf
 * in format "[%H:%M:%S]". On fail place "\0" to strbuf
 * (if his size not zero). Mininum buffer size for successful
 * updating time buffer defined by TIME_BUFFER_SIZE macro.
 * Returns: 1 on success (time updated), 0 otherwise. */
uint update_time_buf(char *time_buf, uint buf_size);

#endif /* TIMESTAMP_H_SENTRY */
