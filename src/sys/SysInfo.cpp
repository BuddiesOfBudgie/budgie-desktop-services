#include "SysInfo.hpp"

#include <QSysInfo>

namespace bd {

  SysInfo::SysInfo(QObject* parent) : QObject(parent) {
    m_machine_id = QString {QSysInfo::machineUniqueId()};
    m_shim_mode  = false;

    QByteArray budgie_session_version = qgetenv("BUDGIE_SESSION_VERSION");
    if (!budgie_session_version.isEmpty() && !budgie_session_version.isNull()) {
      QString version = QString::fromLocal8Bit(budgie_session_version);
      m_shim_mode     = version.startsWith("10.10", Qt::CaseInsensitive);
    }
  }

  SysInfo& SysInfo::instance() {
    static SysInfo _instance(nullptr);
    return _instance;
  }

  bool SysInfo::isShimMode() {
    return m_shim_mode;
  }

  QString SysInfo::getMachineId() {
    return m_machine_id;
  }
}  // bd
