#include <QVariantMap>
#include <QVariant>

#include "result.hpp"

namespace bd::Outputs::Config {
    Result::Result(QObject *parent) : QObject(parent),
        m_global_space(QSharedPointer<QRect>(new QRect(0, 0, 0, 0))),
        m_output_states(QMap<QString, QSharedPointer<TargetState>>()) {
    }

    QSharedPointer<QRect> Result::getGlobalSpace() const {
        return m_global_space;
    }

    QMap<QString, QSharedPointer<TargetState>> Result::getOutputStates() const {
        return m_output_states;
    }

    void Result::setOutputState(QString serial, QSharedPointer<TargetState> output_state) {
        m_output_states.insert(serial, output_state);
    }

    QVariantMap Result::toVariantMap() const {
        QVariantMap map;
        // Serialize globalSpace
        if (m_global_space) {
            QVariantMap gs;
            gs["x"] = m_global_space->x();
            gs["y"] = m_global_space->y();
            gs["width"] = m_global_space->width();
            gs["height"] = m_global_space->height();
            map["globalSpace"] = gs;
        }
        // Serialize outputs
        QVariantMap outputs;
        for (auto it = m_output_states.begin(); it != m_output_states.end(); ++it) {
            QVariantMap out;
            auto state = it.value();
            out["on"] = state->isOn();
            out["dimensions"] = QVariant::fromValue(state->getDimensions());
            out["refresh"] = state->getRefresh();
            out["horizontalAnchor"] = bd::Outputs::Config::HorizontalAnchor::toString(state->getHorizontalAnchor());
            out["verticalAnchor"] = bd::Outputs::Config::VerticalAnchor::toString(state->getVerticalAnchor());
            out["position"] = QVariant::fromValue(state->getPosition());
            out["primary"] = state->isPrimary();
            out["scale"] = state->getScale();
            out["transform"] = state->getTransform();
            out["resultingDimensions"] = QVariant::fromValue(state->getResultingDimensions());
            out["adaptiveSync"] = state->getAdaptiveSync();
            outputs[it.key()] = out;
        }
        map["outputs"] = outputs;
        return map;
    }
}