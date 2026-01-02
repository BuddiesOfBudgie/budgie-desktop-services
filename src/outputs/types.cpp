#include "types.hpp"

#include <QMetaType>

QDBusArgument& operator<<(QDBusArgument& argument, const bd::Outputs::OutputModeInfo& modeInfo) {
  argument.beginStructure();
  argument << modeInfo.id << modeInfo.width << modeInfo.height << modeInfo.refreshRate << modeInfo.preferred;
  argument.endStructure();
  return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, bd::Outputs::OutputModeInfo& modeInfo) {
  argument.beginStructure();
  argument >> modeInfo.id >> modeInfo.width >> modeInfo.height >> modeInfo.refreshRate >> modeInfo.preferred;
  argument.endStructure();
  return argument;
}

QDBusArgument& operator<<(QDBusArgument& argument, const bd::Outputs::OutputModesMap& modesMap) {
  argument.beginMap(QMetaType::fromType<QString>().id(), QMetaType::fromType<bd::Outputs::OutputModeInfo>().id());
  for (auto it = modesMap.constBegin(); it != modesMap.constEnd(); ++it) {
    argument.beginMapEntry();
    argument << it.key() << it.value();
    argument.endMapEntry();
  }
  argument.endMap();
  return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, bd::Outputs::OutputModesMap& modesMap) {
  modesMap.clear();
  argument.beginMap();
  while (!argument.atEnd()) {
    argument.beginMapEntry();
    QString                     key;
    bd::Outputs::OutputModeInfo value;
    argument >> key >> value;
    modesMap.insert(key, value);
    argument.endMapEntry();
  }
  argument.endMap();
  return argument;
}
