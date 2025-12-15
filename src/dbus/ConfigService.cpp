#include "ConfigService.hpp"

#include <QDBusConnection>

#include "outputs/config/action.hpp"
#include "outputs/config/enums/actiontype.hpp"
#include "outputs/config/model.hpp"
#include "outputs/config/result.hpp"

namespace bd {
  ConfigService::ConfigService(QObject* parent) : QObject(parent) {
    m_adaptor = new ConfigAdaptor(this);
    connect(
        &bd::Outputs::Config::Model::instance(), &bd::Outputs::Config::Model::configurationApplied, this,
        &ConfigService::ConfigurationApplied);
  }

  ConfigService& ConfigService::instance() {
    static ConfigService _instance(nullptr);
    return _instance;
  }

  ConfigAdaptor* ConfigService::GetAdaptor() {
    return m_adaptor;
  }

  void ConfigService::ResetConfiguration() {
    bd::Outputs::Config::Model::instance().reset();
  }

  void ConfigService::SetOutputEnabled(const QString& serial, bool enabled) {
    auto action = enabled ? bd::Outputs::Config::Action::explicitOn(serial) : bd::Outputs::Config::Action::explicitOff(serial);
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputMode(const QString& serial, int width, int height, qulonglong refreshRate) {
    auto action = bd::Outputs::Config::Action::mode(serial, QSize(width, height), refreshRate);
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputPositionAnchor(
      const QString& serial,
      const QString& relativeSerial,
      const QString& horizontalAnchor,
      const QString& verticalAnchor) {
    auto hAnchor = bd::Outputs::Config::HorizontalAnchor::fromString(horizontalAnchor);
    auto vAnchor = bd::Outputs::Config::VerticalAnchor::fromString(verticalAnchor);
    auto action  = bd::Outputs::Config::Action::setPositionAnchor(serial, relativeSerial, hAnchor, vAnchor);
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputScale(const QString& serial, double scale) {
    auto action = bd::Outputs::Config::Action::scale(serial, scale);
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputTransform(const QString& serial, quint8 transform) {
    auto action = bd::Outputs::Config::Action::transform(serial, static_cast<quint8>(transform));
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputAdaptiveSync(const QString& serial, uint adaptiveSync) {
    auto action = bd::Outputs::Config::Action::adaptiveSync(serial, static_cast<uint32_t>(adaptiveSync));
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputPrimary(const QString& serial) {
    auto action = bd::Outputs::Config::Action::primary(serial);
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  void ConfigService::SetOutputMirrorOf(const QString& serial, const QString& mirrorSerial) {
    auto action = bd::Outputs::Config::Action::mirrorOf(serial, mirrorSerial);
    bd::Outputs::Config::Model::instance().addAction(action);
  }

  QVariantMap ConfigService::CalculateConfiguration() {
    bd::Outputs::Config::Model::instance().calculate();
    auto result = bd::Outputs::Config::Model::instance().getCalculationResult();
    if (result) { return result->toVariantMap(); }
    return QVariantMap {};
  }

  bool ConfigService::ApplyConfiguration() {
    bd::Outputs::Config::Model::instance().apply();
    // The result will be emitted via ConfigurationApplied signal
    return true;
  }

  QVariantList ConfigService::GetActions() {
    QVariantList result;
    auto         actions = bd::Outputs::Config::Model::instance().getActions();
    for (const auto& action : actions) {
      QVariantMap map;
      map["type"]   = bd::Outputs::Config::ActionType::toString(action->getActionType());
      map["serial"] = action->getSerial();
      switch (action->getActionType()) {
        case bd::Outputs::Config::ActionType::Type::SetOnOff:
          map["on"] = action->isOn();
          break;
        case bd::Outputs::Config::ActionType::Type::SetMode:
          map["dimensions"] = QVariant::fromValue(action->getDimensions());
          map["refresh"]    = action->getRefresh();
          break;
        case bd::Outputs::Config::ActionType::Type::SetPositionAnchor:
          map["relative"]         = action->getRelative();
          map["horizontalAnchor"] = bd::Outputs::Config::HorizontalAnchor::toString(action->getHorizontalAnchor());
          map["verticalAnchor"]   = bd::Outputs::Config::VerticalAnchor::toString(action->getVerticalAnchor());
          break;
        case bd::Outputs::Config::ActionType::Type::SetScale:
          map["scale"] = action->getScale();
          break;
        case bd::Outputs::Config::ActionType::Type::SetTransform:
          map["transform"] = action->getTransform();
          break;
        case bd::Outputs::Config::ActionType::Type::SetAdaptiveSync:
          map["adaptiveSync"] = action->getAdaptiveSync();
          break;
        case bd::Outputs::Config::ActionType::Type::SetPrimary:
          // No extra fields
          break;
        case bd::Outputs::Config::ActionType::Type::SetMirrorOf:
          map["relative"] = action->getRelative();
          break;
        default:
          break;
      }
      result << map;
    }
    return result;
  }

}  // namespace bd
