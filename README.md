
# Catalog Placement Assignment: Polynomial Solver

This project is a C++ implementation to solve a polynomial interpolation problem. The program reads a set of points `(x, y)` from a JSON file, where the `y` values are encoded in various number bases. It then uses Lagrange Interpolation to calculate the polynomial's constant term `c`, which is the value of the polynomial at `x=0`.

---

## 🛠️ Core Concepts

* **Lagrange Interpolation:** The core mathematical algorithm used to find `P(0)` without needing to solve for all polynomial coefficients explicitly.
* **JSON Parsing:** Uses the `nlohmann/json` single-header library to parse the input test cases.
* **Large Number Arithmetic:** Implements a `BigInt` library to handle integer values that exceed the limits of standard C++ types like `long long`.
* **Multi-file C++ Compilation:** The project requires compiling and linking multiple source files.

---

## 📁 Project Structure

The repository contains the following files:

* `solve.cpp`: The main application file containing the logic for reading the input, running the interpolation, and printing the result.
* `BigInt.h`: The header file for the large number library.
* `BigInt.cpp`: The implementation file for the large number library.
* `json.hpp`: A single-header library for parsing JSON in C++.
* `input.json`: The input data file containing the points for a given test case.

---

## 🚀 How to Build and Run

### **Prerequisites**

* A C++ compiler that supports C++11 (like `g++` or `clang++`).

### **Compilation**

Navigate to the project's root directory in your terminal and run the following command to compile the program:

```bash
g++ solve.cpp BigInt.cpp -o solver -std=c++11
