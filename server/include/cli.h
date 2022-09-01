#pragma once

#include "knn.h"

namespace knn {
    typedef DataSet<misc::array<double>> dubdset;
    typedef DataPoint<misc::array<double>> dubdpoint;

    /**
     * The DefaultIO interface provides the following interfaces:
     * + << : writing to the defaultIO
     * + >> : reading from the defaultIO
     * + open_input(filename) : opens a file for input
     * + read() -> str : returns a string read from the input file
     * + close_input() : closes the input file
     * + open_output(filename) : opens a file for output
     * + write(str) : writes str to the output file
     * + close_output() : closes the output file
     */

    class DefaultIO {
        public:
            virtual DefaultIO& operator<<(std::string) =0;
            virtual DefaultIO& operator>>(std::string&) =0;
            virtual void open_input(std::string) =0;
            virtual std::string read() =0;
            virtual void close_input() =0;
            virtual void open_output(std::string) =0;
            virtual void write(std::string) =0;
            virtual void close_output() =0;
    };

    using streams::SerializationTokens;

    /**
     * The DefaultSocketIO class is a wrapper around the Serializer class which
     * allows for the receiving end of the serialization to know what it is to receive
     * or must send.
     * The tokens it sends to denote classes are those used defined by the ClassTokens enum.
     */
    class DefaultSocketIO : public DefaultIO {
        streams::Serializer m_serializer;

        public:
            DefaultSocketIO(streams::Stream* s) {
                this->m_serializer(s);
            }

            DefaultSocketIO(const DefaultSocketIO& other) { this->m_serializer = other.m_serializer; }

            /**
             * Deserializes an object. First a token is sent so the recipient knows what kind of type to send.
             * @param t     The object to deserialize.
             * @return      A reference to this.
             */
            DefaultSocketIO& operator>>(std::string& s) {
                this->m_serializer << SerializationTokens::send_token;
                this->m_serializer >> s;
                return *this;
            }

            /**
             * Serializes an object. First a token is sent which notifies the recipient that it is receiving
             * a serialized object (and not a token for serialization), then the token type is sent, and finally
             * the object is serialized.
             * @param t     The object to serialize.
             * @return      A reference to this.
             */
            DefaultSocketIO& operator<<(std::string s) {
                this->m_serializer << SerializationTokens::receive_token << s;
                return *this;
            }

            void open_input(std::string filename) { this->m_serializer << SerializationTokens::open_file_r_token << filename; }
            std::string read() {
                this->m_serializer << SerializationTokens::read_file_token;
                std::string s;
                this->m_serializer >> s;
                return s;
            }
            void close_input() { this->m_serializer << SerializationTokens::end_token; }

            void open_output(std::string filename) { this->m_serializer << SerializationTokens::open_file_w_token << filename; }
            void write(std::string s) { this->m_serializer << SerializationTokens::write_file_token << s; }
            void close_output() { this->m_serializer << SerializationTokens::end_token; }
    };

    /**
     * The DefaultTerminalIO implements the DefaultIO interface on files.
     */
    class DefaultTerminalIO : public DefaultIO {
        std::istream& m_input;
        std::ostream& m_output;
        std::ifstream m_file_input;
        std::ofstream m_file_output;

        public:
            DefaultTerminalIO(std::istream& input=std::cin, std::ostream& output=std::cout) :
                m_input(input), m_output(output) { }

            DefaultTerminalIO& operator<<(std::string s) { this->m_output << s; return *this; }
            DefaultTerminalIO& operator>>(std::string& s) { this->m_input >> s; return *this; }

            void open_input(std::string filename) { this->m_file_input.open(filename); }
            std::string read() {
                std::string s;
                std::getline(this->m_file_input, s);
                return s;
            }
            void close_input() { this->m_file_input.close(); }

            void open_output(std::string filename) { this->m_file_output.open(filename); }
            void write(std::string s) { this->m_file_output << s; }
            void close_output() { this->m_file_output.close(); }
    };

    class Command;
    
    class CLI {
        std::vector<Command*> m_commands;

        public:
            template <typename... Commands>
            CLI(Commands*... commands) :
                m_commands{ {commands...} } { }

            /**
             * Starts the CLI.
             * @param dataset       The datatset.
             * @param dio           The IO device to use.
             * @param exit_name     What to display for the exit option.
             */
            void start(DataSet<misc::array<double>>* dataset, DefaultIO& dio, std::string exit_name="exit");

            /**
             * This class must be public so Command-derived classes can access it.
             * Stores important information to pass to each command.
             */
            struct Settings {
                DefaultIO& dio;                                 // The io device to use
                int k_value;                                    // The k value to use in the algorithm
                double (*distance_metric)(const dubdpoint*, const dubdpoint*);
                std::string distance_metric_name;
                std::string train_file;                         // The file to train the database with (unclassified)
                std::string test_file;                          // The file to test the database with (classified)
                bool is_classified;                             // Whether or not the data has been classified already
                std::vector<std::string> true_names;            // A vector of the true class names
                std::vector<std::string> classified_names;      // A vector of the classified names
                knn::DataSet<misc::array<double>>* data_set;    // The data set

                Settings(knn::DataSet<misc::array<double>>* dataset, DefaultIO& io, int k,
                        double (*distance)(const dubdpoint*, const dubdpoint*), std::string distance_name) :
                    dio(io), data_set(dataset), k_value(k), distance_metric(distance), distance_metric_name(distance_name), is_classified(false) { }
            };
    };

    class Command {
        std::string m_description;

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
    
    class Upload_Files : public Command {
        public:
            Upload_Files(std::string description) :
                Command(description) { }

            void execute(CLI::Settings& settings) override;
    };

    class Algorithm_Settings : public Command {
        public:
            Algorithm_Settings(std::string description) :
                Command(description) { }

            void execute(CLI::Settings& settings) override;
    };

    class Classify_Data : public Command {
        public:
            Classify_Data(std::string description) :
                Command(description) { }

            void execute(CLI::Settings& settings) override;
    };

    class Display_Results : public Command {
        public:
            Display_Results(std::string description) :
                Command(description) { }

            void execute(CLI::Settings& settings) override;
    };
    
    class Download_Results : public Command {
        public:
            Download_Results(std::string description) :
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

