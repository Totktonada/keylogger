#ifndef AUTORUN_H_SENTRY
#define AUTORUN_H_SENTRY

/* Initialize autorun module (get current executable file path).
 * Returns 1 on success, 0 on failure. */
int autorun_init();

/* Get autorun status.
 * If autorun set, returns 1.
 * Otherwise, returns 0.
 * If existsp != NULL:
 * *existsp set 1, if some path already exists;
 * otherwise *exitsp set to 0. */
int autorun_get(int * existsp);

/* Set autorun status. */
void autorun_set(int enable);

/* Set current executable path, if old path presents. */
void autorun_fix();

#endif /* AUTORUN_H_SENTRY */
