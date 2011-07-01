#ifndef WCONFIG_H
#define WCONFIG_H

// Version defines
#define WT_SERIES 0x3
#define WT_MAJOR  0x1
#define WT_MINOR  0x9

/*! \brief A constant that encodes the library version of %Wt
 *
 * You may use this constant to check for the version of %Wt at build-time.
 */
#define WT_VERSION (((WT_SERIES & 0xff) << 24) | ((WT_MAJOR & 0xff) << 16) | ((WT_MINOR & 0xff) << 8))
#define WT_VERSION_STR "3.1.9"
#define WT_CLASS       "Wt3_1_9"

#define RUNDIR "c:/witty"
#define WT_CONFIG_XML "c:/witty/wt_config.xml"
#define WTHTTP_CONFIGURATION "c:/witty/wthttpd"

#define WT_STATIC
#define WTDBO_STATIC
#define WTDBOPOSTGRES_STATIC
#define WTDBOSQLITE3_STATIC
#define WTHTTP_STATIC
#define WT_EXT_STATIC
#define WT_EXT_STATIC

/* #undef WT_HAS_WRASTERIMAGE */
/* #undef WT_HAS_WPDFIMAGE */

/* #undef WT_NO_BOOST_INTRUSIVE */
/* #undef WT_NO_STD_LOCALE */
/* #undef WT_NO_STD_WSTRING */

#if 0
#ifndef WT_TARGET_JAVA
  #define WT_DEBUG(statement) do { if (Wt::WApplication::instance()->debug()) statement; } while(0)
#else
  #define WT_DEBUG(statement)
#endif
#else
  #define WT_DEBUG(statement)
#endif

#endif

