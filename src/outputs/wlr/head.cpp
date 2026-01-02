#include "head.hpp"

#include <QPoint>

namespace bd::Outputs::Wlr {
  Head::Head(QObject* parent, ::zwlr_output_head_v1* wlr_head)
      : QObject(parent), zwlr_output_head_v1(wlr_head), m_wlr_head(wlr_head) {}

  ::zwlr_output_head_v1* Head::getWlrHead() {
    return m_wlr_head;
  }

  void Head::zwlr_output_head_v1_name(const QString& name) {
    qDebug() << "Head name changed to: " << name;
    emit propertyChanged(MetaHeadProperty::Property::Name, QVariant {name});
  }

  void Head::zwlr_output_head_v1_description(const QString& description) {
    qDebug() << "Head description changed to: " << description;
    emit propertyChanged(MetaHeadProperty::Property::Description, QVariant {description});
  }

  void Head::zwlr_output_head_v1_make(const QString& make) {
    qDebug() << "Head make changed to: " << make;
    emit propertyChanged(MetaHeadProperty::Property::Make, QVariant {make});
  }

  void Head::zwlr_output_head_v1_model(const QString& model) {
    qDebug() << "Head model changed to: " << model;
    emit propertyChanged(MetaHeadProperty::Property::Model, QVariant {model});
  }

  void Head::zwlr_output_head_v1_mode(::zwlr_output_mode_v1* mode) {
    qDebug() << "Head mode added: " << mode;
    emit modeAdded(mode);
  }

  void Head::zwlr_output_head_v1_enabled(int32_t enabled) {
    qDebug() << "Head enabled state changed to: " << enabled;
    emit propertyChanged(MetaHeadProperty::Property::Enabled, QVariant {enabled});
  }

  void Head::zwlr_output_head_v1_current_mode(::zwlr_output_mode_v1* mode) {
    qDebug() << "Head current mode changed to: " << mode;
    emit modeChanged(mode);
  }

  void Head::zwlr_output_head_v1_finished() {
    qDebug() << "Head finished";
    emit headFinished();
  }

  void Head::zwlr_output_head_v1_position(int32_t x, int32_t y) {
    qDebug() << "Head position changed to: " << x << ", " << y;
    emit propertyChanged(MetaHeadProperty::Property::Position, QVariant {QPoint(x, y)});
  }

  void Head::zwlr_output_head_v1_transform(int32_t transform) {
    qDebug() << "Head transform changed to: " << transform;
    emit propertyChanged(MetaHeadProperty::Property::Transform, QVariant {transform});
  }

  void Head::zwlr_output_head_v1_scale(wl_fixed_t scale) {
    qDebug() << "Head scale changed to: " << wl_fixed_to_double(scale);
    emit propertyChanged(MetaHeadProperty::Property::Scale, QVariant {wl_fixed_to_double(scale)});
  }

  void Head::zwlr_output_head_v1_serial_number(const QString& serial) {
    qDebug() << "Head serial number changed to: " << serial;
    emit propertyChanged(MetaHeadProperty::Property::SerialNumber, QVariant {serial});
  }

  void Head::zwlr_output_head_v1_adaptive_sync(uint32_t state) {
    qDebug() << "Head adaptive sync state changed to: " << state;
    emit propertyChanged(MetaHeadProperty::Property::AdaptiveSync, QVariant {state});
  }
}
