#include "knn.h"
#include "serialization.h"

using namespace streams;
using namespace knn;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <client ip> <server ip> <server port>" << std::endl;
        std::exit(1);
    }

    TCPSocket client = TCPSocket(argv[1], 0, argv[2], strtol(argv[3], NULL, 0));
    
    Serializer serializer;
    serializer(&client);

    SerializationTokens token;

    while (true) {
        serializer >> token;
        
        // just prints string
        if (token == receive_token) {
            std::string print_string;
            serializer >> print_string;
            std::cout << print_string;
            continue;
        }

        // sends string
        if (token == send_token) {
            std::string send_string;
            std::cin >> send_string;
            serializer << send_string;
            continue;
        }

        // wants input from file
        if (token == open_file_r_token) {
            std::string file_path;
            serializer >> file_path;
            std::ifstream file;
            file.open(file_path);
            serializer >> token;
            std::string line;
            while (token != end_token) {
                std::getline(file, line);
                serializer << line;
                serializer >> token;
            }
            file.close();
            continue;
        }

        // wants to write to file
        if (token == open_file_w_token) {
            std::string file_path;
            serializer >> file_path;
            std::ofstream file(file_path);
            serializer >> token;
            std::string line;
            while (token != end_token) {
                serializer >> line;
                file << line;
                serializer >> token;
            }
            file.close();
            continue;
        }

        // wants to exit
        if (token == end_token) {
            break;
        }
    }

    client.close();

}

