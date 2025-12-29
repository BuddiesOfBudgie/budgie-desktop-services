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
    Q_PROPERTY(uint AdaptiveSync READ AdaptiveSync NOTIFY adaptiveSyncChanged)
    Q_PROPERTY(QVariantMap CurrentMode READ CurrentMode NOTIFY currentModeChanged)
    Q_PROPERTY(QString Description READ Description NOTIFY descriptionChanged)
    Q_PROPERTY(bool Enabled READ Enabled NOTIFY enabledChanged)
    Q_PROPERTY(int Height READ Height NOTIFY heightChanged)
    Q_PROPERTY(QString HorizontalAnchor READ HorizontalAnchor NOTIFY horizontalAnchorChanged)
    Q_PROPERTY(QString Make READ Make NOTIFY makeChanged)
    Q_PROPERTY(NestedKvMap Modes READ Modes NOTIFY modesChanged)
    Q_PROPERTY(QString MirrorOf READ MirrorOf NOTIFY mirrorOfChanged)
    Q_PROPERTY(QString Model READ Model NOTIFY modelChanged)
    Q_PROPERTY(QString Name READ Name NOTIFY nameChanged)
    Q_PROPERTY(bool Primary READ Primary NOTIFY primaryChanged)
    Q_PROPERTY(qulonglong RefreshRate READ RefreshRate NOTIFY refreshRateChanged)
    Q_PROPERTY(QString RelativeTo READ RelativeTo NOTIFY relativeToChanged)
    Q_PROPERTY(double Scale READ Scale NOTIFY scaleChanged)
    Q_PROPERTY(QString Serial READ Serial NOTIFY serialChanged)
    Q_PROPERTY(quint8 Transform READ Transform NOTIFY transformChanged)
    Q_PROPERTY(QString VerticalAnchor READ VerticalAnchor NOTIFY verticalAnchorChanged)
    Q_PROPERTY(int Width READ Width NOTIFY widthChanged)
    Q_PROPERTY(int X READ X NOTIFY xChanged)
    Q_PROPERTY(int Y READ Y NOTIFY yChanged)

    public:
        MetaHead(QObject *parent, KWayland::Client::Registry *registry);

        ~MetaHead() override;

        QtWayland::zwlr_output_head_v1::adaptive_sync_state getAdaptiveSync();

        QSharedPointer<bd::Outputs::Wlr::MetaMode> getCurrentMode();

        QSharedPointer<bd::Outputs::Wlr::Head> getHead();

        QSharedPointer<bd::Outputs::Wlr::MetaMode> getModeForOutputHead(int width, int height, qulonglong refresh);

        QList<QSharedPointer<bd::Outputs::Wlr::MetaMode>> getModes();

        uint AdaptiveSync() const;
        QVariantMap CurrentMode() const;
        QString Description() const;
        bool Enabled() const;
        int Height() const;
        QString HorizontalAnchor() const;
        QString Make() const;
        QString MirrorOf() const;
        NestedKvMap Modes() const;
        QString Model() const;
        QString Name() const;
        bool Primary() const;
        qulonglong RefreshRate() const;
        QString RelativeTo() const;
        double Scale() const;
        QString Serial() const;
        quint8 Transform() const;
        int Width() const;
        int X() const;
        int Y() const;
        QString VerticalAnchor() const;

        // Internal getters (used by Q_PROPERTY getters or for special return types)
        QString getIdentifier(); // Used by Serial() Q_PROPERTY getter
        QPoint getPosition(); // Returns QPoint (X()/Y() return int)

        bd::Outputs::Config::HorizontalAnchor::Type getHorizontalAnchor() const; // Returns Type (HorizontalAnchor() returns QString)
        bd::Outputs::Config::VerticalAnchor::Type getVerticalAnchor() const; // Returns Type (VerticalAnchor() returns QString)

        std::optional<::zwlr_output_head_v1*> getWlrHead();

        bool isAvailable();
        bool isBuiltIn();

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
        void currentModeChanged(const QVariantMap &currentMode);
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
        void transformChanged(quint8 transform);
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
