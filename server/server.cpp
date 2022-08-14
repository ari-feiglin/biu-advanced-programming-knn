#include "knn.h"

using namespace streams;

template <typename T>
T* SerializablePointer<T>::null_pointer = nullptr;
template <typename T>
size_t SerializablePointer<T>::no_size = 0;

double stod(std::string s) { return std::stod(s); }

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <ip> <port> <classified> <k-value>" << std::endl;
        std::exit(1);
    }

    Serializer serializer = Serializer();
    int k = strtol(argv[4], NULL, 0);

    std::ifstream classified;
    classified.open(argv[3]);

    auto data_set = knn::initialize_dataset<double>(classified, stod);

    TCPSocket server = TCPSocket(argv[1], strtol(argv[2], NULL, 0));
    server.listening(10);
    
    while (true) {
        TCPSocket client = server.accept_connection();
        serializer(&client);

        while (true) {
            knn::CartDataPoint<double> data_point;

            /* If sending/receiving from client fails, assume disconnection and disconnect */
            try {
                serializer >> data_point;
                serializer << data_set->get_nearest_class(k, &data_point, distances::euclidean_distance);
            } catch (std::ios_base::failure e) {
                break;
            }
        }
    }
}

