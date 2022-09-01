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
        
    }

}

