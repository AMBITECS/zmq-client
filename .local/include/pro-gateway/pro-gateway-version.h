/*
 * Copyright © 2016-2025 AMBITECS <info@ambi.biz>
 */
#ifndef PRO_GATEWAY_VERSION_H
#define PRO_GATEWAY_VERSION_H

/* The major version (1, if %LIBPRO_GATEWAY_VERSION is 1.2.3) */
#define LIBPRO_GATEWAY_VERSION_MAJOR (1)

/* The minor version (2, if %LIBPRO_GATEWAY_VERSION is 1.2.3) */
#define LIBPRO_GATEWAY_VERSION_MINOR (0)

/* The micro version (3, if %LIBPRO_GATEWAY_VERSION is 1.2.3) */
#define LIBPRO_GATEWAY_VERSION_MICRO (1)

/* The full version, like 1.2.3 */
#define LIBPRO_GATEWAY_VERSION        1.0.1

/* The full version, in string form (suited for string concatenation)
 */
#define LIBPRO_GATEWAY_VERSION_STRING "1.0.1"

/* Numerically encoded version, eg. v1.2.3 is 0x010203 */
#define LIBPRO_GATEWAY_VERSION_HEX ((LIBPRO_GATEWAY_VERSION_MAJOR << 16) |  \
                               (LIBPRO_GATEWAY_VERSION_MINOR <<  8) |  \
                               (LIBPRO_GATEWAY_VERSION_MICRO <<  0))

/* Evaluates to True if the version is greater than @major, @minor and @micro
 */
#define LIBPRO_GATEWAY_VERSION_CHECK(major,minor,micro)     \
   (  LIBPRO_GATEWAY_VERSION_MAJOR >  (major) ||             \
     (LIBPRO_GATEWAY_VERSION_MAJOR == (major) &&            \
      LIBPRO_GATEWAY_VERSION_MINOR >  (minor)) ||           \
     (LIBPRO_GATEWAY_VERSION_MAJOR == (major) &&            \
      LIBPRO_GATEWAY_VERSION_MINOR == (minor) &&            \
      LIBPRO_GATEWAY_VERSION_MICRO >= (micro)) )

#endif /* PRO_GATEWAY_VERSION_H */
