#pragma once

#include "knn.h"

namespace knn {
    class Command;
    
    template <typename T>
    class CLI {
        std::vector<Command<T>*> m_commands;

        public:
            template <typename... Commands>
            CLI(Commands*... commands) :
                m_commands{ {commands...} } { }

            /**
             * Starts the CLI.
             * @param server        The server connected to.
             * @param exit_name     What to display for the exit option.
             */
            void start(streams::TCPSocket server, std::string exit_name="exit");

            // This class must be public so Command-derived classes can access it.
            struct Settings {
                streams::Serializer serializer;         // Serializer to server
                int k_value;                            // The k value to use in the algorithm
                std::string distance_metric;            // The identifier for the distance metric (EUC, etc)
                std::string train_file;                 // The file to train the database with (unclassified)
                std::string test_file;                  // The file to test the database with (classified)
                bool is_classified;                     // Whether or not the data has been classified already
                std::vector<std::string> true_names;          // A vector of the true class names
                std::vector<std::string> classified_names;    // A vector of the classified names

                Settings() :
                    is_classified(false) { }

                Settings(streams::Serializer server, int k, std::string distance) :
                    serializer(server), k_value(k), distance_metric(distance), is_classified(false) { }

                Settings(streams::TCPSocket& server, int k, std::string distance) :
                    k_value(k), distance_metric(distance), is_classified(false) {
                    this->serializer(&server);
                }
            };
    };

    template <typename T>
    class Command {
        std::string m_description;

        protected:
            T& m_io_device;

        public:
            Command(T& io_device, std::string description) :
                m_io_device(io_device), m_description(description) { }

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

    template <typename T>
    class Upload_Files : public Command<T> {
        public:
            Upload_Files(T& io_device, std::string description) :
                Command(io_device, description) { }

            void execute(CLI::Settings& settings) override;
    };

    template <typename T>
    class Algorithm_Settings : public Command<T> {
        public:
            Algorithm_Settings(T& io_device, std::string description) :
                Command(io_device, description) { }

            void execute(CLI::Settings& settings) override;
    };

    template <typename T>
    class Classify_Data : public Command<T> {
        public:
            Classify_Data(T& io_device, std::string description) :
                Command(io_device, description) { }

            void execute(CLI::Settings& settings) override;
    };

    template <typename T>
    class Display_Results : public Command<T> {
        public:
            Display_Results(T& io_device, std::string description) :
                Command(io_device, description) { }

            void execute(CLI::Settings& settings) override;
    };
    
    template <typename T>
    class Download_Results : public Command<T> {
        public:
            Download_Results(T& io_device, std::string description) :
                Command(io_device, description) { }

            void execute(CLI::Settings& settings) override;
    };
    
    template <typename T>
    class Display_Confusion_Matrix : public Command<T> {
        public:
            Display_Confusion_Matrix(T& io_device, std::string description) :
                Command(io_device, description) { }

            void execute(CLI::Settings& settings) override;
    };
}

