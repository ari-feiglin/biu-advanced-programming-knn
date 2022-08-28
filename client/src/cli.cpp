#include "cli.h"

using namespace knn;

void CLI::start(TCPSocket server, std::string exit_name) {
    CLI::Settings settings{server, 5, "EUC"};

    while (true) {
        int i = 1;

        for (auto& com : this->m_commands) {
            std::cout << i << ".\t" << com->get_description() << std::endl;
            i++;
        }
        std::cout << i << ".\t" << exit_name << std::endl;

        int choice;
        std::cin >> choice;

        if (choice <= 0 || choice > this->m_commands.size() + 1)
            std::cout << "\e[31;1mInvalid Command\e[0m" << std::endl;
        else if (choice == this->m_commands.size() + 1) break;
        else this->m_commands[i]->execute(settings);
    }
}

void Classify_Data::execute(CLI::Settings& settings) override {
    settings.classified.clear();

    while (true) {
        std::string output;
        CartDataPoint<double>* dp = read_point(settings.train_file, stod, false);
        if (dp == nullptr) break;

        settings.serializer << dp;
        settings.serializer >> output;
        settings.classified.push_back(output);

        delete dp;
    }
}

void Display_Confusion_Matrix::execute(CLI::Settings& settings) override {
    if (!settings.classified) {
        std::cout << "\e[31;1mHaven't classified any data yet!\e[0m" << std::endl;
        return;
    }

    std::set<std::string> classes;
    std::vector<std::string> true_classes;

    settings.test_file.clear();
    settings.test_file.seekg(0);

    std::string true_class;

    /* Get all class names as well as the true classes */
    for (int i = 0; std::getline(settings.test_file, true_class); i++) {
        true_classes.push_back(true_class);
        classes.insert(true_class);
        classes.insert(settings.classified[i]);
    }

    std::unordered_map<std::string, size_t> class_order;
    int i = 0;

    /* Order the classes */
    for (std::string class_name : classes) {
        class_order[class_name] = i;
        i++;
    }

    auto true_count = new size_t[classes.size()];
    auto confusion_matrix = new size_t[classes.size()][classes.size()];

    /* Compute the confusion matrix */
    for (int i = 0; i < true_count.size(); i++) {
        confusion_matrix[class_order[true_classes[i]]][class_order[settings.classified[i]]]++;
        true_count[class_order[true_classes[i]]]++;
    }

    /* Print the confusion matrix */
    for (int i = 0; i < confusion_matrix.size(); i++) {
        for (int j = 0; j < confusion_matrix[i].size(); j++) {
            if (true_count[i] > 0) printf("%-5d", 100 * confusion_matrix[i][j] / true_count[i]);
            else if (confusion_matrix[i][j] == 0) printf("    0");
            else printf("  inf");
        }
        putchar('\n');

        delete[] confusion_matrix[i];
    }

    delete[] confusion_matrix;
    delete[] true_count;
}

