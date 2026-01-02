#include "action.hpp"
#include <qdebug.h>

namespace bd::Outputs::Config {
    Action::Action(ActionType::Type action_type, QString serial, QObject *parent) : QObject(parent), m_action_type(action_type), m_serial(QString {serial}),
        m_on(false), m_dimensions(QSize()), m_refresh(0), m_horizontal_anchor(HorizontalAnchor::None),
        m_vertical_anchor(VerticalAnchor::None), m_scale(1.0), m_transform(0), m_adaptive_sync(0), m_primary(false) {
    }

    QSharedPointer<Action> Action::explicitOn(const QString& serial, QObject *parent) {
        qDebug() << "Action::explicitOn" << serial;
        auto action = QSharedPointer<Action>(new Action(ActionType::Type::SetOnOff, serial, parent));
        action->m_on = true;
        return action;
    }

    QSharedPointer<Action> Action::explicitOff(const QString& serial, QObject *parent) {
        qDebug() << "Action::explicitOff" << serial;
        return QSharedPointer<Action>(new Action(ActionType::Type::SetOnOff, serial, parent));
    }

    QSharedPointer<Action> Action::mirrorOf(const QString& serial, QString relative, QObject *parent) {
        qDebug() << "Action::mirrorOf" << serial << relative;
        auto action = QSharedPointer<Action>(new Action(ActionType::Type::SetMirrorOf, serial, parent));
        action->m_relative = QString { relative };
        return action;
    }

    QSharedPointer<Action> Action::mode(const QString& serial, QSize dimensions, qulonglong refresh, QObject *parent) {
        qDebug() << "Action::mode" << serial << dimensions << refresh;
        auto action = QSharedPointer<Action>(new Action(ActionType::Type::SetMode, serial, parent));
        action->m_dimensions = QSize {dimensions};
        action->m_refresh = refresh;
        return action;
    }

    QSharedPointer<Action> Action::absolutePosition(const QString& serial, QPoint position, QObject *parent) {
        qDebug() << "Action::setAbsolutePosition" << serial << position;
        auto action = QSharedPointer<Action>(new Action(ActionType::Type::SetAbsolutePosition, serial, parent));
        action->m_absolute_position = position;
        return action;
    }

    QSharedPointer<Action> Action::positionAnchor(const QString& serial, QString relative, bd::Outputs::Config::HorizontalAnchor::Type horizontal,
                                                                                 bd::Outputs::Config::VerticalAnchor::Type vertical, QObject *parent) {
        qDebug() << "Action::positionAnchor" << serial << relative << bd::Outputs::Config::HorizontalAnchor::toString(horizontal) << bd::Outputs::Config::VerticalAnchor::toString(vertical);
        auto action = QSharedPointer<Action>(new Action(ActionType::SetPositionAnchor, serial, parent));
        action->m_relative = QString { relative };
        action->m_horizontal_anchor = horizontal;
        action->m_vertical_anchor = vertical;
        return action;
    }

    QSharedPointer<Action> Action::scale(const QString& serial, qreal scale, QObject *parent) {
        qDebug() << "Action::scale" << serial << scale;
        auto action = QSharedPointer<Action>(new Action(ActionType::SetScale, serial, parent));
        action->m_scale = scale;
        return action;
    }

    QSharedPointer<Action> Action::transform(const QString& serial, quint16 transform, QObject *parent) {
        qDebug() << "Action::transform" << serial << transform;
        auto action = QSharedPointer<Action>(new Action(ActionType::SetTransform, serial, parent));
        action->m_transform = transform;
        return action;
    }

    QSharedPointer<Action> Action::adaptiveSync(const QString& serial, uint32_t adaptiveSync, QObject *parent) {
        qDebug() << "Action::adaptiveSync" << serial << adaptiveSync;
        auto action = QSharedPointer<Action>(new Action(ActionType::SetAdaptiveSync, serial, parent));
        action->m_adaptive_sync = adaptiveSync;
        return action;
    }

    QSharedPointer<Action> Action::primary(const QString& serial, QObject *parent) {
        qDebug() << "Action::primary" << serial;
        auto action = QSharedPointer<Action>(new Action(ActionType::SetPrimary, serial, parent));
        action->m_primary = true;
        return action;
    }

    ActionType::Type Action::getActionType() const {
        return m_action_type;
    }

    QString Action::getSerial() const {
        return m_serial;
    }

    bool Action::isOn() const {
        return m_on;
    }

    bool Action::isPrimary() const {
        return m_primary;
    }

    QString Action::getRelative() const {
        return m_relative;
    }

    QSize Action::getDimensions() const {
        return m_dimensions;
    }

    qulonglong Action::getRefresh() const {
        return m_refresh;
    }

    QPoint Action::getAbsolutePosition() const {
        return m_absolute_position;
    }

    HorizontalAnchor::Type Action::getHorizontalAnchor() const {
        return m_horizontal_anchor;
    }

    VerticalAnchor::Type Action::getVerticalAnchor() const {
        return m_vertical_anchor;
    }

    qreal Action::getScale() const {
        return m_scale;
    }

    quint16 Action::getTransform() const {
        return m_transform;
    }

    uint32_t Action::getAdaptiveSync() const {
        return m_adaptive_sync;
    }
}
