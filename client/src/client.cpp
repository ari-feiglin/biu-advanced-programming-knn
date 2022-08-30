#include "cli.h"

using namespace streams;
using namespace knn;

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "\e[31;1mUsage:\e[0m " << argv[0] << " <client ip> <server ip> <server port>" << std::endl;
        std::exit(1);
    }

    TCPSocket client = TCPSocket(argv[1], 0, argv[2], strtol(argv[3], NULL, 0));
 
    Upload_Files com1{"upload an unclassified csv file"};
    Algorithm_Settings com2{"algorithm settings"};
    Classify_Data com3{"classify data"};
    Display_Results com4{"display results"};
    Download_Results com5{"download results"};
    Display_Confusion_Matrix com6{"display confusion matrix"};

    CLI cli = CLI(&com1, &com2, &com3, &com4, &com5, &com6);

    cli.start(client);
}

