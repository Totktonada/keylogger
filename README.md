ABOUT
=====

Educational winapi keylogger. Operates using the most simple and obvious
means.

Features:
* logs user input to `C:\keylog.txt`;
* displays a tray icon with a context menu and a popup hint showing current
  time;
* runs automatically on system boot via Windows Registry key \[1\] (switched
  on/off via context menu);
* periodically sends key logs to an e-mail specified at build-time.

[1] `HKLM/Software/Microsoft/Windows/CurrentVersion/Run`.

BUILDING
========

The project’s Makefile is designed for Unix systems with i686-mingw32
crosscompiler and libcurl installed. Once these requirements are met, just type
`make` to build.

Libcurl has to be installed to `mingw` subdirectory of the project root. I used
curl-7.33.0 with these configure options:

```
./configure CFLAGS='-Os'     \
    --disable-ares           \
    --disable-cookies        \
    --disable-crypto-auth    \
    --disable-curldebug      \
    --disable-debug          \
    --disable-dict           \
    --disable-file           \
    --disable-ftp            \
    --disable-gopher         \
    --disable-http           \
    --disable-imap           \
    --disable-ipv6           \
    --disable-ldap           \
    --disable-ldaps          \
    --disable-libcurl-option \
    --disable-manual         \
    --disable-pop3           \
    --disable-proxy          \
    --disable-rtsp           \
    --disable-telnet         \
    --disable-tftp           \
    --disable-verbose        \
    --enable-shared=no       \
    --enable-static=yes      \
    --without-axtls          \
    --without-ca-bundle      \
    --without-ca-path        \
    --without-cyassl         \
    --without-gnutls         \
    --without-libidn         \
    --without-libmetalink    \
    --without-librtmp        \
    --without-libssh2        \
    --without-nghttp2        \
    --without-nss            \
    --without-polarssl       \
    --without-ssl            \
    --without-winidn         \
    --without-winssl         \
    --without-zlib           \
    --host=i686-mingw32      \
    --prefix=${PATH_TO_KEYLOGGER}/mingw
```

The complete set of steps to build the keylogger from scratch is as follows:

```
cd ${PATH_TO_KEYLOGGER}
mkdir mingw && cd mingw
# Now download curl-7.33.0 from http://curl.haxx.se/download.html
# and extract it to 'mingw/curl-7.33.0' directory.
cd curl-7.33.0
# Substitute '...' with the options given above.
./configure ... --prefix=$(realpath ${PWD}/..)
make && make install
# Curl is now installed.
cd ../..
cp sendmail_config.h{.example,}
edit sendmail_config.h
make
# Now 'clock.exe' is finally here.
# Hooray, build finished. Have fun.
```

// vim: set ft=asciidoc:
