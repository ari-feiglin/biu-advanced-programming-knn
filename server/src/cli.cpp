#include <set>
#include <vector>

#include "knn.h"
#include "cli.h"

using namespace knn;

template <typename T>
T* streams::SerializablePointer<T>::null_pointer = nullptr;
template <typename T>
size_t streams::SerializablePointer<T>::no_size = 0;

double stod(std::string s) { std::stod(s); }

void CLI::start(streams::TCPSocket server, std::string exit_name) {
    CLI::Settings settings{server, 5, "EUC"};

    while (true) {
        int i = 1;

        for (auto& com : this->m_commands) {
            this->m_io_device << std::to_string(i) + ".\t" + com->get_description() + "\n";
            i++;
        }
        this->m_io_device << std::to_string(i) + ".\t" + exit_name + "\n";

        int choice;
        this->m_io_device >> choice;

        if (choice <= 0 || choice > this->m_commands.size() + 1)
            this->m_io_device << "\e[31;1mInvalid Command\e[0m" << std::endl;
        else if (choice == this->m_commands.size() + 1) break;
        else this->m_commands.at(choice-1)->execute(settings);
    }
}

void Upload_Files::execute(CLI::Settings& settings) {
    // while loop to ensure valid input
    while (true) {
        // open train file
        this->m_io_device << "Please upload your local train CSV file. (Enter ! to skip)\n";
        std::string train_path;
        this->m_io_device >> train_path;

        // If the user skips this step, dont change the current train file.
        if (train_path == "!") {
            if (settings.train_file != "") this->m_io_device << "Leaving the train file unchanged...\n";
            else {
                this->m_io_device << "You haven't uploaded a train file previously.\n";
                continue;
            }
        } else {
            // resetting the classified to false
            settings.is_classified = false;
            settings.train_file = train_path;
            this->m_io_device << "Upload complete\n";
        }

        break;
    }

    // open test file
    this->m_io_device << "Please upload your local test CSV file.\n";
    this->m_io_device >> settings.test_file;
    this->m_io_device << "Upload complete" << std::endl;
}

void Algorithm_Settings::execute(CLI::Settings& settings) {
    this->m_io_device << std::string("The current KNN parameters are: K = ") +
              std::to_string(settings.k_value) + ", distance metric = " +
              settings.distance_metric + "\n";
    
    while (true) {
        int k;
        std::string distance_metric;
        this->m_io_device >> k >> distance_metric;

        // check if k is between 1-10
        if (k < 1/* || k > 10*/) {
            this->m_io_device << "\e[31;1mInvalid value for K, please try again\e[0m\n";
            continue;
        }

        // check if distance metric is EUC, MAN or CHE
        if (distance_metric.compare("EUC") != 0 &&
            distance_metric.compare("MAN") != 0 &&
            distance_metric.compare("CHE") != 0) {
            this->m_io_device << "\e[31;1mInvalid distance metric, please try again\e[0m\n";
            continue;
        }

        // valid values
        settings.k_value = k;
        settings.distance_metric.assign(distance_metric);
        settings.is_classified = false;
        break;
    }
}

void Classify_Data::execute(CLI::Settings& settings) {
    settings.classified_names = std::vector<std::string>();

    this->m_io_device.open_input(settings.train_file);

    while (true) {
        std::string output = this->m_io_device.read();
        if (output == "") break;

        CartDataPoint<double>* dp = knn::get_point<double>(output, stod, false);
        settings.classified_names.push_back(settings.data_set.get_nearest_class(settings.k, settings.distance_metric));

        delete dp;
    }

    this->m_io_device.close_output();

    settings.is_classified = true;
}

void Display_Results::execute(CLI::Settings& settings) {
    if (!settings.is_classified) {
        this->m_io_device << "\e[31;1mHaven't classified any data yet!\e[0m\n";
        return;
    }
    
    int length = settings.classified_names.size();
    for (int i = 0; i < length; i++) {
        this->m_io_device << std::to_string(i + 1) + "\t" + settings.classified_names[i] + "\n";
    }
    
    this->m_io_device << "Done.\n";
}

void Download_Results::execute(CLI::Settings& settings) {
    if (!settings.is_classified) {
        this->m_io_device << "\e[31;1mHaven't classified any data yet!\e[0m" << std::endl;
        return;
    }

    std::string results_path;
    this->m_io_device << "Please type the path for saving the results." << std::endl;
    this->m_io_device >> results_path;
    this->m_io_device.open_output(results_path);

    int length = settings.classified_names.size();
    for (int i = 0; i < length; i++) {
        results << std::to_string(i + 1) + "\t" + settings.classified_names[i] + "\n";
    }
    
    this->m_io_device.close_output();
}

void Display_Confusion_Matrix::execute(CLI::Settings& settings) {
    if (!settings.is_classified) {
        this->m_io_device << "\e[31;1mHaven't classified any data yet!\e[0m" << std::endl;
        return;
    }

    std::set<std::string> classes;
    std::vector<std::string> true_classes;

    settings.test_file.clear();
    settings.test_file.seekg(0);

    std::string true_class;

    int i = 0;
    /* Get all class names as well as the true classes */
    for (; std::getline(settings.test_file, true_class); i++) {
        true_classes.push_back(true_class);
        classes.insert(true_class);
        classes.insert(settings.classified_names[i]);
    }

    if (i != settings.classified_names.size())
        this->m_io_device << "\e[31;1mMismatch between number of classified and true classes.\e[0m Have " <<
            settings.classified_names.size() << " and " << i << ".\n\tPlease ensure that your test and train files "
            "have the same number of lines." << std::endl;

    std::unordered_map<std::string, size_t> class_order;
    i = 0;
    size_t* true_count = new size_t[classes.size()]();
    size_t**  confusion_matrix = new size_t*[classes.size()]();

    /* Order the classes, and allocate confusion matrix */
    for (std::string class_name : classes) {
        confusion_matrix[i] = new size_t[classes.size()]();
        class_order[class_name] = i;
        i++;
    }

    /* Compute the confusion matrix */
    for (int i = 0; i < true_classes.size(); i++) {
        confusion_matrix[class_order[true_classes[i]]][class_order[settings.classified_names[i]]]++;
        true_count[class_order[true_classes[i]]]++;
    }

    /* Print the confusion matrix */
    for (int i = 0; i < classes.size(); i++) {
        for (int j = 0; j < classes.size(); j++) {
            /*if (true_count[i] > 0) printf("|%5ld|", (100 * confusion_matrix[i][j]) / true_count[i]);
            else if (confusion_matrix[i][j] == 0) printf("|    0|");
            else printf("|  inf|");*/
            printf("|%5ld|", confusion_matrix[i][j]);
        }
        putchar('\n');

        delete[] confusion_matrix[i];
    }

    delete[] confusion_matrix;
    delete[] true_count;
}

