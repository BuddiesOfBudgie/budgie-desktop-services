#pragma once

#include <QMap>
#include <QVariant>

namespace bd::Outputs {
  typedef QMap<QString, QVariant> KvMap;
  typedef QMap<QString, KvMap>    NestedKvMap;
}
