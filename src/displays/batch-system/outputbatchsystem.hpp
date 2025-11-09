#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QMap>
#include <QList>
#include "configurationaction.hpp"
#include "result.hpp"

namespace bd::BatchSystem {
    class OutputBatchSystem : public QObject {
    Q_OBJECT

    public:
        OutputBatchSystem(QObject* parent = nullptr);
        static OutputBatchSystem& instance();
        static OutputBatchSystem* create() { return &instance(); }

        void addAction(QSharedPointer<ConfigurationAction> action);
        void removeAction(QString serial, ConfigurationActionType::Type action_type);

        // Performs a calculation if necessary and applies them
        void apply();

        // Calculate potential resulting state from all actions
        // This does not apply the actions.
        void calculate();

        QSharedPointer<Result> getCalculationResult() const;
        QList<QSharedPointer<ConfigurationAction>> getActions() const;

        // Clears any actions, resets any state
        void reset();

    signals:
        void configurationApplied(bool success);

    private:
        QSharedPointer<Result> m_calculation_result;
        QList<QSharedPointer<ConfigurationAction>> m_actions;

        // Helper method for calculating anchored positions
        QPoint calculateAnchoredPosition(QSharedPointer<TargetState> outputState, QSharedPointer<TargetState> relativeState);

        // Helper to build the horizontal chain for output positioning
        QList<QString> buildHorizontalChain(const QMap<QString, QSharedPointer<TargetState>>& pendingOutputStates, const QList<QSharedPointer<ConfigurationAction>>& actions);
    };
}