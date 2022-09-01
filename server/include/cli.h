#pragma once

#include "knn.h"

namespace knn {
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

    using streams::SerializationTokens;

    /**
     * The DefaultSocketIO class is a wrapper around the Serializer class which
     * allows for the receiving end of the serialization to know what it is to receive
     * or must send.
     * The tokens it sends to denote classes are those used defined by the ClassTokens enum.
     */
    class DefaultSocketIO {
        static std::map<std::type_info, int> class_map;

        Serializer m_serializer;

        public:
            DefaultSocketIO(Stream* s) {
                this->m_serializer(s);
            }

            DefaultSocketIO(const DefaultSocketIO& other) { this->m_serializer = other.m_serializer; }

            /**
             * Deserializes an object. First a token is sent so the recipient knows what kind of type to send.
             * @param t     The object to deserialize.
             * @return      A reference to this.
             */
            template <typename T>
            DefaultSocketIO& operator>>(T t) {
                this->m_serializer << class_map[typeid(T)];
                this->m_serializer >> t;
                return *this;
            }

            /**
             * Serializes an object. First a token is sent which notifies the recipient that it is receiving
             * a serialized object (and not a token for serialization), then the token type is sent, and finally
             * the object is serialized.
             * @param t     The object to serialize.
             * @return      A reference to this.
             */
            template <typename T>
            DefaultSocketIO& operator<<(T t) {
                this->m_serializer << SerializationTokens::send_token << class_map[typeid(T)] << t;
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

    std::map<std::type_info, int> DefaultSocketIO::class_map = {{typeid(int), SerializationTokens::int_token},
                                                                  {typeid(std::string), SerializationTokens::string_token}};

    /**
     * The DefaultFileIO implements the DefaultIO interface on files.
     */
    public DefaultFileIO {
        std::ifstream& m_input;
        std::ofstream& m_output;
        std::ifstream m_file_input;
        std::ofstream m_file_output;

        public:
            DefaultFileIO(std::ifstream& input=std::cin, std::ofstream& output=std::cout) :
                m_input(input), m_output(output) { }

            template <typename T>
            DefaultFileIO& operator<<(T t) { this->m_output << t; return *this; }
            template <typename T>
            DefaultFileIO& operator>>(T t) { this->m_input >> t; return *this; }

            void open_input(std::string filename) { this->m_file_input.open(filename); }
            std::string read() {
                std::string s;
                std::getline(this->m_file_input, s);
                return s;
            }
            void close_input() { this->m_file_input.close(); }

            void open_output(std::string filename) { this->m_file_output.open(filename); }
            void write(std::string s) { this->m_file_output.write << s; }
            void close_output() { this->m_file_output.close(); }
    };

    class Command;
    
    template <typename T>
    class CLI {
        std::vector<Command<T>*> m_commands;

        public:
            template <typename... Commands>
            CLI(DataSet* dataset, Commands*... commands) :
                m_dataset{dataset}, m_commands{ {commands...} } { }

            /**
             * Starts the CLI.
             * @param server        The server connected to.
             * @param exit_name     What to display for the exit option.
             */
            void start(streams::TCPSocket server, std::string exit_name="exit");

            // This class must be public so Command-derived classes can access it.
            struct Settings {
                int k_value;                            // The k value to use in the algorithm
                std::string distance_metric;            // The identifier for the distance metric (EUC, etc)
                std::string train_file;                 // The file to train the database with (unclassified)
                std::string test_file;                  // The file to test the database with (classified)
                bool is_classified;                     // Whether or not the data has been classified already
                std::vector<std::string> true_names;          // A vector of the true class names
                std::vector<std::string> classified_names;    // A vector of the classified names
                knn::DataSet<misc::array<double>>* data_set;    // The data set

                Settings() :
                    is_classified(false) { }

                Settings(int k, std::string distance) :
                    k_value(k), distance_metric(distance), is_classified(false) { }
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

