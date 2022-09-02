#include "cli.h"

#include <set>
#include <vector>

namespace knn {
    double stod(std::string s) { return std::stod(s); }

    void CLI::start(dubdset* data_set, DefaultIO& io_device, std::string exit_name) {
        CLI::Settings settings{data_set, io_device, 5, distances::euclidean_distance, "EUC"};
    
        while (true) {
            int i = 1;
    
            for (auto& com : this->m_commands) {
                settings.dio << std::to_string(i) + ".\t" + com->get_description() + "\n";
                i++;
            }
            settings.dio << std::to_string(i) + ".\t" + exit_name + "\n";
    
            std::string s_choice;
            settings.dio >> s_choice;
            int choice = std::stoi(s_choice);
    
            if (choice <= 0 || choice > (int)this->m_commands.size() + 1)
                settings.dio << "\e[31;1mInvalid Command\e[0m\n";
            else if (choice == (int)this->m_commands.size() + 1) break;
            else this->m_commands.at(choice-1)->execute(settings);
        }
    }
    
    void Upload_Files::execute(CLI::Settings& settings) {
        // while loop to ensure valid input
        while (true) {
            // open train file
            settings.dio << "Please upload your local train CSV file. (Enter ! to skip)\n";
            std::string train_path;
            settings.dio >> train_path;
    
            // If the user skips this step, dont change the current train file.
            if (train_path == "!") {
                if (settings.train_file != "") settings.dio << "Leaving the train file unchanged...\n";
                else {
                    settings.dio << "You haven't uploaded a train file previously.\n";
                    continue;
                }
            } else {
                // resetting the classified to false
                settings.is_classified = false;
                settings.train_file = train_path;
                settings.dio << "Upload complete\n";
            }
    
            break;
        }
    
        // open test file
        settings.dio << "Please upload your local test CSV file.\n";
        settings.dio >> settings.test_file;
        settings.dio << "Upload complete\n";
    }
    
    void Algorithm_Settings::execute(CLI::Settings& settings) {
        settings.dio << std::string("The current KNN parameters are: K = ") +
                  std::to_string(settings.k_value) + ", distance metric = " +
                  settings.distance_metric_name + "\n";

        std::map<std::string, double (*)(const dubdpoint*, const dubdpoint*)> distance_map =
            {{"EUC", distances::euclidean_distance}, {"MAN", distances::manhattan_distance}, {"CHE", distances::chebyshev_distance}};
        
        while (true) {
            int k;
            std::string s_k;
            std::string distance_metric;
            settings.dio >> s_k >> distance_metric;
            k = std::stoi(s_k);
    
            // check if k is between 1-10
            if (k < 1 || k > 10) {
                settings.dio << "\e[31;1mInvalid value for K, please try again\e[0m\n";
                continue;
            }
    
            // check if distance metric is EUC, MAN or CHE
            if (distance_map.find(distance_metric) == distance_map.end()) {
                settings.dio << "\e[31;1mInvalid distance metric, please try again\e[0m\n";
                continue;
            }
    
            // valid values
            settings.k_value = k;
            settings.distance_metric = distance_map[distance_metric];
            settings.distance_metric_name = distance_metric;
            settings.is_classified = false;
            break;
        }
    }
    
    void Classify_Data::execute(CLI::Settings& settings) {
        settings.classified_names = std::vector<std::string>();
    
        settings.dio.open_input(settings.train_file);
    
        while (true) {
            std::string output = settings.dio.read();
            if (output == "") break;
    
            CartDataPoint<double>* dp = knn::get_point<double>(output, stod, false);
            settings.classified_names.push_back(settings.data_set->get_nearest_class(settings.k_value, dp, settings.distance_metric));
    
            delete dp;
        }
    
        settings.dio.close_output();
    
        settings.is_classified = true;
    }
    
    void Display_Results::execute(CLI::Settings& settings) {
        if (!settings.is_classified) {
            settings.dio << "\e[31;1mHaven't classified any data yet!\e[0m\n";
            return;
        }
        
        int length = settings.classified_names.size();
        for (int i = 0; i < length; i++) {
            settings.dio << std::to_string(i + 1) + "\t" + settings.classified_names[i] + "\n";
        }
        
        settings.dio << "Done.\n";
    }
    
    void Download_Results::execute(CLI::Settings& settings) {
        if (!settings.is_classified) {
            settings.dio << "\e[31;1mHaven't classified any data yet!\e[0m\n";
            return;
        }
    
        std::string results_path;
        settings.dio << "Please type the path for saving the results.\n";
        settings.dio >> results_path;
        settings.dio.open_output(results_path);
    
        int length = settings.classified_names.size();
        for (int i = 0; i < length; i++) {
            settings.dio.write(std::to_string(i + 1) + "\t." + settings.classified_names[i] + "\n");
        }
        
        settings.dio.close_output();
    }
    
    void Display_Confusion_Matrix::execute(CLI::Settings& settings) {
        if (!settings.is_classified) {
            settings.dio << "\e[31;1mHaven't classified any data yet!\e[0m\n";
            return;
        }
    
        std::set<std::string> classes;
        std::vector<std::string> true_classes;
    
        std::string true_class;
    
        settings.dio.open_input(settings.test_file);
        size_t i = 0;
        /* Get all class names as well as the true classes */
        for (; (true_class = settings.dio.read()) != ""; i++) {
            true_classes.push_back(true_class);
            classes.insert(true_class);
            classes.insert(settings.classified_names[i]);
        }
        settings.dio.close_input();

        if (i != settings.classified_names.size())
            settings.dio << std::string("\e[31;1mMismatch between number of classified and true classes.\e[0m Have ") +
                std::to_string(settings.classified_names.size())  + " and "  + std::to_string(i) +
                ".\n\tPlease ensure that your test and train files " + "have the same number of lines.\n";
    
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
        for (size_t i = 0; i < true_classes.size(); i++) {
            confusion_matrix[class_order[true_classes[i]]][class_order[settings.classified_names[i]]]++;
            true_count[class_order[true_classes[i]]]++;
        }
    
        /* Print the confusion matrix */
        for (size_t i = 0; i < classes.size(); i++) {
            std::string line;
            for (size_t j = 0; j < classes.size(); j++) {
                std::string num;
                if (true_count[i] > 0) num = std::to_string((100 * confusion_matrix[i][j]) / true_count[i]) + "%";
                else if (confusion_matrix[i][j] == 0) num = "0%";
                else num = "inf";
                line += std::string("|") + std::string((8 - num.length()) / 2, ' ') + num +
                    std::string(8 - num.length() - (8 - num.length()) / 2, ' ');
            }

            settings.dio << line + "|\n";
    
            delete[] confusion_matrix[i];
        }
    
        delete[] confusion_matrix;
        delete[] true_count;
    }
}

