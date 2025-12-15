#pragma once

#include <KWayland/Client/registry.h>

#include <QObject>
#include <QPoint>
#include <QSharedPointer>
#include <optional>

#include "head.hpp"
#include "outputs/wlr/metamode.hpp"
#include "enums.hpp"
#include "outputs/config/enums/anchors.hpp"

namespace bd::Outputs::Wlr {

    class MetaHead : public QObject {
    Q_OBJECT

    public:
        MetaHead(QObject *parent, KWayland::Client::Registry *registry);

        ~MetaHead() override;

        QtWayland::zwlr_output_head_v1::adaptive_sync_state getAdaptiveSync();

        QSharedPointer<bd::Outputs::Wlr::MetaMode> getCurrentMode();

        QString getDescription();

        QSharedPointer<bd::Outputs::Wlr::Head> getHead();

        bd::Outputs::Config::HorizontalAnchor::Type getHorizontalAnchor() const;

        QString getIdentifier();

        QString getMake();

        QString getModel();

        QSharedPointer<bd::Outputs::Wlr::MetaMode> getModeForOutputHead(int width, int height, qulonglong refresh);

        QList<QSharedPointer<bd::Outputs::Wlr::MetaMode>> getModes();

        QString getName();

        QPoint getPosition();

        QString getRelativeOutput();

        double getScale();

        int getTransform();

        bd::Outputs::Config::VerticalAnchor::Type getVerticalAnchor() const;

        std::optional<::zwlr_output_head_v1*> getWlrHead();

        bool isAvailable();
        bool isBuiltIn();
        bool isEnabled();
        bool isPrimary();

        void setHead(::zwlr_output_head_v1 *head);

        void setHorizontalAnchoring(bd::Outputs::Config::HorizontalAnchor::Type horizontal);

        void setPosition(QPoint position);

        void setRelativeOutput(const QString &relative);
        void setPrimary(bool primary);

        void setVerticalAnchoring(bd::Outputs::Config::VerticalAnchor::Type vertical);

        void unsetModes();

    signals:

        void headAvailable();

        void headNoLongerAvailable();

        void propertyChanged(MetaHeadProperty::Property property, const QVariant &value);

    public slots:

        QSharedPointer<bd::Outputs::Wlr::MetaMode> addMode(::zwlr_output_mode_v1 *mode);

        void currentModeChanged(::zwlr_output_mode_v1 *mode);

        void headDisconnected();

        void setProperty(MetaHeadProperty::Property property, const QVariant &value);

    private:
        KWayland::Client::Registry *m_registry;
        QSharedPointer<bd::Outputs::Wlr::Head> m_head;
        QString m_make;
        QString m_model;
        QString m_name;
        QString m_description;
        QString m_identifier;
        QList<QSharedPointer<bd::Outputs::Wlr::MetaMode>> m_output_modes;
        QString m_serial;
        QSharedPointer<bd::Outputs::Wlr::MetaMode> m_current_mode;

        QPoint m_position;
        qint16 m_transform;
        qreal m_scale;

        bool m_is_available;
        bool m_enabled;
        QtWayland::zwlr_output_head_v1::adaptive_sync_state m_adaptive_sync;

        // Non-protocol metadata persisted via config
        QString m_relative_output;
        bd::Outputs::Config::HorizontalAnchor::Type m_horizontal_anchor;
        bd::Outputs::Config::VerticalAnchor::Type m_vertical_anchor;
        bool m_primary;
    };
}
