#include "group.hpp"
#include "outputs/config/model.hpp"
#include "outputs/state.hpp"
#include "sys/SysInfo.hpp"

namespace bd::Config::Outputs {
    Group::Group(QObject* parent) : QObject(parent),
    m_name(""),
    m_stored_primary_output_identifier(""),
    m_preferred(false) {}

    Group::Group(const toml::value& v, QObject* parent) : QObject(parent) {
        m_name               = QString::fromStdString(toml::find<std::string>(v, "name"));
        QList<QString> output_identifiers;
        for (const auto& serial : toml::find_or<std::vector<std::string>>(v, "identifiers", {})) { output_identifiers.append(QString::fromStdString(serial)); }

        // Toplevel values
        m_stored_identifiers = output_identifiers;
        m_stored_primary_output_identifier = QString::fromStdString(toml::find<std::string>(v, "primary_output"));
        m_preferred          = toml::find_or<bool>(v, "preferred", false);

        // Iterate over the outputs and create the output configs
        auto outputs = toml::find_or<std::vector<toml::value>>(v, "output", {});
        if (outputs.empty()) return;

        for (const toml::value& output : outputs) {
            auto output_config = new Output(output);
            m_output_configs.append(QSharedPointer<Output>(output_config));
        }
    }

    // Property getters

    QString Group::name() const {
        return this->m_name;
    }

    bool Group::preferred() const {
        return this->m_preferred;
    }
    
    QString Group::storedPrimaryOutputIdentifier() const {
        return this->m_stored_primary_output_identifier;
    }

    QStringList Group::storedIdentifiers() const {
        return this->m_stored_identifiers;
    }

    QList<QSharedPointer<Output>> Group::outputConfigs() const {
        return this->m_output_configs;
    }

    // Property setters

    void Group::setName(const QString& name) {
        this->m_name = name;
        emit nameChanged(name);
    }

    void Group::setPreferred(bool preferred) {
        this->m_preferred = preferred;
        emit preferredChanged(preferred);
    }

    void Group::setStoredPrimaryOutputIdentifier(const QString& storedPrimaryOutputIdentifier) {
        this->m_stored_primary_output_identifier = storedPrimaryOutputIdentifier;
        emit storedPrimaryOutputIdentifierChanged(storedPrimaryOutputIdentifier);
    }

    void Group::setStoredIdentifiers(QStringList storedIdentifiers) {
        this->m_stored_identifiers = storedIdentifiers;
        emit storedIdentifiersChanged(storedIdentifiers);
    }

    void Group::setOutputConfigs(const QList<QSharedPointer<Output>>& outputConfigs) {
        this->m_output_configs = outputConfigs;
        emit outputConfigsChanged(outputConfigs);
    }

    // Other methods

