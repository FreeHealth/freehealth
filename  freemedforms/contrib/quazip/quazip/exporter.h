#ifndef H_QUAZIP_EXPORTER
#define H_QUAZIP_EXPORTER

#include <QtCore/qglobal.h>

#if defined(QUAZIP_LIBRARY)
#  define ZIP_EXPORT Q_DECL_EXPORT
#else
#  define ZIP_EXPORT Q_DECL_IMPORT
#endif

#endif // End H_QUAZIP_EXPORTER

