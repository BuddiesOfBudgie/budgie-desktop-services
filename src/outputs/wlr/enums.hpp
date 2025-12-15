#pragma once

#include <QObject>
#include <QMetaEnum>
#include <QString>

namespace bd::Outputs::Wlr {
  class MetaHeadProperty : public QObject {
    Q_OBJECT

    public:
      enum Property {
        None, // Invalid property
        AdaptiveSync,
        Description,
        Enabled,
        Make,
        Model,
        Name,
        Position,
        Scale,
        SerialNumber,
        Transform,
      };
      Q_ENUM(Property)

      static QString toString(Property value) {
        QMetaEnum metaEnum = QMetaEnum::fromType<Property>();
        return QString::fromLatin1(metaEnum.valueToKey(static_cast<int>(value)));
      }

      static Property fromString(const QString& str) {
        if (str.isEmpty()) return None;
        QMetaEnum metaEnum = QMetaEnum::fromType<Property>();
        bool ok;
        int value = metaEnum.keyToValue(str.toLatin1().constData(), &ok);
        return ok ? static_cast<Property>(value) : None;
      }

      static Property fromString(const std::string& str) {
        return fromString(QString::fromStdString(str));
      }

      static std::string toStringStd(Property value) {
        return toString(value).toStdString();
      }
  };

  class MetaModeProperty : public QObject {
    Q_OBJECT

    public:
      enum Property {
        None, // Invalid property
        Preferred,
        Refresh,
        Size,
      };
      Q_ENUM(Property)

      static QString toString(Property value) {
        QMetaEnum metaEnum = QMetaEnum::fromType<Property>();
        return QString::fromLatin1(metaEnum.valueToKey(static_cast<int>(value)));
      }

      static Property fromString(const QString& str) {
        QMetaEnum metaEnum = QMetaEnum::fromType<Property>();
        bool ok;
        int value = metaEnum.keyToValue(str.toLatin1().constData(), &ok);
        return ok ? static_cast<Property>(value) : None;
      }

      static Property fromString(const std::string& str) {
        return fromString(QString::fromStdString(str));
      }

      static std::string toStringStd(Property value) {
        return toString(value).toStdString();
      }
  };
}
