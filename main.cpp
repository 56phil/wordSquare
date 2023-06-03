
/*******************************************************************************
 Make a solutionSizex5 grid with 25 unique chars using words from text file.
 *******************************************************************************/

#include <algorithm>
#include <cassert>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

const int SOLUTION_SIZE(5); // number of letters in word, words in solution

using namespace std::chrono;

typedef unsigned long ul;
typedef std::unordered_map<int, ul> mChar;
typedef std::vector<std::string> vString;
typedef std::unordered_set<std::string> sString;

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
 Word class.
 *******************************************************************************/
class Word {
private:
  int score;
  std::string word;

public:
  Word() { reset(); }
  Word(std::string _word) {
    reset();
    if (_word.size() != SOLUTION_SIZE) {
      throw DataValidationException("Must have exactly five letters.");
    }

    std::unordered_set<int> letters;
    for (auto c : _word) {
      if (!isprint(c)) {
        throw DataValidationException("All characters must be letters.");
      }
      if (letters.count(c) > 0) {
        throw DataValidationException("All letters in word must be unique.");
      }
      letters.insert(c);
    }
    word = _word;
  }

  ~Word() { reset(); }

  void setScore(mChar &fm) {
    score = 0;
    for (auto c : word) {
      score += fm[c];
    }
  }

  void reset() {
    word = "";
    score = 0;
  }

  std::string getWord() const { return word; }
  int getScore() const { return score; }
  // std::unordered_set<int> getLetters() const { return letters; }
};
typedef std::vector<Word> vWord;

/*******************************************************************************
 Solution class.
 *******************************************************************************/
class Solution {
private:
  vWord wordVector; // collection of 0..5 Word objects
  ul score;
  std::unordered_set<int> letters; // all letters in solution

  void removeContentsFromLetters(std::string lttrs) {
    for (auto c : lttrs) {
      letters.erase(c);
    }
  }

public:
  Solution() { reset(); }
  ~Solution() { reset(); }

  bool canAddWord(Word &w) {
    for (auto c : w.getWord()) {
      if (letters.count(c) > 0) {
        return false;
      }
    }
    return wordVector.size() < SOLUTION_SIZE;
  }

  bool isEmpty() { return wordVector.empty(); }

  bool isSolved() { return wordVector.size() == SOLUTION_SIZE; }

  vWord getWordVector() { return wordVector; }

  std::string formatSolution() {
    std::stringstream sst;
    sst << this->score;
    for (auto aWord : this->wordVector) {
      sst << ' ' << aWord.getWord();
    }
    sst << std::endl;
    return sst.str();
  }

  ul getVectorSize() { return wordVector.size(); }

  ul getScore() { return score; }

  void addWord(Word aWord) {
    if (wordVector.size() >= SOLUTION_SIZE) {
      throw SolutionException(" Solution size error.");
    }
    for (auto lttr : aWord.getWord()) {
      letters.insert(lttr);
    }
    wordVector.emplace_back(aWord);
  }

  void removeLastWord() {
    if (wordVector.size() > 0) {
      removeContentsFromLetters(wordVector.back().getWord());
      wordVector.pop_back();
    }
  }

  // void removeWord(Word &aWord) { // lose a word from the vector in a solution
  //   auto itr(wordVector.begin());
  //   while (itr != wordVector.end()) {
  //     if (itr->getWord() == aWord.getWord()) {
  //       break;
  //     }
  //     itr++;
  //   }
  //   if (itr != wordVector.end()) {
  //     removeContentsFromLetters(aWord.getWord());
  //     wordVector.erase(itr); // finally, lose the word
  //   }
  // }

  void reset() {
    letters.clear();
    wordVector.clear();
    score = 0;
  }

