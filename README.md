# Overview

I am building a command-line stockroom inventory manager to strengthen my C++ programming skills and practice organizing a small application with classes and standard library containers.

The software lets a user add, list, and search products; update quantities; remove products; and review total units and inventory value. It stores product records in a local text file so inventory is available after the program is closed and reopened.

I wrote this software to practice C++ syntax and core programming concepts in a practical application, including variables, expressions, conditionals, loops, functions, classes, `std::vector`, and file input/output.

[Software Demo Video](https://youtu.be/FiNhDl4q9ms)

# Development Environment

I am developing this project in a C++-capable IDE or editor and compiling it with a C++17 compiler. I will update this section with the specific IDE and compiler version I use.

The program is written in C++17 and uses standard library headers for console input/output, strings, vectors, file streams, formatting, and input validation. It does not use third-party libraries.

# Useful Websites

- [C++ reference: std::vector](https://en.cppreference.com/w/cpp/container/vector.html)
- [C++ reference: file input/output](https://en.cppreference.com/w/cpp/io/basic_fstream.html)
- [C++ reference: std::quoted](https://en.cppreference.com/w/cpp/io/manip/quoted.html)

# Future Work

- Add editing for product names and prices as well as quantities.
- Improve file validation and report which inventory records could not be loaded.
- Add sorting and filtering options for larger inventories.

# Terminal run commands
g++ -std=c++17 -Wall -Wextra -o StockroomInventoryManager.exe main.cpp
./StockroomInventoryManager.exe
