#include "global_preferences.hpp"

#include <QMetaEnum>

namespace bd::Config::Outputs {
    GlobalPreferences::GlobalPreferences(QObject* parent)
        : QObject(parent)
        , m_automaticAttachOutputsRelativePosition(GlobalPreferences::None)
    {
    }

    GlobalPreferences::DisplayRelativePosition GlobalPreferences::automaticAttachOutputsRelativePosition() const
    {
        return m_automaticAttachOutputsRelativePosition;
    }

    void GlobalPreferences::setAutomaticAttachOutputsRelativePosition(GlobalPreferences::DisplayRelativePosition position)
    {
        m_automaticAttachOutputsRelativePosition = position;
    }

    QString GlobalPreferences::toString(GlobalPreferences::DisplayRelativePosition value)
    {
        switch (value) {
            case GlobalPreferences::Left:
                return QStringLiteral("left");
            case GlobalPreferences::Right:
                return QStringLiteral("right");
            case GlobalPreferences::Above:
                return QStringLiteral("above");
            case GlobalPreferences::Below:
                return QStringLiteral("below");
            case GlobalPreferences::None:
            default:
                return QStringLiteral("none");
        }
    }

    GlobalPreferences::DisplayRelativePosition GlobalPreferences::fromString(const QString& str)
    {
        QString lower = str.toLower();
        if (lower == QLatin1String("left")) return GlobalPreferences::Left;
        if (lower == QLatin1String("right")) return GlobalPreferences::Right;
        if (lower == QLatin1String("above")) return GlobalPreferences::Above;
        if (lower == QLatin1String("below")) return GlobalPreferences::Below;
        if (lower == QLatin1String("none")) return GlobalPreferences::None;

        // Try QMetaEnum lookup for PascalCase (e.g., "Left", "Right")
        QMetaEnum metaEnum = QMetaEnum::fromType<GlobalPreferences::DisplayRelativePosition>();
        bool ok;
        int value = metaEnum.keyToValue(str.toLatin1().constData(), &ok);
        if (ok) {
            return static_cast<GlobalPreferences::DisplayRelativePosition>(value);
        }
        return GlobalPreferences::None;
    }

    GlobalPreferences::DisplayRelativePosition GlobalPreferences::fromString(const std::string& str)
    {
        return fromString(QString::fromStdString(str));
    }

    std::string GlobalPreferences::toStringStd(GlobalPreferences::DisplayRelativePosition value)
    {
        return toString(value).toStdString();
    }
}

