#pragma once

#include <KWayland/Client/registry.h>

#include <QDBusContext>
#include <QObject>
#include <QPoint>
#include <QSharedPointer>
#include <optional>

#include "head.hpp"
#include "outputs/wlr/metamode.hpp"
#include "enums.hpp"
#include "outputs/config/enums/anchors.hpp"
#include "outputs/types.hpp"

namespace bd::Outputs::Wlr {

    class MetaHead : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.buddiesofbudgie.Services.Output")
    Q_PROPERTY(uint adaptiveSync READ adaptiveSync NOTIFY adaptiveSyncChanged)
    Q_PROPERTY(bool builtIn READ builtIn)
    Q_PROPERTY(bd::Outputs::OutputModeInfo currentMode READ currentMode NOTIFY currentModeChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    Q_PROPERTY(QString horizontalAnchor READ horizontalAnchor NOTIFY horizontalAnchorChanged)
    Q_PROPERTY(QString make READ make NOTIFY makeChanged)
    Q_PROPERTY(bd::Outputs::OutputModesMap modes READ modes NOTIFY modesChanged)
    Q_PROPERTY(QString mirrorOf READ mirrorOf NOTIFY mirrorOfChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(bool primary READ primary NOTIFY primaryChanged)
    Q_PROPERTY(qulonglong refreshRate READ refreshRate NOTIFY refreshRateChanged)
    Q_PROPERTY(QString relativeTo READ relativeTo NOTIFY relativeToChanged)
    Q_PROPERTY(double scale READ scale NOTIFY scaleChanged)
    Q_PROPERTY(QString serial READ serial NOTIFY serialChanged)
    Q_PROPERTY(quint16 transform READ transform NOTIFY transformChanged)
    Q_PROPERTY(QString verticalAnchor READ verticalAnchor NOTIFY verticalAnchorChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int x READ x NOTIFY xChanged)
    Q_PROPERTY(int y READ y NOTIFY yChanged)

    public:
        MetaHead(QObject *parent);

        ~MetaHead() override;

        QtWayland::zwlr_output_head_v1::adaptive_sync_state getAdaptiveSync();

        QSharedPointer<bd::Outputs::Wlr::MetaMode> getCurrentMode();

        QSharedPointer<bd::Outputs::Wlr::Head> getHead();

        QSharedPointer<bd::Outputs::Wlr::MetaMode> getModeForOutputHead(int width, int height, qulonglong refresh);

        QList<QSharedPointer<bd::Outputs::Wlr::MetaMode>> getModes();

        uint adaptiveSync() const;
        bool builtIn();
        bd::Outputs::OutputModeInfo currentMode() const;
        QString description() const;
        bool enabled() const;
        int height() const;
        QString horizontalAnchor() const;
        QString make() const;
        QString mirrorOf() const;
        bd::Outputs::OutputModesMap modes() const;
        QString model() const;
        QString name() const;
        bool primary() const;
        qulonglong refreshRate() const;
        QString relativeTo() const;
        double scale() const;
        QString serial() const;
        // Maps to wl_output transform enum. See https://wayland.app/protocols/wayland#wl_output:enum:transform
        quint16 transform() const;
        int width() const;
        int x() const;
        int y() const;
        QString verticalAnchor() const;

        // Internal getters (used by Q_PROPERTY getters or for special return types)
        QString getIdentifier(); // Used by serial() Q_PROPERTY getter
        QPoint getPosition(); // Returns QPoint (x()/y() return int)

        bd::Outputs::Config::HorizontalAnchor::Type getHorizontalAnchor() const; // Returns Type (horizontalAnchor() returns QString)
        bd::Outputs::Config::VerticalAnchor::Type getVerticalAnchor() const; // Returns Type (verticalAnchor() returns QString)

        std::optional<::zwlr_output_head_v1*> getWlrHead();

        bool isAvailable();

        void setHead(::zwlr_output_head_v1 *head);

        void setHorizontalAnchoring(bd::Outputs::Config::HorizontalAnchor::Type horizontal);

        void setPosition(QPoint position);

        void setRelativeOutput(const QString &relative);
        void setPrimary(bool primary);

        void setVerticalAnchoring(bd::Outputs::Config::VerticalAnchor::Type vertical);

        void unsetModes();

        // D-Bus registration
        void registerDbusService();

    Q_SIGNALS:

        void headAvailable();

        void headNoLongerAvailable();
        void stateChanged();

        void adaptiveSyncChanged(uint adaptiveSync);
        void currentModeChanged(const bd::Outputs::OutputModeInfo &currentMode);
        void descriptionChanged(const QString &description);
        void enabledChanged(bool enabled);
        void heightChanged(int height);
        void horizontalAnchorChanged(const QString &horizontalAnchor);
        void makeChanged(const QString &make);
        void mirrorOfChanged(const QString &mirrorOf);
        void modelChanged(const QString &model);
        void modesChanged();
        void nameChanged(const QString &name);
        void positionChanged(const QPoint &position);
        void primaryChanged(bool primary);
        void refreshRateChanged(qulonglong refreshRate);
        void relativeToChanged(const QString &relativeTo);
        void scaleChanged(double scale);
        void serialChanged(const QString &serial);
        void transformChanged(quint16 transform);
        void verticalAnchorChanged(const QString &verticalAnchor);
        void widthChanged(int width);
        void xChanged(int x);
        void yChanged(int y);

    private Q_SLOTS:

        QSharedPointer<bd::Outputs::Wlr::MetaMode> addMode(::zwlr_output_mode_v1 *mode);

        void currentZwlrModeChanged(::zwlr_output_mode_v1 *mode);

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

        bool m_built_in;
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
