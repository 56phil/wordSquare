
/*******************************************************************************
 Make a 5x5 grid with 25 unique chars using words from text file.
 *******************************************************************************/

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <ostream>
#include <random>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace std::chrono;

typedef unsigned long ul;
typedef std::map<int, ul> charMap;

/*******************************************************************************
 Word class.
 *******************************************************************************/
class Word {
private:
  ul score; // sum of letter ranks
  std::unordered_set<int> letters;
  std::string word;

  void setLetters() {
    letters.clear();
    for (char c : word) {
      letters.insert(c);
    }
  }

public:
  Word(const std::string _word) {
    score = 0;
    word = _word;
    setLetters();
  }

  std::unordered_set<int> getLetters() const { return letters; }
  const std::string getWord() const { return word; }
  ul getScore() const { return score; }
  ul getSetSize() { return letters.size(); }

  void setScore(charMap m) {
    score = 0;
    for (auto i : word) {
      score += m[i];
    }
  }
};

/*******************************************************************************
 Solution class.
 *******************************************************************************/
class Solution {
public:
  Solution(Word a, Word b, Word c, Word d, Word e)
      : solutionList{{a, b, c, d, e}} {
    scoreSolution();
  }

  std::string formatSolution() {
    std::stringstream sst;
    sst << this->score;
    for (auto w : this->solutionList) {
      sst << ' ' << w.getWord();
    }
    sst << std::endl;
    return sst.str();
  }

private:
  std::array<Word, 5> solutionList;
  ul score;

  void scoreSolution() {
    score = 0;
    for (auto w : solutionList) {
      score += w.getScore();
    }
  }
};

typedef std::vector<Solution> vSol;

/*******************************************************************************
 prototypes
 *******************************************************************************/
bool check2Sets(const std::unordered_set<int>, const std::unordered_set<int>);
bool check3Sets(const std::unordered_set<int>, const std::unordered_set<int>,
                const std::unordered_set<int>);
bool check4Sets(const std::unordered_set<int>, const std::unordered_set<int>,
                const std::unordered_set<int>, const std::unordered_set<int>);
std::string intName(ul);
std::string teenName(ul);
std::string tensName(ul);
std::string textForNumber(ul);
void check5Words(const Word &, const Word &, const Word &, const Word &,
                 const Word &, vSol &);
void format_steady_clock_duration(std::string &,
                                  const steady_clock::time_point &,
                                  const steady_clock::time_point &,
                                  const bool &b = false);
void formatTime(std::string &, bool = false, bool = true);
void getFreqs(charMap &, const std::string &);
void initilzation(const int &, char *[], std::vector<Word> &v);
void search(std::vector<Word> &, vSol &);
void termination(const std::chrono::steady_clock::time_point &, const vSol &);
void textForNumberHelper(std::string &, ul);
void writeResultsToStorage(const vSol &);

/********************************************************************************
 main
 gets: argc (unused)
 argv (unused)
 returns: int
 objective:
 method:
 ********************************************************************************/
int main(int argc, char *argv[]) {
  auto startTime = steady_clock::now();
  std::string ts("");
  formatTime(ts, true, true);
  std::cout << ts << " \tStarting solve. This will take some time.\n";

  std::vector<Word> vWords;
  vSol vResults;

  initilzation(argc, argv, vWords);

  formatTime(ts, true, true);
  std::cout << ts << " \tSearching " << vWords.size() << " words.\n";
  search(vWords, vResults);

  termination(startTime, vResults);

  return 0;
}

/********************************************************************************
   search
   gets: Word Vector address
   Solution vector address
   returns: nothing
   objective: find groups of five Words where all letters are unique.
   method: use five nested loops, pruning dupes as early as possible, to find
   five words where all letters are unique.
   ********************************************************************************/

