#ifndef KEY_NAMES_H_SENTRY
#define KEY_NAMES_H_SENTRY

/* Write name of key with vk code vk_code to buffer buf.
 * Buffer buf must be minimum 64 bytes of size.
 * Source of names:
 * http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx */
void setKeyName(char * buf, int vk_code);

#endif /* KEY_NAMES_H_SENTRY */