  void setScore() {
    score = 0;
    for (auto aWord : wordVector) {
      score += aWord.getScore(); // add word score to solution score
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
ul scoreString(mChar &, const std::string &);
void formatSteadyClockDuration(std::string &, const steady_clock::time_point &,
                               const steady_clock::time_point &,
                               const bool &b = false);
void formatTime(std::string &, bool = true, bool = true);
void getData(const std::string &);
void augmentFreqMap(mChar &, const std::string &);
void initialzation(const int &, char *[], vWord &, mChar &);
void makeLowercase(std::string &str);
void prependFNwithTS(std::string &, std::string &);
void pushCurrentSolutionOnSolutions(const mChar &, vSol &, Solution &,
                                    sString &);
void readWordsFromStorage(const std::string &, vWord &, mChar &);
void termination(const steady_clock::time_point &, vSol &);
void writeResultsToStorage(const vSol &);
void writeWordsWithoutDupeLetters(const std::string &, vWord &);
void recursiveSearch(vWord &, Solution &, vSol &, const mChar &, sString &);

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

  mChar freqMap;       // frequencies of each letter
  vSol solutions;      // all identified solutions
  vWord wordVector;    // Word objects from input file
  sString solutionSet; // fingerprints of previously identified solutions

  freqMap.clear();
  solutions.clear();
  wordVector.clear();
  solutionSet.clear();

  initialzation(argc, argv, wordVector, freqMap);

  Solution currentSolution; // temporary woekspace
  currentSolution.reset();

  if (wordVector.size() > 0) {
    recursiveSearch(wordVector, currentSolution, solutions, freqMap,
                    solutionSet);
    termination(startTime, solutions);
  } else {
    rc = 42;
  }

  return rc;
}

/********************************************************************************
 pushCurrentsolutionOnSolutions
 gets:  mChar address
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

void pushCurrentSolutionOnSolutions(const mChar &freqMap, vSol &solutions,
                                    Solution &currentSolution,
                                    sString &solutionIDSet) {
  std::string ts("");
  std::string solutionID{""};
  for (auto aWord : currentSolution.getWordVector()) { // each word object
    solutionID += aWord.getWord();
  }

  formatTime(ts);
  if (solutionIDSet.count(solutionID) > 0) {
    std::cerr << ts << " \tDupe solution submitted: "
              << currentSolution.formatSolution();
  } else {
    solutionIDSet.insert(solutionID); // save solution ID
    currentSolution.setScore();
    solutions.emplace_back(currentSolution);
    std::cout << ts << std::setw(7) << std::right << solutions.size()
              << ". " << currentSolution.formatSolution();
  }
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

void recursiveSearch(vWord &words, Solution &currentSolution, vSol &solutions,
                     const mChar &freqMap, sString &solutionSet) {
  for (auto &aWord : words) { // foreach word in vector
    if (currentSolution.isSolved()) {
      pushCurrentSolutionOnSolutions(freqMap, solutions, currentSolution,
                                     solutionSet);
      return;
    }
    if (currentSolution.canAddWord(aWord)) {
      currentSolution.addWord(aWord);
      recursiveSearch(words, currentSolution, solutions, freqMap, solutionSet);
      currentSolution.removeLastWord();
    }
  }
}

/********************************************************************************
 augmentFreqMap
 gets: mChar address
       string address
 returns: nothing
 objective: accumulate letter occurances
 method: using each letter from word, adds one to corresponding letter in map
 ********************************************************************************/

void augmentFreqMap(mChar &freqMap, const std::string &aString) {
  for (auto c : aString) {
    freqMap[c]++;
  }
}

/*******************************************************************************
 initialzation
 gets: argc, argv, Word vector address
 returns: nothing (fills in word vector)
 objective: read input file (a bunch of five letter words) and builds the Word
 vector method:  initialize the frequency map open input file for each line
 read: augment the frequency map instanciate a set represnting the W in
 the word if all five letters are unique: instanciate word object (set,
 string, score) emplace object on vector now that the letter frequencies are
 built: traverse the Word vector scoring all words sort the Word objects
 alphabetically or by score the choice is made using a psudo-random number: 0
 no sort preformed 1 word ascending 2 word decending 3 score ascending 4 score
 decending
 ********************************************************************************/

void initialzation(const int &argc, char *argv[], vWord &words,
                   mChar &freqMap) {
  std::string ts("");
  formatTime(ts);

  std::string iFilePath(
      "/Users/prh/code/txt/td/words.txt"); // default file path
  if (argc >= 2) {
    iFilePath = argv[1];
  }

  std::string oFilePath(
      "/home/p/code/txt/out/wordsolutionSize.txt"); // default file path
  if (argc >= 3) {
    oFilePath = argv[2];
  }
  prependFNwithTS(ts, oFilePath);

  std::string dupelessFilePath(""); // no default for dupeless output fp
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

void readWordsFromStorage(const std::string &inFilePath, vWord &words,
                          mChar &freqMap) {
  const std::string alphabet("abcdefghijklmnopqrstuvwxyz");
  std::string element;
  for (char c : alphabet) {
    freqMap[c] = 0;
  }

  std::string ts("");
  int linesRead(0);
  int rejects(0);
  // std::unordered_set<std::string> sString;
  // sString.clear();
  std::ifstream fin(inFilePath);
  if (fin.is_open()) {
    std::unordered_set<std::string> lines;
    while (fin >> element) {
      linesRead++;
      makeLowercase(element);
      lines.insert(element);
      augmentFreqMap(freqMap, element);
    }
    fin.close();

    for (auto &l : lines) {
      try {
        Word tmp(l);
        tmp.setScore(freqMap);
        words.emplace_back(tmp);
      } catch (const DataValidationException &e) {
        rejects++;
        std::cerr << "Data validation error: \t" << l << " \t" << e.what()
                  << std::endl;
      }
    }

    std::sort(words.begin(), words.end(), [](const Word &a, const Word &b) {
      return a.getScore() < b.getScore();
    });

    formatTime(ts);
    std::cout << ts << " \tLines read: " << linesRead
              << " \tValidation errors: " << rejects
              << " \tsolutionSize unique letter words kept: " << words.size()
              << '\n';
  } else {
    formatTime(ts);
    std::cerr << ts << " \t" << inFilePath << " did not open.\n";
  }
}

/*******************************************************************************
 writeWordsWithoutDupeLetters
 gets:  string address
 vWord address
 returns: nothing
 objective: put a list of solutionSize letter words with no duplicate letters to
 storage method:  opens outfile for each word: write a record close file
 ********************************************************************************/

void writeWordsWithoutDupeLetters(const std::string &fp, vWord &words) {
  std::ofstream outputFile(fp);
  std::string ts("");

  if (outputFile.is_open()) {
    for (auto &aWord : words) {
      outputFile << aWord.getWord() << '\n';
    }

    outputFile.close();

    formatTime(ts);
    std::cout << ts << " \t" << words.size()
              << " Strings written to file successfully.\n";
  } else {
    std::cerr << ts << " \tFailed to open " << fp << ".\n";
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

void makeLowercase(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
}
