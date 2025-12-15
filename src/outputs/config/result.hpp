#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QRect>
#include "targetstate.hpp"

namespace bd::Outputs::Config {
    class Result : public QObject {
        Q_OBJECT

    public:
        Result(QObject *parent = nullptr);
        ~Result() = default;

        QSharedPointer<QRect> getGlobalSpace() const;
        QMap<QString, QSharedPointer<TargetState>> getOutputStates() const;
        QVariantMap toVariantMap() const;

        void setOutputState(QString serial, QSharedPointer<TargetState> output_state);

    private:
        QSharedPointer<QRect> m_global_space;
        QMap<QString, QSharedPointer<TargetState>> m_output_states;
    };
}