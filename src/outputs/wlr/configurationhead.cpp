#include "configurationhead.hpp"

namespace bd::Outputs::Wlr {
    ConfigurationHead::ConfigurationHead(
        bd::Outputs::Wlr::MetaHead*          head,
        ::zwlr_output_configuration_head_v1* wlr_head,
        QObject*                             parent)
        : QObject(parent), zwlr_output_configuration_head_v1(wlr_head), m_head(head) {}
  
    bd::Outputs::Wlr::MetaHead* ConfigurationHead::getHead() {
      return m_head;
    }
  
    void ConfigurationHead::release() {
      // TODO: change from being a no-op for now
    }
  
    void ConfigurationHead::setAdaptiveSync(uint32_t state) {
      set_adaptive_sync(state);
    }
  
    void ConfigurationHead::setMode(bd::Outputs::Wlr::MetaMode* mode) {
      auto wlrModeOpt = mode->getWlrMode();
      if (wlrModeOpt == nullptr || (wlrModeOpt != nullptr && !wlrModeOpt.has_value())) {
        qWarning() << "Tried to set mode on configuration head, but mode is not available";
        return;
      }
      set_mode(const_cast<::zwlr_output_mode_v1*>(wlrModeOpt.value()));
    }
  
    void ConfigurationHead::setCustomMode(signed int width, signed int height, qulonglong refresh) {
      set_custom_mode(width, height, static_cast<int32_t>(refresh));
    }
  
    void ConfigurationHead::setPosition(int32_t x, int32_t y) {
      set_position(x, y);
    }
  
    void ConfigurationHead::setScale(double scale) {
      set_scale(wl_fixed_from_double(scale));
    }
  
    void ConfigurationHead::setTransform(quint8 transform) {
      set_transform(static_cast<int32_t>(transform));
    }
}