#include "knn.h"
#include "distances.h"
#include <csignal>
#include "thread-pool.h"
#include <map>

using namespace streams;
using namespace threading;

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
    if (argc < 3) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <ip> <port> <classified>" << std::endl;
        std::exit(1);
    }

    signal(SIGINT, sigint_handler);

    Serializer serializer = Serializer();

    using distance_func = double (*)(const knn::DataPoint<misc::array<double>>*, const knn::DataPoint<misc::array<double>>*);
    std::map<std::string, distance_func> distance_metric_map;
    distance_metric_map["EUC"] = distances::euclidean_distance;
    distance_metric_map["MAN"] = distances::manhattan_distance;
    distance_metric_map["CHE"] = distances::chebyshev_distance;

    // TBD
    int k;
    std::string distance_metric;

    classified.open(argv[3]);

    data_set = knn::initialize_dataset<double>(classified, stod);

    TCPSocket server = TCPSocket(argv[1], strtol(argv[2], NULL, 0));
    server.listening(10);

    //ThreadPool thread_pool{50}; // Commented this out since we havent fully implemented the threading yet
    
    while (true) {
        TCPSocket client = server.accept_connection(300); // times out after 5 minutes with no connection
        Address addr = client.get_address();
        std::cout << addr.ip << ":" << addr.port << " connected." << std::endl;
        serializer(&client);
        char token;
        knn::CartDataPoint<double> data_point;

        while (true) {
            /* If sending/receiving from client fails, assume disconnection and disconnect */
            try {
                // get token, if 0 get k and distance_metric
                serializer >> token;
                if (token == 0) {
                    std::cout << "Getting settings" << std::endl;
                    serializer >> k >> distance_metric;
                    continue;
                }
                // else its 1 and we can get data point and return it as usual
                serializer >> data_point;
                std::cout << distance_metric << std::endl;
                serializer << data_set->get_nearest_class(k, &data_point, distance_metric_map[distance_metric]);
            } catch (std::ios_base::failure e) {
                break;
            }
        }

        std::cout << "Session with " << addr.ip << ":" << addr.port << " has ended." << std::endl;
    }

    classified.close();
    delete data_set;
}

