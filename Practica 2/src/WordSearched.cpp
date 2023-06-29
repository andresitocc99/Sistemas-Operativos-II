/*********************************************
*   Project: Práctica 2 - Buscador SSOOIIGLE 
*
*   Program name: WordSearched.cpp
*
*   Author: Andrés Castellanos Cantos y Felipe Segovia Friginal
*
*   Date created: 13-04-2023
*
*   Porpuse: Classes to store information related to searched words.
*
*   Revision History: It can be found in the GitHub repository.
|*********************************************/

#include <iostream>

class WordSearched {
private:
    int nThread;
    int begin;
    int end;
    int line;
    std::string objective;
    std::string word_before;
    std::string word;
    std::string word_after;

public:
    WordSearched(std::string objective, int nThread, int begin, int end, int line, std::string word_before, std::string word, std::string word_after);
    void toString();
};

WordSearched::WordSearched(std::string objective,int nThread, int begin, int end, int line, std::string word_before, std::string word, std::string word_after) {
    this->nThread=nThread;
    this->begin=begin;
    this->end=end;
    this->line=line;
    this->objective=objective;
    this->word_before=word_before;
    this->word=word;
    this->word_after=word_after;
}

void WordSearched::toString(){
    std::cout<< "[Hilo " << std::to_string(this->nThread+1) + " inicio: " << std::to_string(this->begin) + " – final: " << std::to_string(this->end) + "] :: línea " << std::to_string(this->line) + " :: ... " << this->word_before + " " <<this->word + " " << this->word_after + " ..." <<std::endl;
}
