#include "knn.h"

using namespace streams;

double stod(std::string s) { return std::stod(s); }

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <client ip> <server ip> <server port> <unclassified> <classified file dir>" << std::endl;
        std::exit(1);
    }

    std::ifstream unclassified;
    unclassified.open(argv[4]);

    std::ofstream classified_file(argv[5]);

    CartDataPoint<double> dp = read_datapoint(unclassified, stod, false);
    
    TCPSocket client = TCPSocket(argv[1], 0, argv[2], argv[3]);
    
    while (true) {
        std::string output;

        try {
            serializer(client) << dp;
            serializer >> output;
            classified_file << output << std::endl;
        } catch (std::ios_base::failure e) {
                break;
        }
    }
}