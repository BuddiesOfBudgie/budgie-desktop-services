#include <QFile>
#include <QTextStream>

#include "state.hpp"
#include "outputs/state.hpp"
#include "sys/SysInfo.hpp"
#include "utils.hpp"

namespace bd::Config::Outputs {
    State::State(QObject* parent) : QObject(parent), m_activeGroup(nullptr), m_matchingGroup(nullptr), m_preferences(new GlobalPreferences(this)),
     m_groups(QList<QSharedPointer<Group>>()) {}

    State& State::instance() {
        static State _instance(nullptr);
        return _instance;
    }

    QSharedPointer<Group> State::activeGroup() const {
        return m_activeGroup;
    }

    QList<QSharedPointer<Group>> State::groups() const {
        return m_groups;
    }

    QSharedPointer<Group> State::matchingGroup() const {
        return m_matchingGroup;
    }

    QSharedPointer<GlobalPreferences> State::preferences() const {
        return m_preferences;
    }

    void State::setActiveGroup(QSharedPointer<Group> activeGroup) {
        m_activeGroup = activeGroup;
        emit activeGroupChanged(activeGroup);
    }

    void State::setMatchingGroup(QSharedPointer<Group> matchingGroup) {
        m_matchingGroup = matchingGroup;
        emit matchingGroupChanged(matchingGroup);
    }

    void State::setGroups(const QList<QSharedPointer<Group>>& groups) {
        m_groups = groups;
    }

    void State::apply() {
        auto matching_group = getMatchingGroup();
        // If we don't have a matching group, create a default one and dump its state so we have a default
        if (matching_group.isNull()) {
            qDebug() << "No matching group found, creating a default one";
            m_matchingGroup = createDefaultGroup();
            matching_group = m_matchingGroup;
            m_groups.append(m_matchingGroup);
        }

        // Apply the configuration for the matching group
        matching_group->apply();

        // Set the active group to the matching group
        m_activeGroup = matching_group;
    }

    void State::deserialize() {
        bool isShimMode = SysInfo::instance().isShimMode();
        auto config_location = ConfigUtils::getConfigPath(isShimMode ? "display-config-shim.toml" : "display-config.toml");
        ConfigUtils::ensureConfigPathExists(config_location);

        try {
            auto data = toml::parse(config_location);
            if (data.contains("preferences")) {
                auto position = data.at("preferences").at("automatic_attach_outputs_relative_position");
                if (position.is_string()) {
                  auto pos = std::string_view {position.as_string()};
                  m_preferences->setAutomaticAttachOutputsRelativePosition(
                      Config::Outputs::GlobalPreferences::fromString(std::string(pos)));
                }
            }

            // Iterate over each group and create the Group objects
            for (const auto& group_toml : toml::find<std::vector<toml::value>>(data, "group")) {
                auto group = new bd::Config::Outputs::Group(group_toml);
                m_groups.append(QSharedPointer<Group>(group));
            }

            m_matchingGroup = getMatchingGroup();
        } catch (const std::exception& e) {
            if (QString(e.what()).contains("error opening file")) return;
            qWarning() << "Error deserializing display config: " << e.what();
        }
    }

    void State::save() {
        // Ensure 
        bool isShimMode = SysInfo::instance().isShimMode();
        toml::ordered_value config(toml::ordered_table {});
        config.as_table_fmt().fmt = toml::table_format::multiline;

        toml::ordered_value preferences_table(toml::ordered_table {});
        preferences_table["automatic_attach_outputs_relative_position"] = Config::Outputs::GlobalPreferences::toStringStd(m_preferences->automaticAttachOutputsRelativePosition());

        config["preferences"] = preferences_table;

        toml::ordered_value groups(toml::ordered_array {});
        groups.as_array_fmt().fmt = toml::array_format::array_of_tables;
        for (const auto& group : m_groups) { groups.push_back(group->toToml()); }

        config.as_table().emplace_back("group", groups);

        auto serialized_config = toml::format(config);
        auto config_location = ConfigUtils::getConfigPath(isShimMode ? "display-config-shim.toml" : "display-config.toml");
        auto config_file       = QFile(config_location);
    
        if (config_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
          QTextStream stream(&config_file);
          stream << serialized_config.c_str();
          config_file.close();
        } else {
          qWarning() << "Failed to open display-config.toml for writing";
        }
    }

    QSharedPointer<Group> State::createDefaultGroup() {
        auto &orchestrator = bd::Outputs::State::instance();
        auto manager = orchestrator.getManager();
        if (manager.isNull()) {
            qWarning() << "WaylandOutputManager is not available";
            return QSharedPointer<Group>(nullptr);
        }

        auto heads = manager->getHeads();
        auto group = new Group();

        QStringList names_of_active_outputs;
        QList<QSharedPointer<Output>> output_configs;
        
        // For each existing head in our state, add it to our names and also create a output config for it
        for (const auto& head : heads) {
            if (head->getIdentifier() == nullptr) continue;
            names_of_active_outputs.append(head->getIdentifier());
            auto output_config = new Output();
            output_config->setDisabled(false);
            output_config->setIdentifier(head->getIdentifier());
            output_configs.append(QSharedPointer<Output>(output_config));
        }

        group->setName(names_of_active_outputs.join(", ").append(" (Auto Generated)")); // Set our name to an autogenerated one
        group->setOutputConfigs(output_configs); // Add all of our new output configs
        group->setStoredIdentifiers(names_of_active_outputs); // Add all of our new output identifiers
        group->setStoredPrimaryOutputIdentifier(names_of_active_outputs.first()); // Set our primary output identifier to the first one

        return QSharedPointer<Group>(group);
    }

    QSharedPointer<Group> State::getMatchingGroup() {
        QSharedPointer<Group> matching_group = QSharedPointer<Group>(nullptr);
        auto &orchestrator = bd::Outputs::State::instance();
        auto manager = orchestrator.getManager();
        if (manager.isNull()) {
            qWarning() << "WaylandOutputManager is not available";
            return matching_group;
        }

        // Find any matching group
        auto heads = manager->getHeads();
        auto heads_size = heads.size();

        qDebug() << "Found" << heads_size << "heads";

        auto matching_groups = QList<QSharedPointer<Group>>();
        for (auto group : m_groups) {
            if (group->storedIdentifiers().size() != heads_size) {
                qDebug() << "Group" << group->name() << "has" << group->storedIdentifiers().size() << "identifiers, skipping";
                continue;
            }

            bool match = true;
            for (const auto& qIdentifier : group->storedIdentifiers()) {
                qDebug() << "Checking if head" << qIdentifier << "is in group" << group->name();
                bool found = false;
                for (const auto& head : heads) {
                    if (head->getIdentifier() == qIdentifier) {
                        qDebug() << "Head" << qIdentifier << "found in group" << group->name();
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    match = false;
                    break;
                }
            }

            if (match) {
                qDebug() << "Group" << group->name() << "matches, adding to matching groups";
                matching_groups.append(group);
            }
        }

        if (!matching_groups.isEmpty()) {
            qDebug() << "Found" << matching_groups.size() << "matching groups";
            for (const auto& group : matching_groups) {
                if (group->preferred()) {
                    qDebug() << "Group" << group->name() << "is preferred, setting as matching group";
                    matching_group = group;
                    break;
                }
            }

            if (!matching_group) {
                qDebug() << "No preferred group found, setting first matching group as matching group";
                matching_group = matching_groups.first();
            }
        }
        
        return matching_group;
    }
}