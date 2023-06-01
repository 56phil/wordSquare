
/*******************************************************************************
 Make a 5x5 grid with 25 unique chars using words from text file.
 *******************************************************************************/

#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std::chrono;

typedef unsigned long ul;
typedef std::pair<std::string, int> sip;   // string/int pair
typedef std::unordered_map<int, ul> mChar;
typedef std::unordered_map<std::string, ul> mWord;
typedef std::vector<std::string> vString;
typedef std::unordered_set<std::string> sSol;

class DataValidationException : public std::exception {
public:
  explicit DataValidationException(const std::string &errorMessage)
      : errorMessage(errorMessage) {}

  const char *what() const noexcept override { return errorMessage.c_str(); }

private:
  std::string errorMessage;
};

class SolutionException : public std::exception {
public:
  explicit SolutionException(const std::string &errorMessage)
      : errorMessage(errorMessage) {}

  const char *what() const noexcept override { return errorMessage.c_str(); }

private:
  std::string errorMessage;
};

/*******************************************************************************
 Word class. used only for validation of input.
 *******************************************************************************/
class Word {
private:
public:
  Word(const std::string _word) {
    if (_word.size() != 5) {
      throw DataValidationException("Must have exactly five letters.");
    }
    std::unordered_set<int> tmp;
    for (auto c : _word) {
      if (!isalpha(c)) {
        throw DataValidationException("All characters must be letters.");
      }
      if (tmp.count(c) == 1) {
        throw DataValidationException("All letters in word must be unique.");
      }
      tmp.insert(c);
    }
  }
};

/*******************************************************************************
 Solution class.
 *******************************************************************************/
class Solution {
private:
  mWord wordmap;
  ul score;

public:
  Solution() { reset(); }
  ~Solution() { reset(); }

  bool canAddWord(std::string prospect) {
    for (auto p : wordmap) { // foreach sip in map
      for (auto c : p.first) {  // foreach char in string
        if (prospect.find(c) != std::string::npos) {  // letter already there?
          return false; // all letters in solution must be unique
        }
      }
    }
    return true; // prospect is promising
  }

  bool isEmpty() { return wordmap.empty(); }

  bool isSolved() { return wordmap.size() == 5; }

  mWord getWordMap() { return wordmap; }

  std::string formatSolution() {
    std::stringstream sst;
    sst << this->score;
    for (auto p : this->wordmap) {
      sst << ' ' << p.first;
    }
    sst << std::endl;
    return sst.str();
  }

  ul getMapSize() { return wordmap.size(); }

  ul getScore() { return score; }

  void addWord(sip item) {
    if (wordmap.size() < 5) {
      wordmap[item.first] = item.second;
    } else {
      throw SolutionException(" Map size error.");
    }
  }

  void removeItem(sip p) { wordmap.erase(p.first); }

  void reset() {
    wordmap.clear();
    score = 0;
  }

  void setScore() {
    score = 0;
    for (auto p : wordmap) {
      score += p.second; // add word score to solution score
    }
  }
};
typedef std::vector<Solution> vSol;

/*******************************************************************************
 prototypes
 *******************************************************************************/
std::string intName(ul);
std::string teenName(ul);
std::string tensName(ul);
ul pushCurrentSolutionOnSolutions(const mChar &, vSol &, Solution &, sSol &);  
ul scoreString(mChar &, const std::string &);
void formatSteadyClockDuration(std::string &, const steady_clock::time_point &,
                               const steady_clock::time_point &,
                               const bool &b = false);
void formatTime(std::string &, bool = true, bool = true);
void getData(const std::string &);
void getFreqs(mChar &, const std::string &);
void initialzation(const int &, char *[], mWord &, mChar &);
void prependFNwithTS(std::string &, std::string &);
void readWordsFromStorage(const std::string &, mWord &, mChar &);
void termination(const steady_clock::time_point &, vSol &);
void writeResultsToStorage(const vSol &);
void writeWordsWithoutDupeLetters(const std::string &, mWord &);
void recursiveSearch(mWord &, Solution &, vSol &, const mChar &, sSol &);

