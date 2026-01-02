#pragma once

#include <QObject>
#include <QSharedPointer>

#include "group.hpp"
#include "global_preferences.hpp"

namespace bd::Config::Outputs {
    class State : public QObject {
        Q_OBJECT
        Q_PROPERTY(QSharedPointer<Group> activeGroup READ activeGroup WRITE setActiveGroup NOTIFY activeGroupChanged)
        Q_PROPERTY(QSharedPointer<Group> matchingGroup READ matchingGroup WRITE setMatchingGroup NOTIFY matchingGroupChanged)
        Q_PROPERTY(QList<QSharedPointer<Group>> groups READ groups)
        Q_PROPERTY(QSharedPointer<GlobalPreferences> preferences READ preferences)

    public:
        State(QObject* parent = nullptr);
        static State& instance();
        static State* create() { return &instance(); }
        ~State() = default;

        // Property getters
        QSharedPointer<Group> activeGroup() const;
        QSharedPointer<Group> matchingGroup() const;
        QSharedPointer<GlobalPreferences> preferences() const;
        QList<QSharedPointer<Group>> groups() const;

        // Property setters
        void setActiveGroup(QSharedPointer<Group> Group);
        void setMatchingGroup(QSharedPointer<Group> MatchingGroup);
        void setGroups(const QList<QSharedPointer<Group>>& Groups);

    public Q_SLOTS:
        void apply();
        void deserialize();
        void save();

    Q_SIGNALS:
        void activeGroupChanged(QSharedPointer<Group> ActiveGroup);
        void matchingGroupChanged(QSharedPointer<Group> MatchingGroup);
        void saved();


    private:
        QSharedPointer<Group> createDefaultGroup();
        QSharedPointer<Group> getMatchingGroup();

        QSharedPointer<GlobalPreferences> m_preferences;
        QSharedPointer<Group> m_activeGroup;
        QSharedPointer<Group> m_matchingGroup;
        QList<QSharedPointer<Group>> m_groups;
    };
}