void search(std::vector<Word> &vWords, vSol &vResults) {
  steady_clock::time_point t0(steady_clock::now());
  steady_clock::time_point t1(steady_clock::now());
  vResults.clear();
  std::string ts("");
  for (auto ita(vWords.begin()); ita != vWords.end(); ita++) {
    const auto a(ita->getLetters());
    t1 = steady_clock::now();
    format_steady_clock_duration(ts, t0, t1);
    t0 = steady_clock::now();
    std::cout << std::setw(12) << ts << std::setw(6) << ita->getWord();
    std::cout.flush();
    for (auto itb(ita + 1); itb != vWords.end(); itb++) {
      const auto b(itb->getLetters());
      if (check2Sets(b, a)) {
        for (auto itc(itb + 1); itc != vWords.end(); itc++) {
          const auto c(itc->getLetters());
          if (check3Sets(a, b, c)) {
            for (auto itd(itc + 1); itd != vWords.end(); itd++) {
              const auto d(itd->getLetters());
              if (check2Sets(d, c) && check2Sets(d, b) && check2Sets(d, a)) {
                for (auto ite(itd + 1); ite != vWords.end(); ite++) {
                  check5Words(*ita, *itb, *itc, *itd, *ite, vResults);
                }
              }
            }
          }
        }
      }
    }
  }
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

/********************************************************************************
 check2Sets
 gets: 2 * set<int>  &
 set<int>  &
 returns: bool
 objective: test that all elements of both sets are unique
 method: merges two sets then compares sizes
 ********************************************************************************/

bool check2Sets(const std::unordered_set<int> a,
                const std::unordered_set<int> b) {
  std::unordered_set<int> tmp1(a);
  std::unordered_set<int> tmp2(b);
  tmp1.merge(tmp2);
  return tmp1.size() == (a.size() + b.size());
}

/********************************************************************************
 check3Sets
 gets: 3 * set<int>  &
 set<int>  &
 returns: bool
 objective: test that all elements of each set are unique
 method: merges each set then compares sizes
 ********************************************************************************/

bool check3Sets(const std::unordered_set<int> a,
                const std::unordered_set<int> b,
                const std::unordered_set<int> c) {
  std::unordered_set<int> tmp1(a);
  std::unordered_set<int> tmp2(b);
  std::unordered_set<int> tmp3(c);
  tmp1.merge(tmp2);
  tmp1.merge(tmp3);
  return tmp1.size() == (a.size() + b.size()) + c.size();
}

/********************************************************************************
 check4Sets
 gets: 4 * set<int>  &
 set<int>  &
 returns: bool
 objective: test that all elements of each set are unique
 method: merges each set then compares sizes
 ********************************************************************************/

bool check4Sets(const std::unordered_set<int> a,
                const std::unordered_set<int> b,
                const std::unordered_set<int> c,
                const std::unordered_set<int> d) {
  std::unordered_set<int> tmp1(a);
  std::unordered_set<int> tmp2(b);
  std::unordered_set<int> tmp3(c);
  std::unordered_set<int> tmp4(d);
  tmp1.merge(tmp2);
  tmp1.merge(tmp3);
  tmp1.merge(tmp4);
  return tmp1.size() == (a.size() + b.size()) + c.size() + d.size();
}

/********************************************************************************
 check5Sets
 gets:  five word object addresses
        solution vector (vSol) address
 returns: nothing
 objective: test that all letters in all word objects are unique
 method: merges all letters of given word objects into a single set. if the
 count is 25 (5x5) the tested Word objects are placed in a Solution object
 which goes to in the vSol.
 ********************************************************************************/

void check5Words(const Word &a, const Word &b, const Word &c, const Word &d,
                 const Word &e, vSol &solutions) {
  auto e0(e.getLetters());
  auto d0(d.getLetters());
  auto c0(c.getLetters());
  auto b0(b.getLetters());
  auto a0(a.getLetters());
  a0.merge(b0);
  a0.merge(c0);
  a0.merge(d0);
  a0.merge(e0);
  if (a0.size() ==
      a.getLetters().size() + b0.size() + c0.size() + d0.size() + e0.size()) {
    Solution tmp(a, b, c, d, e);
    solutions.emplace_back(tmp);

    std::string ts("");
    formatTime(ts, true, true);
    std::cout << '\n' << ts << " \t" << tmp.formatSolution() << '\n';
  }
}

/*******************************************************************************
 initilzation
 gets: argc, argv, Word vector address
 returns: nothing (fills in word vector)
 objective: read input file (a bunch of five letter words) and builds the Word
 vector method:  initilize the frequency map open input file for each line
 read: augment the frequency map instanciate a set represnting the letters in
 the word if all five letters are unique: instanciate word object (set,
 string, score) emplace object on vector now that the letter frequencies are
 built: traverse the Word vector scoring all words sort the Word objects
 alphabetically or by score the choice is made using a psudo-random number: 0
 no sort preformed 1 word ascending 2 word decending 3 score ascending 4 score
 decending
 ********************************************************************************/

void initilzation(const int &argc, char *argv[], std::vector<Word> &vWords) {
  int switchOn(0); // default sort option (none)
  std::string filePath("/Users/prh/code/txt/tdWords.dat"); // default file path
  if (argc >= 3) {
    filePath = argv[2];
  }
  const std::string alphabet("abcdefghijklmnopqrstuvwxyz");
  charMap freqMap;
  std::string element;
  for (char c : alphabet) {
    freqMap[c] = 0;
  }

  std::ifstream fin(filePath.c_str());
  while (fin >> element) {
    getFreqs(freqMap, element);
    Word tmp(element);
    if (tmp.getSetSize() == element.size()) {
      vWords.emplace_back(tmp);
    }
  }
  fin.close();

  for (auto &w : vWords) {
    w.setScore(freqMap);
  }

  std::sort(vWords.begin(), vWords.end(),
            [](Word &a, Word &b) { return a.getWord() > b.getWord(); });
  auto uniqueEnd = std::unique(vWords.begin(), vWords.end(),
                               [](const Word &obj1, const Word &obj2) {
                                 return obj1.getWord() == obj2.getWord();
                               });
  vWords.erase(uniqueEnd, vWords.end());

  if (argc >= 2)
    switchOn = atoi(argv[1]);
  else {
    // Create a random device to seed the random number generator
    std::random_device rd;

    // Create a random engine using the random device as the seed
    std::mt19937 engine(rd());

    // Define the range for the random integer
    int minRange = 0;
    int maxRange = 4;

    // Create a uniform distribution over the range
    std::uniform_int_distribution<int> dist(minRange, maxRange);
    switchOn = dist(engine);
  }

  std::string ts("");
  formatTime(ts, true, true);

  switch (switchOn) {
  case 1:
    std::sort(vWords.begin(), vWords.end(),
              [](Word &a, Word &b) { return a.getWord() > b.getWord(); });
    std::cout << ts << " \tSorted words alphabetically, descending.\n";
    break;

  case 2:
    std::sort(vWords.begin(), vWords.end(),
              [](Word &a, Word &b) { return a.getWord() < b.getWord(); });
    std::cout << ts << " \tSorted words alphabetically, ascending.\n";
    break;

  case 3:
    std::sort(vWords.begin(), vWords.end(),
              [](Word &a, Word &b) { return a.getScore() < b.getScore(); });
    std::cout << ts << " \tSorted words by score, ascending.\n";
    break;

  case 4:
    std::sort(vWords.begin(), vWords.end(),
              [](Word &a, Word &b) { return a.getScore() > b.getScore(); });
    std::cout << ts << " \tSorted words by score, descending.\n";
    break;

  default:
    std::mt19937 rng;
    std::shuffle(vWords.begin(), vWords.end(), rng);
    std::cout << ts << " \tShuffled word list.\n";
    break;
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

void writeResultsToStorage(const vSol &vResults) {
  std::string ts;
  formatTime(ts, true, true);
  ts += "results.txt";
  std::ofstream ofx(ts.c_str());
  for (auto solution : vResults) {
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
  std::cout << ts << " \tString search complete. Solutions listed: "
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
    strftime(buffer, 80, "%H:%M:%S", timeinfo);
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