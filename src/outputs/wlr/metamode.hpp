#pragma once

#include <QObject>
#include <QSize>
#include <QVariant>
#include <QString>
#include <optional>

#include "mode.hpp"
#include "enums.hpp"

namespace bd::Outputs::Wlr {
    class MetaMode : public QObject {
    Q_OBJECT

    public:
        MetaMode(QObject *parent, ::zwlr_output_mode_v1 *wlr_mode);
        ~MetaMode() override;

        QString getId();

        std::optional<qulonglong> getRefresh();

        std::optional<QSize> getSize();

        std::optional<const ::zwlr_output_mode_v1*> getWlrMode();

        std::optional<bool> isAvailable();

        std::optional<bool> isPreferred();

        bool isSameAs(MetaMode *mode);

        void setMode(::zwlr_output_mode_v1 *wlr_mode);

        void setPreferred(bool preferred);

        void unsetMode();

    signals:

        void done();

        void modeNoLongerAvailable();

        void propertyChanged(MetaModeProperty::Property property, const QVariant &value);

    public slots:

        void modeDisconnected();

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
