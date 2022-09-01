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

        // just prints int
        if (token == int_token) {
            int print_int;
            serializer >> print_int;
            std::cout << print_int;
            continue;
        }

        // just prints string
        if (token == string_token) {
            std::string print_string;
            serializer >> print_string;
            std::cout << print_string;
            continue;
        }

        // sends something
        if (token == send_token) {
            serializer >> token;
            
            // sends int
            if (token == int_token) {
                int send_int;
                std::cin >> send_int;
                serializer << send_int;
                continue;
            }

            // sends string
            if (token == string_token) {
                std::string send_string;
                std::cin >> send_string;
                serializer << send_string;
                continue;
            }
        }
    }

}

