# BIU Advanced Programming Assignment Submission
**Ari Feiglin, Gal Nemetsky**

## Compiling and Running The Project

We have provided you with a [Makefile](./Makefile) for your (and our) comfort.
The Makefile compiles the program into an executable called `knn`.
Compiling is simple, simply run:

```bash
$ make
```

To run `knn`, pass a k value to the executable, for example 3:

```bash
$ ./knn 3
```

This will write output to the required `*_output.csv` files.


## General Structure

The project is split into three main directories:

+ **The `include` directory** - 
    The `include` directory holds all of the header files that have been used in this project.
    Some of the headers, for example `knn-algo.h`, hold implementations for template methods or other things that should be implemented in header files (`knn-algo.h` holds an implementation for the quick select algorithm for a vector and in `misc.h` there is an implemetation for an array we used in other parts of the project).
+ **The `lib` directory** - 
    The `lib` directory contains all files which are meant to be linked to the program and not compiled.
    These are the files which contain implementations of the template methods declared in the various header files in the `include` directory which have not been defined.
+ **The `src` directory** - 
    In the `src` directory we have `.cpp` files that implement all of the program-dependent part of the project, that includes the `main` and other program-dependent processes.

## Optimization

In an attempt to optimize the run time of the program we implemented the Quickselect algorithm.
We implemented its random form (choosing a pivot randomly) as according to the internet in practice this is more efficient than the median of medians approach, despite it having worse time complexity in theory.
The source can be found in [knn-algo.h](./include/knn-algo.h).

## Code Overview

Here we will provide a quick overview of the structures and methods we implemented in our project.

### Data Structures

All code discussed here can be found in [include/knn-datastructs.h](include/knn-datastructs.h) and [lib/knn-datastructs.tpp](lib/knn-datastructs.tpp).

The main data structure is the `DataSet` data structure.
Its purpose is to contain all the classified points read in from the `classified.csv` file.
Using this data it can then find the closest class to an input point.
In an attempt to make the class more versatile, we made it generic, capable of storing any datatype, or more specifically any kind of `DataPoint` (which we'll discuss further down).
The DataSet can accept an input `DataPoint` as well as a function for computing distances between datapoints, and then returns the name of the closest class to said `DataPoint`.
It does this in the following steps:

1.
    Transform the `DataSet`'s vector of classified `DataPoint`s into a vector of `DistancePoint`s, which is a struct containing the index of a classified `DataPoint` and its distance from
    the input `DataPoint`.
2.
    Pass this to a function which computes the k-smallest `DistancePoint`s.

3.
    Unmap the indexes of the k-smallest `DistancePoint`s back to `DataPoint`s.

4.
    Create a hashmap of strings to integers, which count how many of each class are in the k-closest `DataPoint`s.
    Then return the name of the class which has the greatest count.

This algorithm should be relatively efficient, with almost linear time complexity (since the quickselect algorithm we implemented has an expected time complexity of $O(n)$, discussed above).

The next important data structure is the `DataPoint` interface.
The `DataPoint` interface simply stores data and a class name, since it is a template class, it can store data of *any* type.
The most important implementation of the `DataPoint` interface is the `CartDataPoint` (Cartesian Data Point) class.
It too is a template class, which stores an array (implemented in [misc.h](./include/misc.h)) of any data type.
Thus the flowers read in from the csv files are stored as Cartesian Data Points of a `double` type.

We also implemented a small class for static arrays in [misc.h](./include/misc.h).
We wanted a lightweight array which didn't need to be dynamic with efficient random access, so neither `std::vector` nor `std::list` were a good fit.
Unfortunately `std::array` requires its size be declared explicitly, which was not ideal.
So we implemented our own, which is simply a wrapper around a primitive array.

### Functions

As mentioned before, we implemented a quickselect algorithm in [knn-algo.h](./include/knn-algo.h).
It utilizes a method for randomly selecting a pivot, which while theoretically is not the most efficient method in practicality it is very efficient (supposedly. According to that one source I skimmed over).

Furthermore, we implemented various methods for initializing a `DataSet` from a csv file (`classified.csv`), and reading unclassified points from another file and computing its nearest class.
These are all template methods intended to be versatile.
Them being template methods also doesn't add any more complexity on our part (just for the compiler and your memory) so it was a win-win for us.
The functions can be found in [knn-io.tpp](./lib/knn-io.tpp).

