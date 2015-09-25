#ifndef READPLY_GLOBAL_H
#define READPLY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(READPLY_LIBRARY)
#  define READPLYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define READPLYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // READPLY_GLOBAL_H