    void Group::apply() {
        if (this->m_output_configs.isEmpty()) {
            qWarning() << "No output configs to apply for group:" << this->m_name;
            return;
        }

        auto &orchestrator = bd::Outputs::State::instance();
        auto manager = orchestrator.getManager();
        if (manager.isNull()) {
            qWarning() << "WaylandOutputManager is not available";
            return;
        }

        // Reset the batch system and prepare for new configuration
        auto& batchSystem = bd::Outputs::Config::Model::instance();
        batchSystem.reset();

        // Connect to batch system completion signals
        connect(
            &batchSystem, &bd::Outputs::Config::Model::configurationApplied, this,
            [this, &batchSystem](bool success) {
            if (success) {
                qDebug() << "Display configuration applied successfully via batch system";
            } else {
                qWarning() << "Display configuration failed via batch system";
            }
            // Disconnect to avoid duplicate signals on subsequent uses
            disconnect(&batchSystem, &bd::Outputs::Config::Model::configurationApplied, this, nullptr);
            },
            Qt::SingleShotConnection
        );

        for (const auto& output : this->m_output_configs) {
            auto identifier = output->identifier();
            qDebug() << "Creating batch actions for output:" << identifier;

            if (output->disabled()) {
                // Create action to disable this output
                auto offAction = bd::Outputs::Config::Action::explicitOff(output->identifier());
                batchSystem.addAction(offAction);
                qDebug() << "  - Disable output action created";
                continue; // Skip the rest of the loop for this output
            } else {
                // Create action to enable this output
                auto onAction = bd::Outputs::Config::Action::explicitOn(output->identifier());
                batchSystem.addAction(onAction);
                qDebug() << "  - Enable output action created";
            }

            // Set mode (dimensions and refresh)
            auto modeAction = bd::Outputs::Config::Action::mode(output->identifier(), QSize(output->width(), output->height()), output->refresh());
            batchSystem.addAction(modeAction);
            qDebug() << "  - Set mode action created with mode:" << output->width() << "x" << output->height() << "@" << output->refresh() << "Hz";

            // Set anchoring if specified; also update the meta head so defaults propagate
            if (!SysInfo::instance().isShimMode()) {
                auto relativeOutput = output->relativeOutput();
                if (!relativeOutput.isEmpty()) {
                    auto horizontalAnchor = output->horizontalAnchor();
                    auto verticalAnchor   = output->verticalAnchor();
                    auto anchorAction     = bd::Outputs::Config::Action::positionAnchor(output->identifier(), relativeOutput, horizontalAnchor, verticalAnchor);
                    batchSystem.addAction(anchorAction);
                    qDebug() << "  - Set anchoring relative to:" << relativeOutput << "with horizontal anchor:" << bd::Outputs::Config::HorizontalAnchor::toString(horizontalAnchor) << "and vertical anchor:" << bd::Outputs::Config::VerticalAnchor::toString(verticalAnchor);
                    // Update meta head anchoring
                    auto head = manager->getOutputHead(output->identifier());
                    if (!head.isNull()) {
                        head->setRelativeOutput(relativeOutput);
                        head->setHorizontalAnchoring(horizontalAnchor);
                        head->setVerticalAnchoring(verticalAnchor);
                    }
                } else {
                    // Clear meta head anchoring explicitly
                    auto head = manager->getOutputHead(identifier);
                    if (!head.isNull()) {
                        head->setRelativeOutput("");
                        head->setHorizontalAnchoring(bd::Outputs::Config::HorizontalAnchor::None);
                        head->setVerticalAnchoring(bd::Outputs::Config::VerticalAnchor::None);
                    }
                    qDebug() << "  - No anchoring set";
                }
            } else {
                auto absolutePosition = QPoint(output->x(), output->y());
                auto absolutePositionAction = bd::Outputs::Config::Action::absolutePosition(identifier, absolutePosition);
                batchSystem.addAction(absolutePositionAction);
                qDebug() << "  - Set absolute position:" << output->x() << "," << output->y();
            }

            // Set scale
            auto scaleAction = bd::Outputs::Config::Action::scale(identifier, output->scale());
            batchSystem.addAction(scaleAction);
            qDebug() << "  - Set scale:" << output->scale();

            // Set transform (rotation)
            auto transformAction = bd::Outputs::Config::Action::transform(identifier, output->transform());
            batchSystem.addAction(transformAction);
            qDebug() << "  - Set transform:" << output->transform();

            // Set adaptive sync
            auto adaptiveSyncAction = bd::Outputs::Config::Action::adaptiveSync(identifier, output->adaptiveSync());
            batchSystem.addAction(adaptiveSyncAction);
            qDebug() << "  - Set adaptive sync:" << output->adaptiveSync();
        }

        // Set primary output if specified
        auto primaryOutput = storedPrimaryOutputIdentifier();
        if (!primaryOutput.isEmpty()) {
            qDebug() << "Primary output:" << primaryOutput;
            for (const auto& head : manager->getHeads()) {
                if (head.isNull()) continue;
                head->setPrimary(head->getIdentifier() == primaryOutput);
            }
        }

        // Calculate and apply the configuration
        // batchSystem.calculate();
        batchSystem.apply();
    }

    QSharedPointer<Output> Group::getOutputForIdentifier(const QString& identifier) {
        for (const auto& output : this->m_output_configs) {
            if (output->identifier() == identifier) {
                return output;
            }
        }
        return QSharedPointer<Output>(nullptr);
    }

    toml::ordered_value Group::toToml() {
        toml::ordered_value group_table(toml::ordered_table {});
        group_table.as_table_fmt().fmt = toml::table_format::multiline;

        std::vector<std::string> output_identifiers;
        for (const auto& identifier : m_stored_identifiers) {
            output_identifiers.push_back(identifier.toStdString());
        }

        group_table["name"] = this->m_name.toStdString();
        group_table["preferred"] = this->m_preferred;
        group_table["identifiers"] = output_identifiers;
        group_table["primary_output"] = this->m_stored_primary_output_identifier.toStdString();

        toml::ordered_value outputs(toml::ordered_array {});
        outputs.as_array_fmt().fmt = toml::array_format::array_of_tables;

        for (const auto& output : this->m_output_configs) {
            outputs.push_back(output->toToml());
        }

        group_table.as_table().emplace_back("output", outputs);

        return group_table;
    }
}