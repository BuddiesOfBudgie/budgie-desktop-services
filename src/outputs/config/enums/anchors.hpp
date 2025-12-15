#pragma once

#include <QObject>
#include <QMetaEnum>
#include <QString>
#include <string>

namespace bd::Outputs::Config {
    class HorizontalAnchor : public QObject {
        Q_OBJECT

    public:
        enum Type {
            None, // No horizontal anchor set
            Left, // Right edge of serial is at the left edge of relative
            Right, // Left edge of serial is at the right edge of relative
            Center, // Center of serial is at the center of relative
        };
        Q_ENUM(Type)

        // Convert enum to string (lowercase for compatibility with config files)
        static QString toString(Type value) {
            switch (value) {
                case Left:
                    return QStringLiteral("left");
                case Right:
                    return QStringLiteral("right");
                case Center:
                    return QStringLiteral("center");
                case None:
                default:
                    return QStringLiteral("none");
            }
        }

        // Convert string to enum (handles both lowercase and PascalCase)
        static Type fromString(const QString& str) {
            if (str.isEmpty()) return None;
            QMetaEnum metaEnum = QMetaEnum::fromType<Type>();
            bool ok;
            int value = metaEnum.keyToValue(str.toLatin1().constData(), &ok);
            return ok ? static_cast<Type>(value) : None;
        }

        // Convert std::string to enum (for compatibility)
        static Type fromString(const std::string& str) {
            return fromString(QString::fromStdString(str));
        }

        // Convert enum to std::string (for compatibility)
        static std::string toStringStd(Type value) {
            return toString(value).toStdString();
        }
    };

    class VerticalAnchor : public QObject {
        Q_OBJECT

    public:
        enum Type {
            None, // No vertical anchor set
            Above, // Bottom edge of serial is at the top edge of relative
            Top, // Top edge of serial is at the top edge of relative
            Middle, // Middle of serial is at the middle of relative
            Bottom, // Bottom edge of serial is at the bottom edge of relative
            Below // Top edge of serial is at the bottom edge of relative
        };
        Q_ENUM(Type)

        // Convert enum to string (lowercase for compatibility with config files)
        static QString toString(Type value) {
            switch (value) {
                case Above:
                    return QStringLiteral("above");
                case Top:
                    return QStringLiteral("top");
                case Middle:
                    return QStringLiteral("middle");
                case Bottom:
                    return QStringLiteral("bottom");
                case Below:
                    return QStringLiteral("below");
                case None:
                default:
                    return QStringLiteral("none");
            }
        }

        // Convert string to enum (handles both lowercase and PascalCase)
        static Type fromString(const QString& str) {
            QString lower = str.toLower();
            if (lower == QLatin1String("above")) return Above;
            if (lower == QLatin1String("top")) return Top;
            if (lower == QLatin1String("middle")) return Middle;
            if (lower == QLatin1String("bottom")) return Bottom;
            if (lower == QLatin1String("below")) return Below;
            if (lower == QLatin1String("none")) return None;
            
            // Try QMetaEnum lookup for PascalCase (e.g., "Above", "Top")
            QMetaEnum metaEnum = QMetaEnum::fromType<Type>();
            bool ok;
            int value = metaEnum.keyToValue(str.toLatin1().constData(), &ok);
            if (ok) {
                return static_cast<Type>(value);
            }
            return None;
        }

        // Convert std::string to enum (for compatibility)
        static Type fromString(const std::string& str) {
            return fromString(QString::fromStdString(str));
        }

        // Convert enum to std::string (for compatibility)
        static std::string toStringStd(Type value) {
            return toString(value).toStdString();
        }
    };
}