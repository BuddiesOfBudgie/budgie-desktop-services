#include "OutputsService.hpp"

#include <QDBusConnection>

#include "outputs/config/model.hpp"
#include "outputs/state.hpp"

namespace bd {
  OutputsService::OutputsService(QObject* parent) : QObject(parent) {
    if (!QDBusConnection::sessionBus().registerObject(OUTPUTS_SERVICE_PATH, this, QDBusConnection::ExportAllContents)) {
      qCritical() << "Failed to register DBus object at path" << OUTPUTS_SERVICE_PATH;
    }
  }

  QStringList OutputsService::GetAvailableOutputs() {
    auto outputs = QStringList {};
    for (const auto& output : bd::Outputs::State::instance().getManager()->getHeads()) { outputs.append(output->getIdentifier()); }
    return outputs;
  }

  static QSharedPointer<bd::Outputs::Wlr::MetaHead> getPrimaryOrFirstHead() {
    auto manager = bd::Outputs::State::instance().getManager();
    if (!manager) return nullptr;
    const auto heads = manager->getHeads();
    if (heads.isEmpty()) return nullptr;

    for (const auto& head : heads) {
      if (head && head->isPrimary()) return head;
    }
    return heads.first();
  }

  QString OutputsService::GetPrimaryOutput() {
    auto head = getPrimaryOrFirstHead();
    if (!head) return QString();
    return head->getIdentifier();
  }

  QVariantMap OutputsService::GetPrimaryOutputRect() {
    QVariantMap rect;
    auto        head = getPrimaryOrFirstHead();
    if (!head) return rect;

    // Populate QRect-like map similar to GetModeInfo pattern
    int  x    = head->getPosition().x();
    int  y    = head->getPosition().y();
    int  w    = 0;
    int  h    = 0;
    auto mode = head->getCurrentMode();
    if (mode) {
      auto sizeOpt = mode->getSize();
      if (sizeOpt.has_value()) {
        w = sizeOpt->width();
        h = sizeOpt->height();
      }
    }

    rect["X"]      = x;
    rect["Y"]      = y;
    rect["Width"]  = w;
    rect["Height"] = h;
    return rect;
  }

  QVariantMap OutputsService::GetGlobalRect() {
    QVariantMap rect;
    auto        calculationResult = bd::Outputs::Config::Model::instance().getCalculationResult();
    if (!calculationResult) return rect;

    auto globalSpace = calculationResult->getGlobalSpace();
    if (!globalSpace) return rect;

    rect["X"]      = globalSpace->x();
    rect["Y"]      = globalSpace->y();
    rect["Width"]  = globalSpace->width();
    rect["Height"] = globalSpace->height();
    return rect;
  }
}