/********************************************************************************
 main
 gets: argc (unused)
 argv (unused)
 returns: int
 objective:
 method:
 ********************************************************************************/
int main(int argc, char *argv[]) {
  int rc(0);
  auto startTime = steady_clock::now();
  std::string ts("");
  formatTime(ts);
  std::cout << ts << " \tStarting solve. This will take some time.\n";

  mChar freqMap;
  vSol solutions;
  mWord wordMap;
  sSol solutionSet;

  freqMap.clear();
  solutions.clear();
  wordMap.clear();
  solutionSet.clear();

  initialzation(argc, argv, wordMap, freqMap);

  Solution currentSolution;

  if (wordMap.size() > 0) {
    recursiveSearch(wordMap, currentSolution, solutions, freqMap, solutionSet);
    termination(startTime, solutions);
  } else {
    rc = 42;
  }

  return rc;
}

/********************************************************************************
 pushCurrentsolutionOnSolutions
 gets:  mWord address
        Solution address
        vSol address
        mChar address
        mWord address
        sSol address
 returns: nothing
 objective: 1) score currentSolution 2) put on the vector 3) display solution
            4) get current solution ready for next one.
 method:  add a string (five letter word) to the current Solution. When there
 are five strings in the current Solution, give it to the vSol.
 ********************************************************************************/

ul pushCurrentSolutionOnSolutions(const mChar &freqMap, 
                                  vSol &solutions, Solution &currentSolution,
                                  sSol &solutionSet) {
  std::string tString{""};
  for (auto p : currentSolution.getWordMap()) { // each word/score pair in map
    tString += p.first;   // make one 25 char string
  }

  std::string ts("");
  formatTime(ts);
  solutionSet.insert(tString);  // try adding to set
  if (solutionSet.count(tString) == 1) {
    std::cerr << ts << " \tDupe solution submitted: "
              << currentSolution.formatSolution();
  } else {
    currentSolution.setScore();
    solutions.emplace_back(currentSolution);
    std::cout << ts << " \tn: " << solutions.size() << ". "
              << currentSolution.formatSolution();
  }

  currentSolution.reset();
  return solutions.size();
}

/********************************************************************************
 recursiveSearch
 gets:  mWord address   all words that may form a solution 
        Solution        current solution 
        vSol address    solution vector 
        sSol address    one element foreach solution 
 returns: nothing
 objective: find groups of five words where all letters are unique.
 method:  add a string (five letter word) to the current Solution. When there
 are five strings in the current Solution, wive it to the vSol.
 ********************************************************************************/

void recursiveSearch(mWord &words, Solution &currentSolution, vSol &solutions,
                     const mChar &freqMap, sSol &solutionSet) {
  // If the word count of the current solution is 5, then we have found a
  // solution.
  if (currentSolution.isSolved()) {
    pushCurrentSolutionOnSolutions(freqMap, solutions, currentSolution,
                                   solutionSet);
    return;
  }

  for (auto p : words) { // foreach element in map
    if (currentSolution.canAddWord(p.first)) {
      currentSolution.addWord(p);
      recursiveSearch(words, currentSolution, solutions, freqMap, solutionSet);
      currentSolution.removeItem(p);
    }
  }
}

/********************************************************************************
 getFreqs
 gets: mChar address
          string address
 returns: nothing
 objective: accumulates letter occurances
 method: using each letter from word, adds one to corresponding letter in map
 ********************************************************************************/

void getFreqs(mChar &freqMap, const std::string &word) {
  for (auto c : word) {
    freqMap[c]++;
  }
}

/*******************************************************************************
 initialzation
 gets: argc, argv, Word vector address
 returns: nothing (fills in word vector)
 objective: read input file (a bunch of five letter words) and builds the Word
 vector method:  initialize the frequency map open input file for each line
 read: augment the frequency map instanciate a set represnting the letters in
 the word if all five letters are unique: instanciate word object (set,
 string, score) emplace object on vector now that the letter frequencies are
 built: traverse the Word vector scoring all words sort the Word objects
 alphabetically or by score the choice is made using a psudo-random number: 0
 no sort preformed 1 word ascending 2 word decending 3 score ascending 4 score
 decending
 ********************************************************************************/

