#pragma once

#include <QObject>
#include <QSize>
#include <QSharedPointer>

#include "enums/configurationactiontype.hpp"
#include "enums/configurationanchors.hpp"

namespace bd::BatchSystem {

    class ConfigurationAction : public QObject {
        Q_OBJECT

    public:
        static QSharedPointer<ConfigurationAction> explicitOn(const QString& serial, QObject *parent = nullptr);
        static QSharedPointer<ConfigurationAction> explicitOff(const QString& serial, QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> mirrorOf(const QString& serial, QString relative, QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> mode(const QString& serial, QSize dimensions, qulonglong refresh,
                                             QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> setPositionAnchor(const QString& serial, QString relative, ConfigurationHorizontalAnchor::Type horizontal,
                          ConfigurationVerticalAnchor::Type vertical, QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> scale(const QString& serial, qreal scale,  QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> transform(const QString& serial, quint8 transform, QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> adaptiveSync(const QString& serial, uint32_t adaptiveSync, QObject *parent = nullptr);

        static QSharedPointer<ConfigurationAction> primary(const QString& serial, QObject *parent = nullptr);

        ~ConfigurationAction() override;

        ConfigurationActionType::Type getActionType() const;
        QString getSerial() const;
        bool isOn() const;
        bool isPrimary() const;
        QString getRelative() const;
        QSize getDimensions() const;
        qulonglong getRefresh() const;
        ConfigurationHorizontalAnchor::Type getHorizontalAnchor() const;
        ConfigurationVerticalAnchor::Type getVerticalAnchor() const;
        qreal getScale() const;
        quint8 getTransform() const;
        uint32_t getAdaptiveSync() const;

    protected:
        explicit ConfigurationAction(ConfigurationActionType::Type action_type, QString serial,
                                     QObject *parent = nullptr);

    private:
        ConfigurationActionType::Type m_action_type;
        QString m_serial;

        // Explicit On/Off (otherwise uses whatever current state of WlrOutputMetaHead is)
        bool m_on;

        // Shared by mirrorOf and setAnchorTo
        QString m_relative;

        // Mode
        QSize m_dimensions;
        qulonglong m_refresh;

        // Position Anchor
        ConfigurationHorizontalAnchor::Type m_horizontal_anchor;
        ConfigurationVerticalAnchor::Type m_vertical_anchor;

        // Scale
        qreal m_scale;

        // Transform
        quint8 m_transform;

        // Adaptive Sync
        uint32_t m_adaptive_sync;

        // Primary
        bool m_primary;
    };

} // bd