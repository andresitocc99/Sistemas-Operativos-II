/*********************************************
*   Project: Práctica 2 - Buscador SSOOIIGLE
*
*   Program name: ssoogle.cpp
*
*   Author: Andrés Castellanos Cantos y Felipe Segovia Friginal
*
*   Date created: 13-04-2023
*
*   Porpuse: Search for a word in a file divided into threads to speed up the process.
*
*   Revision History: It can be found in the GitHub repository.
|*********************************************/

#include <iostream>
#include <cctype>
#include <filesystem>
#include <regex>
#include <vector>
#include <thread>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <fstream>
#include <functional>
#include "WordSearched.cpp"


void checkArgumentCount (int argc);
void read_arguments (char *argv[], std::string &file, std::string &objective, int &number_threads, int &number_lines);
int read_file (std::string path);
std::map<int,std::vector<std::string>> divide_lines (std::string file, int number_lines, int number_threads, int size_threads);
void divide_and_lookfor (int &begin, int &end, int &number_threads, int &number_lines, int &size_threads, std::map<int, std::vector<std::string>> line_per_thread, std::string objective);
void find_word(int thread,std::vector<std::string> line_per_thread, int begin, int end, std::string objective);
std::vector<std::string> splitLine(std::string line);
std::string analizeWord(std::string word);
void printResult();


std::vector<std::thread> vector_threads;
std::map <int, std::vector<WordSearched>> vector_words; // vector containing the words found
std::mutex control; // semaphore that will control writing to the vector 'vector_words'


int main(int argc, char *argv[]) {

    std::string file;
    std::string objective;
    std::map<int,std::vector<std::string>> line_per_thread;
    int number_threads;
    int number_lines;
    int size_threads;
    int begin, end;
    
    checkArgumentCount(argc);
    read_arguments(argv, file, objective, number_threads, number_lines);
    size_threads = number_lines / number_threads;
    line_per_thread = divide_lines (file, number_lines, number_threads, size_threads);

    divide_and_lookfor(begin, end, number_threads, number_lines, size_threads, line_per_thread, objective);

    std::for_each(vector_threads.begin(), vector_threads.end(), std::mem_fn(&std::thread::join));
    printResult();
}

/* Check that the number of input arguments are 4. */
void checkArgumentCount(int argc) {
    if(argc!=4) {
        std::cerr << "[ERROR] -- Number of arguments is incorrect. Expected 4, but got " << argc - 1 << std::endl;
        exit(EXIT_FAILURE);
    }
}

/* Read all arguments and 'check' that are correct. */
void read_arguments(char *argv[], std::string &file, std::string &objective, int &number_threads, int &number_lines) {
    std::string path = "Libros_P2/" + std::string(argv[1]);

    number_lines = read_file(path);

    file = path;
    objective = argv[2];

    if(isdigit(atoi(argv[3])) == 0) {
        number_threads = atoi(argv[3]);
    } else {
        std::cerr << "[ERROR] -- In the number of threads, introduce a valid number " << std::endl;
        exit(EXIT_FAILURE);
    }

    if(number_threads > number_lines){
        std::cerr << "[ERROR] -- There can be no more treads than lines " << std::endl;
        exit(EXIT_FAILURE);
    }

}

/* Read and check the file and gets the number of lines */
int read_file (std::string path) {
    std::ifstream File(path);
    int lines = 0;
    if(File.good()) {
        while (File.good()) { 
            std::string line; 
            std::getline(File, line); 
            ++lines; 
        } 
    } else {
        std::cerr << "[ERROR] -- The File does not exist " << std::endl;
        exit(EXIT_FAILURE);
    }
    return lines;
}

/* Keep in the data structure the differents blocks of lines */
std::map<int,std::vector<std::string>> divide_lines (std::string file, int number_lines, int number_threads, int size_threads) {
    std::ifstream File(file);
    std::string line;
    std::map<int, std::vector<std::string>> result;
    for(int i = 0; i < number_threads; i++){
        for (int j = 0; j < size_threads; j++) {
            std::getline(File, line);
            result[i].push_back(line);
        }

        if(i == number_threads-1 && number_lines % number_threads != 0) {       // in the event that the division is not exact, the remaining lines will be made by the last thread
            for (int i = size_threads*number_threads; i < number_lines; i++) {  
                std::getline(File, line);
                result[i].push_back(line);
            }
        }
    }
    return result; 

}

/* Method that calculates the lines that will be assigned to each thread */
void divide_and_lookfor (int &begin, int &end, int &number_threads, int &number_lines, int &size_threads, std::map<int, std::vector<std::string>> line_per_thread, std::string objective) {
    for (int i = 0; i < number_threads; i++) {
        begin = i *size_threads+1;
        end = begin + size_threads - 1;

        if(number_lines%number_threads!= 0 && i==number_threads-1){  // corrects the number of lines if the division is not exact
            end = number_lines;
        }

        vector_threads.push_back(std::thread(find_word, i, line_per_thread[i], begin, end, objective));
    }
}

/* Method to find the given word*/
void find_word(int thread,std::vector<std::string> line_per_thread, int begin, int end, std::string objective){
    std::string word;
    std::string solution[3]; // Vector of 3 size positions to keep the word before, the word finded and the following one
    std::vector<std::string> line;
    for(std::size_t number_line = 0; number_line < line_per_thread.size(); number_line++){
        line = splitLine(line_per_thread[number_line]);
        solution[0]=""; //If there is a new line, word before will be reset
        for(std::size_t position = 0; position< line.size(); position++){
            if(!analizeWord(objective).compare(analizeWord(line[position]))){
                solution[1]=line[position];
                if(position+1==line.size()){ // In case that objective words is found at the end of the line, the value of the next word will be null
                    solution[2]="";
                }else{
                    solution[2]=line[position+1];
                }
                WordSearched wordFounded(objective, thread, begin, end, number_line+begin, solution[0], solution[1], solution[2]);
                std::lock_guard<std::mutex> semaphore(control); //Here lock the access to the data structure
                vector_words[thread].push_back(wordFounded);
            }
            solution[0]=line[position];
        }
            
    }
}

/* Method that will return a vector with all the words of the line passed by argument */
std::vector<std::string> splitLine(std::string line){
    std::string word;
    std::vector<std::string> result;
    std::stringstream buffer(line);
    while(std::getline(buffer, word, ' ')){
        result.push_back(word);
    }
    return result;
}

/* Here we analice choosen word to check if it's equal than the input word.*/
std::string analizeWord(std::string word){
    std::string result;
    for (std::size_t i = 0; i < word.length(); i++) { 
        word[i] = tolower(word[i]);
    }
     std::remove_copy_if(word.begin(), word.end(), std::back_inserter(result), std::ptr_fun<int, int>(&std::ispunct));//Y esto para eliminar simbilos que no sean letras
    return result;
}

/* Method that calls the toString of all the words found */
void printResult(){
    for (std::size_t i = 0; i < vector_words.size(); i++){
        for (std::size_t j = 0; j < vector_words[i].size(); j++){
            vector_words[i][j].toString();
        }   
    }
}