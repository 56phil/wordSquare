
/*******************************************************************************
 Make a 5x5 grid with 25 unique chars using words from text file.
 *******************************************************************************/

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <ostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std::chrono;

typedef unsigned long ul;
typedef std::map<int, ul> charMap;
typedef std::vector<std::string> vString;

class DataValidationException : public std::exception {
public:
  explicit DataValidationException(const std::string &errorMessage)
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
  std::string word;

public:
  Word(const std::string _word) {
    if (_word.size() != 5) {
      throw DataValidationException(" doesn't have exactly five letters.");
    }
    std::unordered_set<int> tmp;
    for (auto c : _word) {
      tmp.insert(c);
    }
    if (tmp.size() != 5) {
      throw DataValidationException(" doesn't have five unique letters.");
    }
    word = _word;
  }

  const std::string getWord() const { return word; }
};
typedef std::unordered_map<std::string, int> mWord;

/*******************************************************************************
 Solution class.
 *******************************************************************************/
class Solution {
public:
  Solution() { reset(); }

  vString getWords() {
    vString t(words);
    return t;
  }
  void addWord(std::string wrd) {
    words.push_back(wrd);
    for (auto ch : wrd) {
      letters.insert(ch);
    }
  }
  void removeLast() {
    std::string tmp(words.back());
    for (auto ch : tmp) {
      letters.erase(ch);
    }
  }
  void reset() {
    words.clear();
    score = 0;
    letters.clear();
  }
  ul wCount() { return words.size(); }

  std::string formatSolution() {
    std::stringstream sst;
    sst << this->score;
    for (std::string word : this->words) {
      sst << ' ' << word;
    }
    sst << std::endl;
    return sst.str();
  }

private:
  vString words;
  ul score;
  std::unordered_set<int> letters;

  void scoreSolution() {
    score = 0;
    // for (Word word : words) {
    //   score += word.getScore();
    // }
  }
};
typedef std::vector<Solution> vSol;

/*******************************************************************************
 prototypes
 *******************************************************************************/
bool wordOK(Solution &, const std::string &);
int scoreString(const charMap &, const std::string &);
std::string intName(ul);
std::string teenName(ul);
std::string tensName(ul);
std::string textForNumber(ul);
void format_steady_clock_duration(std::string &,
                                  const steady_clock::time_point &,
                                  const steady_clock::time_point &,
                                  const bool &b = false);
void formatTime(std::string &, bool = false, bool = true);
void getData(const std::string &);
void getFreqs(charMap &, const std::string &);
void initialzation(const int &, char *[], mWord &, charMap &);
void prependFNwithTS(std::string &, std::string &);
void readWordsFromStorage(const std::string &, mWord &, charMap &);
void search(mWord &, vSol &);
void termination(const std::chrono::steady_clock::time_point &, const vSol &);
void textForNumberHelper(std::string &, ul);
void writeResultsToStorage(const vSol &);
void writeWordsWithoutDupeLetters(const std::string &, mWord &);
void recursiveSearch(mWord &, Solution, vSol &);

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
  formatTime(ts, true, true);
  std::cout << ts << " \tStarting solve. This will take some time.\n";

  charMap freqMap;
  vSol solutions;
  mWord words;

  initialzation(argc, argv, words, freqMap);

  formatTime(ts, true, true);
  std::cout << ts << " \tSearching " << words.size() << " words.\n";
  Solution currentSolution;

  if (words.size() > 0) {
    recursiveSearch(words, currentSolution, solutions);
    termination(startTime, solutions);
  } else {
    rc = 42;
  }

  return rc;
}

/********************************************************************************
   recursiveSearch
   gets: mWord address
         Solution
         vSol address
   returns: nothing
   objective: find groups of five words where all letters are unique.
   method:  add a string (five letter word) to the current Solution. When there
   are five strings in the current Solution, wive it to the vSol.
   ********************************************************************************/

void recursiveSearch(mWord &words, Solution currentSolution, vSol &solutions) {
  // If the word count of the current solution is 5, then we have found a
  // solution.
  if (currentSolution.wCount() == 5) {
    solutions.push_back(currentSolution);
    currentSolution.reset();
    return;
  }

  for (auto p : words) { // foreach element in map
    if (wordOK(currentSolution, p.first)) {
      currentSolution.addWord(p.first);
      recursiveSearch(words, currentSolution, solutions);
      currentSolution.removeLast();
    }
  }
}

