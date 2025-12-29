#pragma once
#include <QMap>
#include <QMetaType>
#include <QVariant>

namespace bd::Outputs {
  typedef QMap<QString, QVariantMap> NestedKvMap;
}

Q_DECLARE_METATYPE(bd::Outputs::NestedKvMap);
