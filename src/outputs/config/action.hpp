#pragma once

#include <QObject>
#include <QPoint>
#include <QSize>
#include <QSharedPointer>

#include "enums/actiontype.hpp"
#include "enums/anchors.hpp"

namespace bd::Outputs::Config {

    class Action : public QObject {
        Q_OBJECT

    public:
        static QSharedPointer<Action> explicitOn(const QString& serial, QObject *parent = nullptr);
        static QSharedPointer<Action> explicitOff(const QString& serial, QObject *parent = nullptr);

        static QSharedPointer<Action> mirrorOf(const QString& serial, QString relative, QObject *parent = nullptr);

        static QSharedPointer<Action> mode(const QString& serial, QSize dimensions, qulonglong refresh,
                                             QObject *parent = nullptr);

        static QSharedPointer<Action> scale(const QString& serial, qreal scale,  QObject *parent = nullptr);

        static QSharedPointer<Action> transform(const QString& serial, quint16 transform, QObject *parent = nullptr);

        static QSharedPointer<Action> adaptiveSync(const QString& serial, uint32_t adaptiveSync, QObject *parent = nullptr);

        static QSharedPointer<Action> primary(const QString& serial, QObject *parent = nullptr);

        static QSharedPointer<Action> absolutePosition(const QString& serial, QPoint position, QObject *parent = nullptr);

        static QSharedPointer<Action> positionAnchor(const QString& serial, QString relative, HorizontalAnchor::Type horizontal,
            VerticalAnchor::Type vertical, QObject *parent = nullptr);

        ~Action() = default;

        ActionType::Type getActionType() const;
        QString getSerial() const;
        bool isOn() const;
        bool isPrimary() const;
        QString getRelative() const;
        QSize getDimensions() const;
        QPoint getAbsolutePosition() const;
        qulonglong getRefresh() const;
        HorizontalAnchor::Type getHorizontalAnchor() const;
        VerticalAnchor::Type getVerticalAnchor() const;
        qreal getScale() const;
        quint16 getTransform() const;
        uint32_t getAdaptiveSync() const;

    protected:
        explicit Action(ActionType::Type action_type, QString serial,
                                     QObject *parent = nullptr);

    private:
        ActionType::Type m_action_type;
        QString m_serial;

        // Explicit On/Off (otherwise uses whatever current state of WlrOutputMetaHead is)
        bool m_on;

        // Shared by mirrorOf and setAnchorTo
        QString m_relative;

        // Mode
        QSize m_dimensions;
        qulonglong m_refresh;

        // Position Anchor (relative)
        HorizontalAnchor::Type m_horizontal_anchor;
        VerticalAnchor::Type m_vertical_anchor;

        // Position (absolute)
        QPoint m_absolute_position;

        // Scale
        qreal m_scale;

        // Transform
        quint16 m_transform;

        // Adaptive Sync
        uint32_t m_adaptive_sync;

        // Primary
        bool m_primary;
    };

} // bd