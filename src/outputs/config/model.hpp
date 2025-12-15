#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QList>
#include "action.hpp"
#include "result.hpp"

namespace bd::Outputs::Config {
    class Model : public QObject {
    Q_OBJECT

    public:
        Model(QObject* parent = nullptr);
        static Model& instance();
        static Model* create() { return &instance(); }

        void addAction(QSharedPointer<Action> action);
        void removeAction(QString serial, ActionType::Type action_type);

        // Performs a calculation if necessary and applies them
        void apply();

        // Calculate potential resulting state from all actions
        // This does not apply the actions.
        void calculate();

        QSharedPointer<Result> getCalculationResult() const;
        QList<QSharedPointer<Action>> getActions() const;

        // Clears any actions, resets any state
        void reset();

    signals:
        void configurationApplied(bool success);

    private:
        QSharedPointer<Result> m_calculation_result;
        QList<QSharedPointer<Action>> m_actions;

        // Helper method for calculating anchored positions
        QPoint calculateAnchoredPosition(QSharedPointer<TargetState> outputState, QSharedPointer<TargetState> relativeState);

        // Helper to build the horizontal chain for output positioning
        QList<QString> buildHorizontalChain(const QMap<QString, QSharedPointer<TargetState>>& pendingOutputStates, const QList<QSharedPointer<Action>>& actions);
    };
}