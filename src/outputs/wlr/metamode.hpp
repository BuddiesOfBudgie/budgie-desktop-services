#pragma once

#include <QDBusContext>
#include <QObject>
#include <QSize>
#include <QVariant>
#include <QString>
#include <optional>

#include "mode.hpp"
#include "enums.hpp"

namespace bd::Outputs::Wlr {
    class MetaMode : public QObject, protected QDBusContext {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.buddiesofbudgie.Services.OutputMode")
    Q_PROPERTY(bool Available READ Available)
    Q_PROPERTY(bool Current READ Current)
    Q_PROPERTY(int Height READ Height)
    Q_PROPERTY(QString Id READ Id)
    Q_PROPERTY(bool Preferred READ Preferred)
    Q_PROPERTY(qulonglong RefreshRate READ RefreshRate)
    Q_PROPERTY(int Width READ Width)

    public:
        MetaMode(QObject *parent, ::zwlr_output_mode_v1 *wlr_mode);
        ~MetaMode() override;

        // Property getters
        bool Available() const;
        bool Current() const;
        int Height() const;
        QString Id() const;
        bool Preferred() const;
        qulonglong RefreshRate() const;
        int Width() const;

        QVariantMap toDBusVariantMap() const;

        std::optional<qulonglong> getRefresh();

        std::optional<QSize> getSize();

        std::optional<const ::zwlr_output_mode_v1*> getWlrMode();

        std::optional<bool> isAvailable();

        std::optional<bool> isPreferred();

        bool isSameAs(MetaMode *mode);

        void registerDbusService();

        void setMode(::zwlr_output_mode_v1 *wlr_mode);

        void setPreferred(bool preferred);

        void unsetMode();

    Q_SIGNALS:
        void availabilityChanged(bool available);
        void done();
        void preferredChanged(bool preferred);
        void refreshRateChanged(qulonglong refreshRate);
        void sizeChanged(QSize size);

    public Q_SLOTS:
        void modeDisconnected();

    private Q_SLOTS:
        void setProperty(MetaModeProperty::Property property, const QVariant &value);

    private:
        QSharedPointer<bd::Outputs::Wlr::Mode> m_mode;
        QString m_id;
        QSize m_size;
        qulonglong m_refresh;
        std::optional<bool> m_preferred;
        std::optional<bool> m_is_available;
    };
}
