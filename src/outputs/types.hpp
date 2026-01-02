#pragma once

#include <QDBusArgument>
#include <QMap>
#include <QMetaType>
#include <QString>
#include <QVariant>

namespace bd::Outputs {
  typedef QMap<QString, QVariantMap> NestedKvMap;

  struct OutputModeInfo {
      QString    id;
      int        width;
      int        height;
      qulonglong refreshRate;
      bool       preferred;

      bool operator==(const OutputModeInfo& other) const {
        return id == other.id && width == other.width && height == other.height && refreshRate == other.refreshRate && preferred == other.preferred;
      }
  };

  typedef QMap<QString, OutputModeInfo> OutputModesMap;
}

Q_DECLARE_METATYPE(bd::Outputs::NestedKvMap);
Q_DECLARE_METATYPE(bd::Outputs::OutputModeInfo);
Q_DECLARE_METATYPE(bd::Outputs::OutputModesMap);

QDBusArgument&       operator<<(QDBusArgument& argument, const bd::Outputs::OutputModeInfo& modeInfo);
const QDBusArgument& operator>>(const QDBusArgument& argument, bd::Outputs::OutputModeInfo& modeInfo);

QDBusArgument&       operator<<(QDBusArgument& argument, const bd::Outputs::OutputModesMap& modesMap);
const QDBusArgument& operator>>(const QDBusArgument& argument, bd::Outputs::OutputModesMap& modesMap);
