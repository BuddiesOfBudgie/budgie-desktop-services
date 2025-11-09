#pragma once

#include <KWayland/Client/registry.h>

#include <QObject>
#include <QPoint>
#include <QSharedPointer>
#include <optional>

#include "head.hpp"
#include "displays/output-manager/wlr/mode/metamode.hpp"
#include "enums.hpp"
#include "displays/batch-system/enums/configurationanchors.hpp"

namespace bd::OutputManager::Wlr {

    class MetaHead : public QObject {
    Q_OBJECT

    public:
        MetaHead(QObject *parent, KWayland::Client::Registry *registry);

        ~MetaHead() override;

        QtWayland::zwlr_output_head_v1::adaptive_sync_state getAdaptiveSync();

        QSharedPointer<bd::OutputManager::Wlr::MetaMode> getCurrentMode();

        QString getDescription();

        QSharedPointer<bd::OutputManager::Wlr::Head> getHead();

        bd::BatchSystem::ConfigurationHorizontalAnchor::Type getHorizontalAnchor() const;

        QString getIdentifier();

        QString getMake();

        QString getModel();

        QSharedPointer<bd::OutputManager::Wlr::MetaMode> getModeForOutputHead(int width, int height, qulonglong refresh);

        QList<QSharedPointer<bd::OutputManager::Wlr::MetaMode>> getModes();

        QString getName();

        QPoint getPosition();

        QString getRelativeOutput();

        double getScale();

        int getTransform();

        bd::BatchSystem::ConfigurationVerticalAnchor::Type getVerticalAnchor() const;

        std::optional<::zwlr_output_head_v1*> getWlrHead();

        bool isAvailable();
        bool isBuiltIn();
        bool isEnabled();
        bool isPrimary();

        void setHead(::zwlr_output_head_v1 *head);

        void setHorizontalAnchoring(bd::BatchSystem::ConfigurationHorizontalAnchor::Type horizontal);

        void setPosition(QPoint position);

        void setRelativeOutput(const QString &relative);
        void setPrimary(bool primary);

        void setVerticalAnchoring(bd::BatchSystem::ConfigurationVerticalAnchor::Type vertical);

        void unsetModes();

    signals:

        void headAvailable();

        void headNoLongerAvailable();

        void propertyChanged(MetaHeadProperty property, const QVariant &value);

    public slots:

        QSharedPointer<bd::OutputManager::Wlr::MetaMode> addMode(::zwlr_output_mode_v1 *mode);

        void currentModeChanged(::zwlr_output_mode_v1 *mode);

        void headDisconnected();

        void setProperty(MetaHeadProperty property, const QVariant &value);

    private:
        KWayland::Client::Registry *m_registry;
        QSharedPointer<bd::OutputManager::Wlr::Head> m_head;
        QString m_make;
        QString m_model;
        QString m_name;
        QString m_description;
        QString m_identifier;
        QList<QSharedPointer<bd::OutputManager::Wlr::MetaMode>> m_output_modes;
        QString m_serial;
        QSharedPointer<bd::OutputManager::Wlr::MetaMode> m_current_mode;

        QPoint m_position;
        qint16 m_transform;
        qreal m_scale;

        bool m_is_available;
        bool m_enabled;
        QtWayland::zwlr_output_head_v1::adaptive_sync_state m_adaptive_sync;

        // Non-protocol metadata persisted via config
        QString m_relative_output;
        bd::BatchSystem::ConfigurationHorizontalAnchor::Type m_horizontal_anchor;
        bd::BatchSystem::ConfigurationVerticalAnchor::Type m_vertical_anchor;
        bool m_primary;
    };
}