void initialzation(const int &argc, char *argv[], mWord &words,
                   mChar &freqMap) {
  std::string ts("");
  formatTime(ts);

  std::string iFilePath("/home/p/code/txt/td/words.txt"); // default file path
  if (argc >= 2) {
    iFilePath = argv[1];
  }

  std::string oFilePath("/home/p/code/txt/out/word5.txt"); // default file path
  if (argc >= 3) {
    oFilePath = argv[2];
  }
  prependFNwithTS(ts, oFilePath);

  std::string dupelessFilePath(""); // no default for dupeless 5-letter words
  if (argc >= 4) {
    dupelessFilePath = argv[3];
  }

  readWordsFromStorage(iFilePath, words, freqMap);

  if (argc >= 4) {
    prependFNwithTS(ts, dupelessFilePath);
    writeWordsWithoutDupeLetters(dupelessFilePath, words);
  }
}

/*******************************************************************************
 prependFNwithTS
 gets:  string address
        string address
 returns: nothing
 objective: make time stamp first part of file name.
 method: find position of filename
         when entire string os file name, place time stamp in front of file name
         otherwise, insert timestamp after last '/'
 ********************************************************************************/

void prependFNwithTS(std::string &ts, std::string &fp) {
  auto p(fp.find_last_of("/"));
  if (p == std::string::npos) {
    fp = ts + "-" + fp;
  } else {
    fp = fp.substr(0, p + 1) + ts + "-" + fp.substr(p + 1);
  }
}

/*******************************************************************************
 readWordsFromStorage
 gets:  string address
 vWord address
 returns: nothing
 objective: get word list from storage
 fill the vector with new Word objects
 method:  initializes frequency map
 opens input file
 for each line:
   call Word consructor to validate word
   insert valid word into unordered set
 close file
 for each word:
   set the score using the frequency map
   insert key/value (word/score) pair into words
 ********************************************************************************/

void readWordsFromStorage(const std::string &inFilePath, mWord &words,
                          mChar &freqMap) {
  std::string ts("");
  const std::string alphabet("abcdefghijklmnopqrstuvwxyz");
  std::string element;
  for (char c : alphabet) {
    freqMap[c] = 0;
  }

  int linesRead(0);
  int rejects(0);
  std::unordered_set<std::string> nd;
  nd.clear();
  std::ifstream fin(inFilePath);
  if (fin.is_open()) {
    while (fin >> element) {
      linesRead++;
      getFreqs(freqMap, element);
      try {
        Word tmp(element);
        nd.insert(element);
      } catch (const DataValidationException &e) {
        rejects++;
        std::cerr << "Data validation error: \t" << element << " \t" << e.what()
                  << std::endl;
      }
    }
    fin.close();
    formatTime(ts);
    std::cout << ts << " \tLines read: " << linesRead
              << " \tValidation errors: " << rejects
              << " \t5 unique letter words kept: " << nd.size() << '\n';

    for (auto &s : nd) {
      words[s] = scoreString(freqMap, s);
    }
  } else {
    formatTime(ts);
    std::cerr << ts << " \t" << inFilePath << " did not open.\n";
  }
}

/********************************************************************************
 scoreString
 gets: map address
 string address
 returns: int
 objective: return the score for the given string.
 method: accumulate the rank of each letter. return the sum.
 ********************************************************************************/

ul scoreString(mChar &m, const std::string &word) {
  ul tmp = 0;
  for (auto c : word) {
    tmp += m[c];
  }
  return tmp;
}

/*******************************************************************************
 writeWordsWithoutDupeLetters
 gets:  string address
 vWord address
 returns: nothing
 objective: put a list of 5 letter words with no duplicate letters to storage
 method:  opens outfile
 for each word:
 write a record
 close file
 ********************************************************************************/

