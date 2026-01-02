#include "configuration.hpp"
#include "outputs/state.hpp"

namespace bd::Outputs::Wlr {
    Configuration::Configuration(QObject* parent, ::zwlr_output_configuration_v1* config)
    : QObject(parent), zwlr_output_configuration_v1(config) {}

    QSharedPointer<ConfigurationHead> Configuration::enable(bd::Outputs::Wlr::MetaHead* head) {
        auto wlrHeadOpt = head->getWlrHead();
        if (!wlrHeadOpt.has_value()) {
            qWarning() << "Tried to enable head, but wlr_head is not available";
            return nullptr;
        }
        auto zwlr_config_head = enable_head(wlrHeadOpt.value());
        auto config_head      = new ConfigurationHead(head, zwlr_config_head);
        return QSharedPointer<ConfigurationHead>(config_head);
    }

    void Configuration::applySelf() {
        apply();
        wl_display_roundtrip(bd::Outputs::State::instance().getDisplay());
    }

    void Configuration::release() {
        destroy();
    }

    void Configuration::disable(bd::Outputs::Wlr::MetaHead* head) {
        auto wlrHeadOpt = head->getWlrHead();
        if (!wlrHeadOpt.has_value()) {
            qWarning() << "Tried to disable head, but wlr_head is not available";
            return;
        }
        disable_head(wlrHeadOpt.value());
    }

    void Configuration::zwlr_output_configuration_v1_succeeded() {
        emit succeeded();
    }

    void Configuration::zwlr_output_configuration_v1_failed() {
        emit failed();
    }

    void Configuration::zwlr_output_configuration_v1_cancelled() {
        emit cancelled();
    }
}