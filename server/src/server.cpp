#include "knn.h"
#include "cli.h"
#include <csignal>
#include "thread-pool.h"
#include <map>

using namespace streams;
using namespace threading;
using namespace knn;

void thread_job(Address addr, CLI cli, DataSet<misc::array<double>>* dataset, DefaultIO* dio) {
    cli.start(dataset, *dio);
    std::cout << "Session with " << addr.ip << ":" << addr.port << " has ended." << std::endl;
}

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

    classified.open(argv[3]);
    data_set = knn::initialize_dataset<double>(classified, stod);
    classified.close();

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
        DefaultSocketIO dio{&client};

        // assigning a thread for each new client.
        thread_pool.add_job(thread_job, addr, CLI(&com1, &com2, &com3, &com4, &com5, &com6), data_set, (DefaultIO*)&dio);
    }

    delete data_set;
}

