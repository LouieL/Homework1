//
//  main.cpp
//
//  Created by HL on 14/10/17.
//  Copyright © 2017 Huan Liu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <unordered_map>
#include <cmath>

using namespace std;

struct First30words {
    string word;
    float tf=0;
    float df=0;
    float idf=0;
    float tf_idf=0;
    float possibility=0;
};

//-----------------------------------------------------------------------------------
void readFiles(vector<string>& files)
{
    struct dirent *ptr;
    DIR *dir;
    string PATH = "./transcripts"; // files are in "transcripts" folder
    dir=opendir(PATH.c_str());
    
    while((ptr=readdir(dir))!=NULL)
    {
        
        //ignore'.' and '..'
        if(ptr->d_name[0] == '.')
            continue;
        files.push_back(ptr->d_name);
        
    }
    
    closedir(dir);
    
}

//-----------------------------------------------------------------------------------
void readWords(vector<string> files, vector<string>& words_vector, vector<unordered_map<string, int>>& wordsInFile)
{
    for(int a=0; a<files.size(); ++a)
    {
        string file= "./transcripts/" + files[a];
        char ch;
        vector<string> word;  //store the words in each file temporarily
        string temp_word;
        
        unordered_map<string, int> temp_wordInFile;
        
        fstream infile,outfile;
        infile.open(file,ios::in);
        infile.unsetf(ios::skipws);
        
        if(!infile)
        {
            cerr<<"Can't open  file "<<file<<endl;
        }
        while(infile>>ch)
        {
            
            if(isalpha(ch))
            {
                if(ch>=65 && ch<=90)  // ignore the letter case, change upper case to lower case
                    ch += 32;
                
                temp_word += ch;
            }
            else
            {
                {
                    if(temp_word=="")
                        continue;
                    else if(temp_word=="s")
                        temp_word="is";
                    else if(temp_word=="t")
                        temp_word="not";
                    else if(temp_word=="m")
                        temp_word="am";
                    
                    words_vector.push_back(temp_word);
                    
                    if(temp_wordInFile.find(temp_word) == temp_wordInFile.end())
                        temp_wordInFile[temp_word] = 1;
                    else
                        ++temp_wordInFile[temp_word];
                    
                    temp_word = "";
                }
            }
            
        }
        
        infile.close();
        
        wordsInFile.push_back(temp_wordInFile);
        
    }
    
}

//-----------------------------------------------------------------------------------
int cmp(const pair<std::string, int>& x, const pair<std::string, int>& y)
{
    return x.second > y.second;
}

//-----------------------------------------------------------------------------------
void sortMapByValue(unordered_map<std::string, int>& tMap, vector<pair<string, int>>& tVector)
{
    for (unordered_map<std::string, int>::iterator curr = tMap.begin(); curr != tMap.end(); curr++)
    {
        tVector.push_back(make_pair(curr->first, curr->second));
    }
    
    sort(tVector.begin(), tVector.end(), cmp);
}


//-----------------------------------------------------------------------------------
int appearOnceCount(unordered_map<string, int> wordsFreq)
{
    int count=0;
    
    for(unordered_map<string, int>::iterator it=wordsFreq.begin(); it != wordsFreq.end(); ++it)
    {
        if(it->second == 1)
            ++count;
    }
    
    return count;
}

//-----------------------------------------------------------------------------------
int main()
{
    float totalFiles = 404;
    vector<string> words_all;
    vector<string> files;
    unordered_map<string, int> wordsFreq;
    unordered_map<string, int> wordAppear;   // first 30 words
    vector<unordered_map<string, int>> wordsInFile;  // stored by file
    vector<pair<string, int>> order;
    vector<First30words> first30Words;
    int appearOnce;
    float averageNum;
    
    readFiles(files);
    readWords(files, words_all, wordsInFile);
    
    for(int m=0; m<words_all.size(); ++m)
    {
        
        if(wordsFreq.find(words_all[m]) == wordsFreq.end())
            wordsFreq[words_all[m]] = 1;
        else
            ++wordsFreq[words_all[m]];
    }
    
    sortMapByValue(wordsFreq, order);  // sort by tf
    appearOnce = appearOnceCount(wordsFreq);
    
    
    
    for(int i=0; i<30; ++i)
    {
        wordAppear[order[i].first] = 0;
        for(int j=0; j<wordsInFile.size(); ++j)
        {
            if(wordsInFile[j].find(order[i].first) != wordsInFile[j].end())
                ++wordAppear[order[i].first];
        }
    }
    
    
    for(int i=0; i<30; ++i)
    {
        
        First30words temp;
        temp.word = order[i].first;
        temp.tf = order[i].second;
        for(unordered_map<string, int>::iterator it_30words=wordAppear.begin(); it_30words!=wordAppear.end(); ++it_30words)
        {
            if(it_30words->first == temp.word)
                temp.df = it_30words->second;
        }
        
        temp.idf = log10(totalFiles/temp.df);
        temp.tf_idf = temp.tf * temp.idf;
        temp.possibility = temp.tf / words_all.size();
        
        first30Words.push_back(temp);
        
    }
    
    
    
    // print results
    
    averageNum = words_all.size() / totalFiles;
    
    cout <<"Q1. Total number of words:      "<<words_all.size()<<endl<<endl;
    cout<<"---------------------------"<<endl;
    
    cout <<"Q2. Unique words:       "<<wordsFreq.size()<<endl<<endl;
    cout<<"---------------------------"<<endl;
    
    cout <<"Q3. words only appear once:        " << appearOnce << endl << endl;
    cout<<"---------------------------"<<endl;
    
    cout <<"Q4."<<endl<<"term"<<"  "<<"Tf"<<"    "<<"df"<<"       "<<"idf"<<"       "<<"Tf.idf"<<"     "<<"possibility"<<endl<<"-----"<<endl;
    for(int i=0; i<first30Words.size(); ++i)
    {
        cout<<first30Words[i].word<<"   "<<first30Words[i].tf<<"    "<<first30Words[i].df<<"    "<<first30Words[i].idf<<"   "<<first30Words[i].tf_idf<<"    "<<first30Words[i].possibility<<endl;
    }
    cout<<"---------------------------"<<endl;
    
    
    cout<<"Q5. average number of words per document:    "<<averageNum<<endl<<endl;
    
    return 0;
}