/********************************************************************************
 wordOK
 gets: map address
 string address
 returns: bool
 objective: determin if any letters on currentSolution are present in prospect
 method: test each letter in currentSolution. if one is in the prospect, return
 flase. Otherwise, after all words in currentSolution have been tested, return
 true.
 ********************************************************************************/

bool wordOK(Solution &solution, const std::string &prospect) {
  for (auto str : solution.getWords()) { // foreach word in solution
    for (auto c : str) {  // foreach charecter in word
      if (prospect.find(c) != std::string::npos)
        return false;
    }
  }
  return true;
}

/********************************************************************************
 scoreString
 gets: map address
 string address
 returns: int
 objective: return the score for the given string.
 method: accumulate the rank of each letter. return the sum.
 ********************************************************************************/

int scoreString(charMap &m, const std::string &word) {
  int score = 0;
  for (char c : word) {
    score += m[c];
  }
  return score;
}

/********************************************************************************
 getFreqs
 gets: map address
 string address
 returns: nothing
 objective: accumulates letter occurances
 method: using each letter from word, adds one to corresponding letter in map
 ********************************************************************************/

void getFreqs(charMap &freqMap, const std::string &word) {
  for (char c : word) {
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
                   charMap &freqMap) {
  std::string ts("");
  formatTime(ts, true, true);

  std::string iFilePath(
      "/Users/prh/code/txt/td/tdWords.dat"); // default file path
  if (argc >= 2) {
    iFilePath = argv[1];
  }

  std::string oFilePath(
      "/Users/prh/code/txt/out/wordSolutions.dat"); // default file path
  if (argc >= 3) {
    oFilePath = argv[2];
  }
  prependFNwithTS(ts, oFilePath);

  std::string dupelessFilePath(
      "/Users/prh/code/txt/out/dupelessWordList.txt"); // default file path
  if (argc >= 4) {
    dupelessFilePath = argv[3];
  }
  prependFNwithTS(ts, dupelessFilePath);

  readWordsFromStorage(iFilePath, words, freqMap);
  writeWordsWithoutDupeLetters(dupelessFilePath, words);
}

void prependFNwithTS(std::string &ts, std::string &fp) {
  ts += "-";
  auto p(fp.find_last_of("/"));
  if (p == std::string::npos) {
    fp = ts + "-" + fp;
  } else {
    auto tmp1(fp.substr(0, p + 1));
    auto tmp2(fp.substr(p + 1));
    fp = tmp1 + ts + tmp2;
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
            instanciate a Word object
            emplace the new object in the Word vector.
          close file
          for each word:
            set the score using the corrent frequency map
 ********************************************************************************/

void readWordsFromStorage(const std::string &inFilePath, mWord &words,
                          charMap &freqMap) {
  const std::string alphabet("abcdefghijklmnopqrstuvwxyz");
  std::string element;
  for (char c : alphabet) {
    freqMap[c] = 0;
  }

  int cntr1(0);
  int cntr2(0);
  std::ifstream fin(inFilePath.c_str());
  if (fin.is_open()) {
    while (fin >> element) {
      cntr1++;
      getFreqs(freqMap, element);
      try {
        Word tmp(element);
        words[element] = 0;
      } catch (const DataValidationException &e) {
        cntr2++;
        std::cerr << "Data validation error: " << element << e.what()
                  << std::endl;
      }
    }
    fin.close();
    std::string ts{""};
    formatTime(ts, true, true);
    std::cout << ts << " \t" << cntr1 << " lines read.\t" << words.size()
              << " five letter words ready for algorithm.\n";

    for (auto &pair : words) {
      pair.second = scoreString(freqMap, pair.first);
    }
  }
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
  std::ofstream outputFile(fp.c_str());

  if (outputFile.is_open()) {
    for (const auto &pair : words) {
      outputFile << pair.first << '\n';
    }

    outputFile.close(); // Close the file
    std::cout << "Strings written to file successfully." << std::endl;
  } else {
    std::cerr << "Failed to open the file." << std::endl;
  }
}

/*******************************************************************************
 writeResultsToStorage
 gets: vSol address
 returns: nothing
 objective: put the solutions out to storage
 method:  builds file name
          opens outfile
          for each solution:
            calculate solution score
            write a record
          close file
 ********************************************************************************/

void writeResultsToStorage(const vSol &solutions) {
  std::string ts;
  formatTime(ts, true, true);
  ts += "results.txt";
  std::ofstream ofx(ts.c_str());
  for (auto solution : solutions) {
    ofx << solution.formatSolution();
  }
  ofx << std::endl;
  ofx.close();
}

/*******************************************************************************
 format_steady_clock_duration
 gets:  string
        time_point
        time_point
 returns: nothing
 objective: The string is filled in with the two given time points
 ********************************************************************************/

void format_steady_clock_duration(std::string &durStr,
                                  const steady_clock::time_point &start,
                                  const steady_clock::time_point &end,
                                  const bool &fancy) {
  auto dur(end - start);
  auto durationInSeconds(duration_cast<seconds>(dur).count());
  const long secsInDay(3600 * 24);
  const long secsInHour(3600);
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
        << (durationInSeconds > 1 ? "s." : ".");
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

void termination(const steady_clock::time_point &startTime,
                 const vSol &vResults) {
  std::string ts("");

  writeResultsToStorage(vResults);

  auto stopTime = steady_clock::now();
  formatTime(ts, true, true);
  std::string durStr("");
  format_steady_clock_duration(durStr, startTime, stopTime, true);
  std::cout << '\n' << ts << " \t" << durStr << '\n';

  formatTime(ts, true, true);
  std::cout << ts << " \tString search complete. Solutions identified: "
            << vResults.size() << ".\n";
}

/*******************************************************************************
 formatTime
 gets: string address
 bool (date switch)
 bool (time switch)
 returns: nothing (fills in tms)
 objective: provide formatted date or time
 method: calls textForNumber
 ********************************************************************************/

void formatTime(std::string &tms, bool doDate, bool doTime) {
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[256];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  std::stringstream sst;

  if (doDate) {
    strftime(buffer, 80, "%Y-%m-%d", timeinfo);
    sst << buffer;
    if (doTime)
      sst << "T";
  }

  if (doTime) {
    strftime(buffer, 80, "%H_%M_%S", timeinfo);
    sst << buffer;
  }
  tms = sst.str();
}

/*******************************************************************************
 textForNumber
 gets: number (unsigned long)
 returns: nothing (recursive)
 objective: convert the input parameter to text
 method: recursivly call for each digit.
 ********************************************************************************/

std::string textForNumber(ul n) {

  std::vector<std::string> const ones{"",     "one", "two",   "three", "four",
                                      "five", "six", "seven", "eight", "nine"};
  std::vector<std::string> const teens{
      "ten",     "eleven",  "twelve",    "thirteen", "fourteen",
      "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
  std::vector<std::string> const tens{"",       "",      "twenty", "thirty",
                                      "forty",  "fifty", "sixty",  "seventy",
                                      "eighty", "ninety"};
  if (n < 10) {
    return ones[n];
  } else if (n < 20) {
    return teens[n - 10];
  } else if (n < 100) {
    return tens[n / 10] + ((n % 10 != 0) ? " " + textForNumber(n % 10) : "");
  } else if (n < 1'000) {
    return textForNumber(n / 100) + " hundred" +
           ((n % 100 != 0) ? " " + textForNumber(n % 100) : "");
  } else if (n < 1'000'000) {
    return textForNumber(n / 1'000) + " thousand" +
           ((n % 1000 != 0) ? " " + textForNumber(n % 1000) : "");
  } else if (n < 1'000'000'000) {
    return textForNumber(n / 1'000'000) + " million" +
           ((n % 1'000'000 != 0) ? " " + textForNumber(n % 1'000'000) : "");
  } else if (n < 1'000'000'000'000) {
    return textForNumber(n / 1'000'000'000) + " billion" +
           ((n % 1'000'000'000 != 0) ? " " + textForNumber(n % 1'000'000'000)
                                     : "");
  }
  return "error";
}

/*******************************************************************************
 textForNumberHelper
 gets: number (unsigned long),
 string
 returna: string
 objective: provide interface between callar and textForNumber
 method: calls textForNumber
 ********************************************************************************/

void textForNumberHelper(std::string &text, ul digits) {
  text = textForNumber(digits);
}
