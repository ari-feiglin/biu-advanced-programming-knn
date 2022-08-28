#pragma once

#include "knn.h"

namespace knn {
    class Command;

    class CLI {
        std::vector<Command*> m_commands;

        public:
            template <typename... Commands>
            CLI(Commands*... commands) :
                m_commands{ {commands...} } { }

            /**
             * Starts the CLI.
             * @param server        The server connected to.
             * @param exit_name     What to display for the exit option.
             */
            void start(TCPSocket server, std::string exit_name="exit");

            // This class must be public so Command-derived classes can access it.
            struct Settings {
                streams::Serializer serializer;         // Serializer to server
                int k_value;                            // The k value to use in the algorithm
                std::string distance_metric;            // The identifier for the distance metric (EUC, etc)
                std::ofstream train_file;               // The file to train the database with (unclassified)
                std::ofstream test_file;                // The file to test the database with (classified)
                bool classified;                        // Whether or not the data has been classified already
                std::vector<std::string> classified;    // A vector of the classified names

                Settings() :
                    classified(false) { }

                Settings(streams::Serializer server, int k, std::string distance) :
                    serializer(server), k_value(k), distance_metric(distance), classified(false) { }

                Settings(streams::TCPSocket& server, int k, std::string distance) :
                    k_value(k), distance_metric(distance), classified(false) {
                    this->serializer(&server);
                }
            };
    };

    class Command {
        std::string m_description;
        //streams::DefaultIO m_dio; We will ignore this for now.

        public:
            Command(std::string description) :
                m_description(description) { }

            /**
             * Execute the command.
             * @param settings      The current settings.
             */
            virtual void execute(CLI::Settings& settings) =0;

            /**
             * Gets the description of the command.
             */
            std::string get_description() { return this->m_description; }
    };

    class Classify_Data : public Command {
        public:
            Classify_Data(std::string description) :
                Command(description) { }

            void execute(CLI::Settings& settings) override;
    };

    class Display_Confusion_Matrix : public Command {
        public:
            Display_Confusion_Matrix(std::string description) :
                Command(description) { }

            void execute(CLI::Settings& settings) override;
    };
}

