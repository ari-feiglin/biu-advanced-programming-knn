#include "knn.h"
#include <csignal>

using namespace streams;

template <typename T>
T* SerializablePointer<T>::null_pointer = nullptr;
template <typename T>
size_t SerializablePointer<T>::no_size = 0;

double stod(std::string s) { return std::stod(s); }

knn::DataSet<misc::array<double>>* data_set;
std::ifstream classified;

void sigint_handler(int signum) {
    delete data_set;
    classified.close();
    std::exit(signum);
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <ip> <port> <classified> <k-value>" << std::endl;
        std::exit(1);
    }

    signal(SIGINT, sigint_handler);

    Serializer serializer = Serializer();
    int k = strtol(argv[4], NULL, 0);

    classified.open(argv[3]);

    data_set = knn::initialize_dataset<double>(classified, stod);

    TCPSocket server = TCPSocket(argv[1], strtol(argv[2], NULL, 0));
    server.listening(10);
    
    while (true) {
        TCPSocket client = server.accept_connection();
        Address addr = client.get_address();
        std::cout << addr.ip << ":" << addr.port << " connected." << std::endl;
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

        std::cout << "Session with " << addr.ip << ":" << addr.port << " has ended." << std::endl;
    }

    classified.close();
    delete data_set;
}

