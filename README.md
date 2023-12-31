Parallel RSA Encryption Project
===============================

Overview
--------

This project is dedicated to the development and analysis of a parallelized version of the RSA encryption algorithm. We aim to compare its performance against the traditional sequential implementation of RSA.

Objectives
----------

*   **Implement Standard RSA Algorithm**: To establish a baseline, we will first implement the conventional RSA encryption algorithm.
*   **Measure Runtime**: The runtime of the standard RSA algorithm will be carefully measured to serve as a benchmark.
*   **Develop Parallel Implementation**: Utilizing MPI (Message Passing Interface) and Shared Address Space techniques, we will create an optimized parallel version of the RSA algorithm.
*   **Enhance Computational Efficiency**: The primary aim of the parallel implementation is to improve computational efficiency and scalability.
*   **Comprehensive Performance Analysis**: The project will conclude with a detailed analysis of the runtime performance of both the sequential and parallel versions. This analysis will focus on how these implementations scale with varying input sizes.

Approach
--------

*   **Benchmarking Sequential RSA**: We start by implementing the sequential version of RSA and measuring its performance across different scenarios.
*   **Parallelization Techniques**: The RSA algorithm will be parallelized using advanced techniques like MPI and Shared Address Space, known for their efficacy in high-performance computing environments.
*   **Performance Metrics**: We will measure key performance metrics, such as computational time and scalability, under various conditions.

Expected Outcomes
-----------------

*   A clear comparison between the sequential and parallel implementations of RSA in terms of efficiency and scalability.
*   Insights into the computational complexity of the RSA algorithm in both sequential and parallel forms, depicted through runtime plots against varying input sizes.

Note :
parallel.c : is MPI project

SAS.c : is Shared address memory project
