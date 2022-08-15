#include "knn.h"

using namespace streams;

template <typename T>
T* SerializablePointer<T>::null_pointer = nullptr;
template <typename T>
size_t SerializablePointer<T>::no_size = 0;

double stod(std::string s) { return std::stod(s); }

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <client ip> <server ip> <server port> <unclassified> <classified file output>" << std::endl;
        std::exit(1);
    }

    std::ifstream unclassified;
    unclassified.open(argv[4]);

    std::ofstream classified_file(argv[5]);
    
    TCPSocket client = TCPSocket(argv[1], 0, argv[2], strtol(argv[3], NULL, 0));
    Serializer serializer;
    serializer(&client);
    
    while (true) {
        std::string output;

        CartDataPoint<double>* dp = knn::read_point(unclassified, stod, false);
        if (dp == nullptr) break;
        serializer << dp;
        serializer >> output;
        classified_file << output << std::endl;
        delete dp;
    }

    classified_file.close();
}
