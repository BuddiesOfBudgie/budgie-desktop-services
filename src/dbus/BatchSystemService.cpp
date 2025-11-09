#include "BatchSystemService.hpp"

#include <QDBusConnection>

#include "displays/batch-system/configurationaction.hpp"
#include "displays/batch-system/enums/configurationactiontype.hpp"
#include "displays/batch-system/outputbatchsystem.hpp"
#include "displays/batch-system/result.hpp"

namespace bd {
  BatchSystemService::BatchSystemService(QObject* parent) : QObject(parent) {
    m_adaptor = new BatchSystemAdaptor(this);
    connect(
        &bd::BatchSystem::OutputBatchSystem::instance(), &bd::BatchSystem::OutputBatchSystem::configurationApplied, this,
        &BatchSystemService::ConfigurationApplied);
  }

  BatchSystemService& BatchSystemService::instance() {
    static BatchSystemService _instance(nullptr);
    return _instance;
  }

  BatchSystemAdaptor* BatchSystemService::GetAdaptor() {
    return m_adaptor;
  }

  void BatchSystemService::ResetConfiguration() {
    bd::BatchSystem::OutputBatchSystem::instance().reset();
  }

  void BatchSystemService::SetOutputEnabled(const QString& serial, bool enabled) {
    auto action = enabled ? bd::BatchSystem::ConfigurationAction::explicitOn(serial) : bd::BatchSystem::ConfigurationAction::explicitOff(serial);
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputMode(const QString& serial, int width, int height, qulonglong refreshRate) {
    auto action = bd::BatchSystem::ConfigurationAction::mode(serial, QSize(width, height), refreshRate);
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputPositionAnchor(
      const QString& serial,
      const QString& relativeSerial,
      const QString& horizontalAnchor,
      const QString& verticalAnchor) {
    auto hAnchor = bd::BatchSystem::ConfigurationHorizontalAnchor::fromString(horizontalAnchor);
    auto vAnchor = bd::BatchSystem::ConfigurationVerticalAnchor::fromString(verticalAnchor);
    auto action  = bd::BatchSystem::ConfigurationAction::setPositionAnchor(serial, relativeSerial, hAnchor, vAnchor);
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputScale(const QString& serial, double scale) {
    auto action = bd::BatchSystem::ConfigurationAction::scale(serial, scale);
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputTransform(const QString& serial, quint8 transform) {
    auto action = bd::BatchSystem::ConfigurationAction::transform(serial, static_cast<quint8>(transform));
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputAdaptiveSync(const QString& serial, uint adaptiveSync) {
    auto action = bd::BatchSystem::ConfigurationAction::adaptiveSync(serial, static_cast<uint32_t>(adaptiveSync));
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputPrimary(const QString& serial) {
    auto action = bd::BatchSystem::ConfigurationAction::primary(serial);
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  void BatchSystemService::SetOutputMirrorOf(const QString& serial, const QString& mirrorSerial) {
    auto action = bd::BatchSystem::ConfigurationAction::mirrorOf(serial, mirrorSerial);
    bd::BatchSystem::OutputBatchSystem::instance().addAction(action);
  }

  QVariantMap BatchSystemService::CalculateConfiguration() {
    bd::BatchSystem::OutputBatchSystem::instance().calculate();
    auto result = bd::BatchSystem::OutputBatchSystem::instance().getCalculationResult();
    if (result) { return result->toVariantMap(); }
    return QVariantMap {};
  }

  bool BatchSystemService::ApplyConfiguration() {
    bd::BatchSystem::OutputBatchSystem::instance().apply();
    // The result will be emitted via ConfigurationApplied signal
    return true;
  }

  QVariantList BatchSystemService::GetActions() {
    QVariantList result;
    auto         actions = bd::BatchSystem::OutputBatchSystem::instance().getActions();
    for (const auto& action : actions) {
      QVariantMap map;
      map["type"]   = bd::BatchSystem::ConfigurationActionType::toString(action->getActionType());
      map["serial"] = action->getSerial();
      switch (action->getActionType()) {
        case bd::BatchSystem::ConfigurationActionType::Type::SetOnOff:
          map["on"] = action->isOn();
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetMode:
          map["dimensions"] = QVariant::fromValue(action->getDimensions());
          map["refresh"]    = action->getRefresh();
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetPositionAnchor:
          map["relative"]         = action->getRelative();
          map["horizontalAnchor"] = bd::BatchSystem::ConfigurationHorizontalAnchor::toString(action->getHorizontalAnchor());
          map["verticalAnchor"]   = bd::BatchSystem::ConfigurationVerticalAnchor::toString(action->getVerticalAnchor());
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetScale:
          map["scale"] = action->getScale();
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetTransform:
          map["transform"] = action->getTransform();
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetAdaptiveSync:
          map["adaptiveSync"] = action->getAdaptiveSync();
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetPrimary:
          // No extra fields
          break;
        case bd::BatchSystem::ConfigurationActionType::Type::SetMirrorOf:
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
