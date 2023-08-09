# K-Nearest Neighbors (KNN) Classifier

This project implements a K-Nearest Neighbors (KNN) classifier in C++. The classifier is designed to work with datasets provided in CSV format and can perform both prediction and evaluation tasks. It utilizes the provided `Dataset` class for managing and manipulating datasets and inherits from the `Classifier` base class for implementing the KNN algorithm.

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)
- [Implementation Details](#implementation-details)
- [Dependencies](#dependencies)
- [Contributing](#contributing)
- [License](#license)

## Introduction

The K-Nearest Neighbors (KNN) classifier is a popular machine learning algorithm used for classification tasks. It classifies new data points by considering the class labels of their nearest neighbors in the training dataset. This project provides a C++ implementation of the KNN classifier, allowing users to load datasets from CSV files, train the classifier, make predictions, and evaluate its performance.

## Installation

1. Clone the repository to your local machine:
    https://github.com/AMQ4/classification-knn

2. Navigate to the project directory:
    cd knn-classifier

3. Compile the source code using the compilation script provided,It is 
   nothing1, but runs g++ -g -c main. cpp -o main, associates other 
   object files with the main object file, and generates an executable file:
    ./compile 
 
   **note:** 
   If you create some modifications to a file other than the main file, 
   you must re-create the object of that file that you modified.
   e.g.
   g++ -g -c the_file_you_have_modified.cpp -o name_of_that_file

4. Run the executable:
    ./run


## Usage

1. Make sure you have your dataset in CSV format. Update the dataset path and other parameters in the `main.cpp` file.

2. Compile the project using the provided Makefile.

3. Run the executable to train the KNN classifier, make predictions, and evaluate its performance.

## Implementation Details

The project is organized into several header and source files:

- `Dataset`: Manages and manipulates datasets, including reading from CSV files, normalization, renormalization, splitting, and more.
- `Classifier`: Defines the abstract base class for machine learning classifiers, including methods for training, prediction, and evaluation.
- `KNN`: Implements the KNN classifier by inheriting from the `Classifier` base class. It performs KNN-based classification and evaluation tasks.
- `PrettyTable`: Provides a utility for displaying data in a neat tabular format.

The project follows a modular structure, making it easy to extend and maintain the codebase.

## Dependencies

The project uses the following dependencies:

- `Boost Iostreams`
- `Boost System`

Make sure you have these dependencies installed on your system before compiling the code.

## Contributing

Contributions are welcome! If you find any issues or want to improve the project, feel free to open a pull request or submit an issue.

## License

This project is licensed under the [MIT License](LICENSE).





