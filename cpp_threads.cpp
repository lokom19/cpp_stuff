#include <iostream>
#include <thread>
#include <vector>
#include <list>
#include <boost/filesystem.hpp>
// #include <mutex>

namespace fs = boost::filesystem;

void workerFunction(std::list<std::string>& il) {
    std::cout << il.size() << std::endl;
    for (std::list<std::string>::iterator it = il.begin(); it != il.end(); ++it) {
        std::string command = "echo " + *it;
                    // std::lock_guard<std::mutex> lock(mtx); // Заблокировать мьютекс
        system(command.c_str());
    }
}

std::list<std::string> get_files_list() {
    std::string path = "/home";
    std::list<std::string> list_of_files;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        std::string file_path = entry.path().c_str();
        list_of_files.emplace_back(file_path);
    }
    std::cout << list_of_files.size() << std::endl;
    // exit(0);
    return list_of_files;
}


int main() {
    std::vector<std::thread> threads;
    std::list<std::string> list_of_files = get_files_list();

    const int numInnerLists = 15;
    std::vector<std::list<std::string>> listOfLists(numInnerLists);

    int elementsPerList = list_of_files.size() / numInnerLists;
    int extraElements = list_of_files.size() % numInnerLists;

    auto it = list_of_files.begin();
    
    for (int i = 0; i < numInnerLists; ++i) {
        int currentElements = elementsPerList + (extraElements-- > 0 ? 1 : 0);
        listOfLists[i].insert(listOfLists[i].end(), it, std::next(it, currentElements));
        std::advance(it, currentElements);
    }

    for (int i = 1; i < numInnerLists; i++) {
        std::cout << listOfLists[i].size() << std::endl;
        threads.emplace_back(workerFunction, std::ref(listOfLists[i]));
    }

    for (int i = 0; i < threads.size(); ++i) {
        threads[i].join();
    }

    return 0;
}
