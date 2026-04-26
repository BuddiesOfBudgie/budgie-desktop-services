#include "outputmanager.hpp"

#include <KWayland/Client/output.h>

#include <QRect>
#include <QSet>

namespace bd::Outputs::Wlr {
  OutputManager::OutputManager(QObject* parent, KWayland::Client::Registry* registry, uint32_t serial, uint32_t version)
      : QObject(parent),
        zwlr_output_manager_v1(registry->registry(), serial, static_cast<int>(version)),
        m_registry(registry),
        m_serial(serial),
        m_has_serial(true),
        m_version(version) {}

  // Overridden methods from QtWayland::zwlr_output_manager_v1
  void OutputManager::zwlr_output_manager_v1_head(zwlr_output_head_v1* wlr_head) {
    // resolve the identity of the existing heads
    for (const auto& existingHead : m_heads) {
      if (existingHead->getWlrHead().has_value() && existingHead->getWlrHead().value() == wlr_head) {
        qDebug() << "previously known head - updating to use the existing head" << existingHead->getIdentifier();
        existingHead->setHead(wlr_head);
        return;
      }
    }

    // need to create the metahead if not previously known about
    auto head = new bd::Outputs::Wlr::MetaHead(nullptr);
    qInfo() << "OutputManager::zwlr_output_manager_v1_head with id:" << head->getIdentifier() << ", description:" << head->description();

    connect(head, &bd::Outputs::Wlr::MetaHead::headAvailable, this, [this, head]() {
      if (m_inhead) {
        qDebug() << "head processing already in progress, skipping";
        return;
      }
      m_inhead = true;
      qDebug() << "Head available for output: " << head->getIdentifier();
      auto sharedHead = QSharedPointer<bd::Outputs::Wlr::MetaHead>(head);
      m_heads.append(sharedHead);
      emit headAdded(sharedHead);

      m_inhead = false;
    });

    head->setHead(wlr_head);
  }

  void OutputManager::zwlr_output_manager_v1_finished() {
    qInfo() << "OutputManager::zwlr_output_manager_v1_finished";
  }

  void OutputManager::zwlr_output_manager_v1_done(uint32_t serial) {
    qDebug() << "OutputManager::zwlr_output_manager_v1_done with serial:" << serial;
    m_serial     = serial;
    m_has_serial = true;

    emit done();
  }

  QSharedPointer<Configuration> OutputManager::configure() {
    auto wlr_output_configuration = create_configuration(m_serial);
    auto config                   = new Configuration(nullptr, wlr_output_configuration);
    connect(config, &Configuration::cancelled, this, [this, config]() {
      qDebug() << "Configuration cancelled";
      // config->deleteLater();
    });
    connect(config, &Configuration::succeeded, this, [this, config]() {
      qDebug() << "Configuration succeeded";
      // config->deleteLater();
    });
    connect(config, &Configuration::failed, this, [this, config]() {
      qDebug() << "Configuration failed";
      // config->deleteLater();
    });
    return QSharedPointer<Configuration>(config);
  }

  QList<QSharedPointer<bd::Outputs::Wlr::MetaHead>> OutputManager::getHeads() {
    return m_heads;
  }

  QSharedPointer<bd::Outputs::Wlr::MetaHead> OutputManager::getOutputHead(const QString& str) {
    for (auto head : m_heads) {
      if (head->getIdentifier() == str) { return head; }
    }

    return nullptr;
  }

  uint32_t OutputManager::getSerial() {
    return m_serial;
  }

  uint32_t OutputManager::getVersion() {
    return m_version;
  }
}
