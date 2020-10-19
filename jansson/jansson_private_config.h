

#ifndef JANSSON_PRIVATE_CONFIG_H
#define JANSSON_PRIVATE_CONFIG_H

#define HAVE_STDINT_H 1


#if defined __StratifyOS__ || defined __macosx || defined __linux
#define HAVE_UNISTD_H 1
#define HAVE_GETPID 1
#endif

#endif
