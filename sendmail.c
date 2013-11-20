#include <stdio.h>
#include <curl/curl.h>
#include "sendmail.h"

/* See sendmail_config.h.example */
#include "sendmail_config.h"

int sendmail(const char *path)
{
    int ok = 0;
    CURL *curl;
    struct curl_slist *recipients = NULL;
    FILE *stream;
 
    curl = curl_easy_init();

    if (!curl)
        return ok;

    stream = fopen(path, "r");

    if (!stream)
        return ok;

    curl_easy_setopt(curl, CURLOPT_URL, SMTP_SERVER);
    curl_easy_setopt(curl, CURLOPT_USERNAME, AUTH_USERNAME);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, AUTH_PASSWORD);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, MAIL_FROM);
    recipients = curl_slist_append(recipients, MAIL_TO);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READDATA, stream);
    ok = curl_easy_perform(curl) == CURLE_OK;
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);

    fclose(stream);

    return ok;
}
