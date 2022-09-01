#include "knn.h"
#include <csignal>
#include "thread-pool.h"
#include "cli.h"
#include <map>

using namespace knn;
using namespace threading;
using namespace knn;

double stod(std::string s) { return std::stod(s); }

#if 0
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
    std::map<std::string, distance_func> distance_metric_map = { {"EUC", distances::euclidean_distance},
                                                                 {"MAN", distances::manhattan_distance},
                                                                 {"CHE", distances::chebyshev_distance}};

    // TBD
    int k;
    std::string distance_metric;

    classified.open(argv[3]);

    data_set = knn::initialize_dataset<double>(classified, stod);

    TCPSocket server = TCPSocket(argv[1], strtol(argv[2], NULL, 0));
    server.listening(10);
    
    ThreadPool thread_pool{50};
    
    Upload_Files com1{"upload an unclassified csv file"};
    Algorithm_Settings com2{"algorithm settings"};
    Classify_Data com3{"classify data"};
    Display_Results com4{"display results"};
    Download_Results com5{"download results"};
    Display_Confusion_Matrix com6{"display confusion matrix"};
    
    while (true) {
        TCPSocket client = server.accept_connection(300); // times out after 5 minutes with no connection
        Address addr = client.get_address();
        std::cout << addr.ip << ":" << addr.port << " connected." << std::endl;
        serializer(&client);
        // assigning a thread for each new client.
        thread_pool.add_job(CLI::start, CLI(&data_set, serializer, &com1, &com2, &com3, &com4, &com5, &com6));


        

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

#endif

int main(int argc, char** argv) {
    std::ifstream classified;
    classified.open(argv[1]);
    DataSet<misc::array<double>>* data_set = knn::initialize_dataset<double>(classified, stod);

    DefaultTerminalIO dio;

    Upload_Files com1{"upload an unclassified csv file"};
    Algorithm_Settings com2{"algorithm settings"};
    Classify_Data com3{"classify data"};
    Display_Results com4{"display results"};
    Download_Results com5{"download results"};
    Display_Confusion_Matrix com6{"display confusion matrix"};

    CLI cli{&com1, &com2, &com3, &com4, &com5, &com6};
    cli.start(data_set, dio); 
}
