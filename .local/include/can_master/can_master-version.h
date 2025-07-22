/*
 * Copyright © 2016-2025 AMBITECS <info@ambi.biz>
 */
#ifndef CAN_MASTER_VERSION_H
#define CAN_MASTER_VERSION_H

/* The major version (1, if %LIBCAN_MASTER_VERSION is 1.2.3) */
#define LIBCAN_MASTER_VERSION_MAJOR (1)

/* The minor version (2, if %LIBCAN_MASTER_VERSION is 1.2.3) */
#define LIBCAN_MASTER_VERSION_MINOR (0)

/* The micro version (3, if %LIBCAN_MASTER_VERSION is 1.2.3) */
#define LIBCAN_MASTER_VERSION_MICRO (1)

/* The full version, like 1.2.3 */
#define LIBCAN_MASTER_VERSION        1.0.1

/* The full version, in string form (suited for string concatenation)
 */
#define LIBCAN_MASTER_VERSION_STRING "1.0.1"

/* Numerically encoded version, eg. v1.2.3 is 0x010203 */
#define LIBCAN_MASTER_VERSION_HEX ((LIBCAN_MASTER_VERSION_MAJOR << 16) |  \
                               (LIBCAN_MASTER_VERSION_MINOR <<  8) |  \
                               (LIBCAN_MASTER_VERSION_MICRO <<  0))

/* Evaluates to True if the version is greater than @major, @minor and @micro
 */
#define LIBCAN_MASTER_VERSION_CHECK(major,minor,micro)     \
   ( LIBCAN_MASTER_VERSION_MAJOR >  (major) ||             \
     (LIBCAN_MASTER_VERSION_MAJOR == (major) &&            \
      LIBCAN_MASTER_VERSION_MINOR >  (minor)) ||           \

     (LIBCAN_MASTER_VERSION_MAJOR == (major) &&            \
      LIBCAN_MASTER_VERSION_MINOR == (minor) &&            \
      LIBCAN_MASTER_VERSION_MICRO >= (micro)) )

#endif /* CAN_MASTER_VERSION_H */
