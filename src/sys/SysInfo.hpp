#pragma once
#include <QObject>

namespace bd {

  class SysInfo : public QObject {
      Q_OBJECT
      Q_PROPERTY(QString machineId READ getMachineId)
      Q_PROPERTY(bool shimMode READ isShimMode)

    public:
      SysInfo(QObject* parent);
      static SysInfo& instance();
      static SysInfo* create() { return &instance(); }

      QString getMachineId();
      bool    isShimMode();

    private:
      QString m_machine_id;
      bool    m_shim_mode;
  };

}  // bd
