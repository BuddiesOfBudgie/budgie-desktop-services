#pragma once

#include <QObject>
#include <QMetaEnum>
#include <QString>

namespace bd::Outputs::Config {
    class ActionType : public QObject {
        Q_OBJECT

    public:
        enum Type {
            SetAbsolutePosition,
            SetAdaptiveSync,
            SetGamma,
            SetMirrorOf,
            SetMode,
            SetOnOff,
            SetPrimary,
            SetPositionAnchor,
            SetScale,
            SetTransform,
        };
        Q_ENUM(Type)

        // Convert enum to string using QMetaEnum
        static QString toString(Type value) {
            QMetaEnum metaEnum = QMetaEnum::fromType<Type>();
            const char* key = metaEnum.valueToKey(static_cast<int>(value));
            if (key) {
                return QString::fromLatin1(key);
            }
            return QString();
        }

        // Convert string to enum using QMetaEnum
        static Type fromString(const QString& str) {
            QMetaEnum metaEnum = QMetaEnum::fromType<Type>();
            bool ok;
            int value = metaEnum.keyToValue(str.toLatin1().constData(), &ok);
            if (ok) {
                return static_cast<Type>(value);
            }
            return SetOnOff; // Default fallback
        }
    };
}