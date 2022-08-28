# BIU Advanced Programming Assignment Submission
**Ari Feiglin, Gal Nemetsky**

The current branch has the source for the third assignment.
For the first assignment's source, please navigate to the `assignment1-submission` branch.
For the second assignment's source, please navigate to the `assignment2-submission` branch.

## Compiling and Running The Project

We have provided you with a [Makefile](./Makefile) for your (and our) comfort.
The Makefile compiles the program into two executables: `knnserver` and `knnclient`.
Compiling is simple, simply run in the current directory (the one with this file):

```bash
$ make
```

To run the `knnserver` you must provide the following command line arguments:

+ The server IP
+ The server port
+ The classified file (the csv file which contains data of already classified object)
+ And a k-value

So for example running:

```bash
$ ./knnserver 127.0.0.1 1234 ./classified.csv 3
```

Runs the `knnserver` on IP address `127.0.0.1` and port `1234`, gets the classified data from `./classified.csv` and runs the knn algorithm with a k-value of 3.

To run the `knnclient` you must provide the following:

+ The client's IP
+ The server's IP
+ The server's port
+ The unclassified file
+ The file to output the classified information

For example:

```bash
$ ./knnclient 127.0.0.1 127.0.0.1 1234 ./unclassified.csv ./output.csv
```

Will connect the client to the server set up by the previous command and will output the classification of the objects in `./unclassified.csv` into `./output.csv`.

## General Structure

The project is split into four main directories:

+ **The `include` directory** - 
    The `include` directory holds all of the header files that have been used in this project.
    Some of the headers, for example `knn-algo.h`, hold implementations for template methods or other things that should be implemented in header files (`knn-algo.h` holds an implementation for the quick select algorithm for a vector and in `misc.h` there is an implemetation for an array we used in other parts of the project).
+ **The `lib` directory** - 
    The `lib` directory has files which are meant to be portable/be shared between the client and server.
    These are the files which contain implementations of the template methods declared in the various header files in the `include` directory which have not been defined.
    It also includes `.cpp` files for implementations of non-template member functions declared in various header files.
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

### Streams and Serialization

We also implemented streams and serilialization methods, detail on which can be found in this repository's wiki.

## Socket Constants

Because of the serialization we implemented, buffering was redundant and therefore not used.
This is because the amount of data needed to be read is known at run time, as per our implementation of serialization for various objects.
The port is left up to the user to determine, and the timeout for the socket was not specified (this may change for the next assignment when the server needs to actually be good).