void writeWordsWithoutDupeLetters(const std::string &fp, mWord &words) {
  std::ofstream outputFile(fp);
  std::string ts("");

  if (outputFile.is_open()) {
    for (const auto &pair : words) {
      outputFile << pair.first << '\n';
    }

    outputFile.close();

    formatTime(ts);
    std::cout << ts << " \t" << words.size()
              << " \nStrings written to file successfully.\n";
  } else {
    std::cerr << ts << " \tFailed to open the file.\n";
  }
}

/*******************************************************************************
 writeResultsToStorage
 gets: vSol address
 returns: nothing
 objective: put solutions out to storage
 method:  builds file name
 opens outfile
 for  each solution:
      calculate solution score
      write a record
 close file
 ********************************************************************************/

void writeResultsToStorage(const vSol &solutions) {
  std::string ts;
  formatTime(ts);
  ts += "results.txt";
  std::ofstream ofx(ts);
  for (auto solution : solutions) {
    ofx << solution.formatSolution();
  }
  ofx << std::endl;
  ofx.close();
}

/*******************************************************************************
 formatSteadyClockDuration
 gets:  string
 time_point
 time_point
 returns: nothing
 objective: The string is filled in with the two given time points
 ********************************************************************************/

void formatSteadyClockDuration(std::string &durStr,
                               const steady_clock::time_point &start,
                               const steady_clock::time_point &end,
                               const bool &fancy) {
  auto dur(end - start);
  auto durationInSeconds(duration_cast<seconds>(dur).count());
  const int secsInDay(86'400);
  const int secsInHour(3'600);
  long days(durationInSeconds / (secsInDay));
  durationInSeconds -= days * secsInDay;
  long hours(durationInSeconds / secsInHour);
  durationInSeconds -= hours * secsInHour;
  long mins(durationInSeconds / 60);
  durationInSeconds -= mins * 60;
  std::stringstream sst;
  if (!fancy) {
    sst << days << 'd' << std::setw(2) << std::setfill('0') << hours << ':'
        << std::setw(2) << mins << ':' << std::setw(2) << durationInSeconds;
  } else {
    sst << "Job took ";
    if (days > 0) {
      sst << days << " day" << (days > 1 ? "s " : " ");
    }

    if (hours > 0) {
      sst << hours << " hour" << (hours > 1 ? "s " : " ");
    }

    if (mins > 0) {
      sst << mins << " minute" << (mins > 1 ? "s and " : " and ");
    }

    sst << durationInSeconds << " second"
        << (durationInSeconds == 1 ? "." : "s.");
  }

  durStr = sst.str();
}

/*******************************************************************************
 termination
 gets:  string (scratch)
 vSol (solutionsg)
 returns: nothing
 objective: do whatever needs to be done to wrap things up.
 method: results get written to storage
 alerts operator that this job ended normally.
 ********************************************************************************/

void termination(const steady_clock::time_point &startTime, vSol &solutions) {
  writeResultsToStorage(solutions);
  auto stopTime = steady_clock::now();
  std::string durStr("");
  formatSteadyClockDuration(durStr, startTime, stopTime, true);
  std::string ts("");
  formatTime(ts);
  std::cout << '\n' << ts << " \t" << durStr << '\n';

  formatTime(ts);
  std::cout << ts << " \tString search complete. Solutions identified: "
            << solutions.size() << ".\n";
}

/*******************************************************************************
 formatTime
 gets: string address
 bool (date switch)
 bool (time switch)
 returns: nothing (fills in tms)
 objective: provide formatted date or time
 method:
 ********************************************************************************/

void formatTime(std::string &tms, bool doDate, bool doTime) {
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[256];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  std::stringstream sst;

  if (doDate) {
    strftime(buffer, 80, "%Y%m%d", timeinfo);
    sst << buffer;
    if (doTime)
      sst << "T";
  }

  if (doTime) {
    strftime(buffer, 80, "%H%M%S", timeinfo);
    sst << buffer;
  }
  tms = sst.str();
}
