# C++ Program: Unique Character Grid

This C++ program generates a 5x5 grid with 25 unique characters using words from a text file. It uses a combination of nested loops and set operations to find groups of five words where all letters are unique. The program calculates the score for each solution based on the letter ranks and outputs the solutions in a formatted manner.

### Program Structure

The program is structured into several classes and functions:

#### Word Class
The `Word` class represents a word and contains the following members:
- `score`: The sum of letter ranks.
- `letters`: An unordered set of integers representing the unique letters in the word.
- `word`: The string representation of the word.

The `Word` class provides methods to retrieve the letters, word, score, and set size. It also calculates the score based on a given character map.

#### Solution Class
The `Solution` class represents a solution consisting of five words. It contains the following members:
- `solutionList`: An array of `Word` objects representing the five words.
- `score`: The total score of the solution.

The `Solution` class provides a method to format the solution as a string.

#### Other Functions
The program also includes several helper functions for set operations, formatting time, reading input files, searching for solutions, and writing results to storage.

### Usage

To use the program, follow these steps:

1. Compile the C++ program using a C++ compiler.
2. Run the compiled program.
3. The program will read a text file containing a list of five-letter words. You can specify the file path as a command-line argument.
4. The program will generate unique character grids by finding groups of five words where all letters are unique.
5. The program will output the solutions in a formatted manner.
6. The solutions will be written to a file named "results.txt".

### Example

Here is an example of how to run the program:

```
$ ./unique_character_grid program_input.txt
```

This command will run the program using the input file "program_input.txt" to generate unique character grids. The solutions will be displayed on the console and written to the "results.txt" file.

### Dependencies

The program requires the following C++ libraries:

- `<algorithm>`
- `<array>`
- `<chrono>`
- `<fstream>`
- `<iomanip>`
- `<iostream>`
- `<map>`
- `<ostream>`
- `<random>`
- `<sstream>`
- `<unordered_set>`
- `<vector>`

### Notes

- The program uses a pseudorandom number generator to determine the sorting option for the word list if no command-line argument is provided.
- The program assumes that the input file contains a list of five-letter words, with each word on a separate line.

Feel free to explore the source code for more details and customization options.
