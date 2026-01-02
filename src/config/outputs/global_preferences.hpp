#pragma once

#include <QObject>
#include <QMetaEnum>
#include <QString>

namespace bd::Config::Outputs {
    class GlobalPreferences : public QObject {
        Q_OBJECT

    public:
        enum DisplayRelativePosition {
            None = 0,
            Left,
            Right,
            Above,
            Below,
        };
        Q_ENUM(DisplayRelativePosition)

        Q_PROPERTY(DisplayRelativePosition automaticAttachOutputsRelativePosition 
                   READ automaticAttachOutputsRelativePosition 
                   WRITE setAutomaticAttachOutputsRelativePosition)

        explicit GlobalPreferences(QObject* parent = nullptr);
        ~GlobalPreferences() = default;

        // Property getter
        DisplayRelativePosition automaticAttachOutputsRelativePosition() const;

        // Property setter
        void setAutomaticAttachOutputsRelativePosition(DisplayRelativePosition position);

        // Convert enum to string (lowercase for compatibility with config files)
        static QString toString(DisplayRelativePosition value);

        // Convert string to enum (handles both lowercase and PascalCase)
        static DisplayRelativePosition fromString(const QString& str);

        // Convert std::string to enum (for compatibility)
        static DisplayRelativePosition fromString(const std::string& str);

        // Convert enum to std::string (for compatibility)
        static std::string toStringStd(DisplayRelativePosition value);

    private:
        DisplayRelativePosition m_automaticAttachOutputsRelativePosition;
    };
}

