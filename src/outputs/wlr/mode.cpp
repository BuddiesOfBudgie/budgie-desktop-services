#include "mode.hpp"

namespace bd::Outputs::Wlr {
  Mode::Mode(::zwlr_output_mode_v1* mode) : zwlr_output_mode_v1(mode) {}

  std::optional<::zwlr_output_mode_v1*> Mode::getWlrMode() {
      if (isInitialized() && object()) {
          return std::make_optional(object());
      }
      return std::nullopt;
  }

  void Mode::zwlr_output_mode_v1_size(int32_t width, int32_t height) {
    qDebug() << "Mode size changed to: " << width << "x" << height;
    emit propertyChanged(MetaModeProperty::Property::Size, QVariant {QSize(width, height)});
  }

  void Mode::zwlr_output_mode_v1_refresh(int32_t refresh) {
    qDebug() << "Mode refresh changed to: " << refresh;
    auto val = QVariant::fromValue(refresh);
    emit propertyChanged(MetaModeProperty::Property::Refresh, val);
  }

  void Mode::zwlr_output_mode_v1_preferred() {
    emit propertyChanged(MetaModeProperty::Property::Preferred, QVariant::fromValue(true));
  }
}
