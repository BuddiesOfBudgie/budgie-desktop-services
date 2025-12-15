#pragma once

#include <QObject>
#include <QSharedPointer>
#include "qwayland-wlr-output-management-unstable-v1.h"

#include "configurationhead.hpp"
#include "metahead.hpp"

namespace bd::Outputs::Wlr {
    class Configuration : public QObject, QtWayland::zwlr_output_configuration_v1 {
        Q_OBJECT

    public:
        Configuration(QObject* parent, ::zwlr_output_configuration_v1* config);

        void                                            applySelf();
        QSharedPointer<ConfigurationHead> enable(bd::Outputs::Wlr::MetaHead* head);
        void                                            disable(bd::Outputs::Wlr::MetaHead* head);
        void                                            release();

    signals:
        void succeeded();
        void failed();
        void cancelled();

    protected:
        void zwlr_output_configuration_v1_succeeded() override;
        void zwlr_output_configuration_v1_failed() override;
        void zwlr_output_configuration_v1_cancelled() override;
    };
}