#include<iostream>

#include<unordered_map>

#include "b+tree.cpp"

#include <algorithm>

#include <random>

#include <chrono>

#include <cstring>

#include <vector>

#include <fstream>

#include <stdlib.h>

#include <atomic>

#include <thread>

#include <cstdlib>

#include <pthread.h>

#include <sched.h>

#include <math.h>

#include "lock.h"


using namespace std;

struct thread_args {
  BplusTree * bPlusTreeInstance;
  uint niters;
  uint nitems;
  uint thread_id;
  double * duration;
};

double calculate_mean(double * items, uint nitems) {

  double sum = 0;

  for (uint i = 0; i < nitems; i++) {

    sum += items[i];

  }
  return sum / nitems;

}

double calculate_std_dev(double * items, double mean, uint nitems) {

  double sum = 0;

  for (uint i = 0; i < nitems; i++) {

    sum += pow(items[i] - mean, (double) 2);

  }

  return sqrt(sum / nitems);

}

double calculate_min(double * items, uint nitems) {

  double min = items[0];

  for (uint i = 1; i < nitems; i++) {

    if (items[i] < min) {
      min = items[i];
    }
  }

  return min;

}

double calculate_max(double * items, uint nitems) {

  double max = items[0];

  for (uint i = 1; i < nitems; i++) {

    if (items[i] > max) {
      max = items[i];
    }
  }

  return max;

}

void * searchThread(void * arg) {
  thread_args * my_args = (thread_args * ) arg;
  uint niters = my_args -> niters;
  uint nitems = my_args -> nitems;
  int cpuid = sched_getcpu();
  // int * items = my_args->items;
  BplusTree * bPlusTreeInstance = my_args -> bPlusTreeInstance;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution < int > dist(100000, 100000 + 1000);
  for (uint iter = 0; iter < niters; iter++) {
    int search = dist(gen);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    bool searchResult = bPlusTreeInstance -> query(search);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    my_args -> duration[iter] = (std::chrono::duration_cast < std::chrono::duration < double >> (end - start)).count() * 1000;
    // std::cout<<"Search Result for "<<search<<": "<< searchResult <<std::endl;

  }

  return NULL;
}

void * rangeThread(void * arg) {
  thread_args * my_args = (thread_args * ) arg;
  uint niters = my_args -> niters;
  uint nitems = my_args -> nitems;
  uint thread_id = my_args -> thread_id;
  int cpuid = sched_getcpu();
  // int * items = my_args->items;
  BplusTree * bPlusTreeInstance = my_args -> bPlusTreeInstance;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution < int > dist(100000, 100000 + 1000);
  int search = dist(gen);
  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
  vector < long unsigned int > searchResult = bPlusTreeInstance -> rangesearch(search, search + 99);
  std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
  my_args -> duration[thread_id] = (std::chrono::duration_cast < std::chrono::duration < double >> (end - start)).count() * 1000;
  // std::cout<<"Search Result for "<<search<<": "<< searchResult <<std::endl;

  return NULL;
}

void * insertThread(void * arg) {
  thread_args * my_args = (thread_args * ) arg;
  // uint niters = my_args->niters / 10;
  uint nitems = my_args -> nitems;
  uint thread_id = my_args -> thread_id;
  int insert = 100 + thread_id + 1;
  BplusTree * bPlusTreeInstance = my_args -> bPlusTreeInstance;
  // int start_item = nitems + 1 + my_args->thread_id*niters;
  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
  bPlusTreeInstance -> insert(insert, insert);
  std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
  my_args -> duration[thread_id] = (std::chrono::duration_cast < std::chrono::duration < double >> (end - start)).count() * 1000;
  if (!bPlusTreeInstance -> query(insert)) {
    std::cout << "Error Inserting" << std::endl;
  }
  return NULL;
}

void * deleteThread(void * arg) {
  thread_args * my_args = (thread_args * ) arg;
  // uint niters = my_args->niters / 10;
  uint nitems = my_args -> nitems;
  uint thread_id = my_args -> thread_id;
  int delete_item = thread_id + 100;
  BplusTree * bPlusTreeInstance = my_args -> bPlusTreeInstance;
  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
  bPlusTreeInstance -> remove(delete_item);
  std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
  my_args -> duration[thread_id] = (std::chrono::duration_cast < std::chrono::duration < double >> (end - start)).count() * 1000;
  if (bPlusTreeInstance -> query(delete_item)) {
    std::cout << "Error deleting " << std::endl;
  }

  return NULL;
}

int main(int argc, char ** argv) {

  if (argc != 6) {
    std::cout << "Usage: ./run [num reader threads][num writer threads][num items][num iterations][tree order]\n";
    return 1;
  }

  uint nreaders = atoi(argv[1]);
  uint nwriters = atoi(argv[2]);
  uint nitems = atoi(argv[3]);
  uint niters = atoi(argv[4]);
  uint64_t order = atoi(argv[5]);

  if (nreaders == 0 || nwriters == 0 || nitems == 0 || niters == 0) {
    perror("All arguments must be > 0");
    return -1;
  }
  if (order < 4 && order % 2 != 0) {
    perror("Order must be even and >= 4");
    return -1;
  }

  printf("Running benchmark with %u readers, %u writers, %u items, %u iterations\n", nreaders, nwriters, nitems, niters);

  BplusTree obj = BplusTree(order);
  unordered_map < uint64_t, uint64_t > umap;
  uint64_t * myArray = (uint64_t * ) malloc(nitems * sizeof(uint64_t));
  for (int i = 0; i < nitems; i++) {
    myArray[i] = i + 10145671744257337181;
  }

  // Initialize a random number generator
  std::random_device rd;
  std::mt19937 rng(rd());

  for (int i = 0; i < nitems; i++) {
    umap[myArray[i]] = 1;

    obj.insert(myArray[i], myArray[i]);
    if (obj.query(myArray[i])) {
      cout << "";
    } else {
      cout << umap[myArray[i]] << " " << myArray[i] << "   " << "Error" << endl;
      obj.print();
    }
  }

  double * reader_output = (double * ) malloc(nreaders * niters * sizeof(double));
  if (reader_output == NULL) {
    perror("Malloc reader output");
    return -1;
  }
  thread_args * reader_args = (thread_args * ) malloc(nreaders * sizeof(thread_args));
  if (reader_args == NULL) {
    perror("Malloc reader args");
    return -1;
  }
  for (uint i = 0; i < nreaders; i++) {
    reader_args[i].bPlusTreeInstance = & obj;
    reader_args[i].nitems = nitems;
    reader_args[i].niters = niters;
    reader_args[i].thread_id = i;
    // reader_args[i].items = items;
    reader_args[i].duration = reader_output + (i * niters);
  }
  pthread_t * readers = (pthread_t * ) malloc(nreaders * sizeof(pthread_t));
  if (readers == NULL) {
    perror("Malloc readers");
    return -1;
  }

  double * range_output = (double * ) malloc(nreaders * sizeof(double));
  if (range_output == NULL) {
    perror("Malloc reader output");
    return -1;
  }
  thread_args * range_args = (thread_args * ) malloc(nreaders * sizeof(thread_args));
  if (range_args == NULL) {
    perror("Malloc reader args");
    return -1;
  }

  for (uint i = 0; i < nreaders; i++) {
    range_args[i].bPlusTreeInstance = & obj;
    range_args[i].nitems = nitems;
    range_args[i].niters = niters;
    range_args[i].thread_id = i;
    // reader_args[i].items = items;
    range_args[i].duration = range_output + (i);
  }
  pthread_t * range_readers = (pthread_t * ) malloc(nreaders * sizeof(pthread_t));
  if (range_readers == NULL) {
    perror("Malloc readers");
    return -1;
  }

  uint w_niters = niters / 10;
  double * writer_output = (double * ) malloc(nwriters * sizeof(double));
  if (writer_output == NULL) {
    perror("Malloc writer output");
    return -1;
  }
  thread_args * writer_args = (thread_args * ) malloc(nwriters * sizeof(thread_args));
  if (writer_args == NULL) {
    perror("Malloc writer args");
    return -1;
  }
  for (uint i = 0; i < nwriters; i++) {
    writer_args[i].bPlusTreeInstance = & obj;
    writer_args[i].nitems = nitems;
    writer_args[i].niters = w_niters;
    writer_args[i].thread_id = i;
    writer_args[i].duration = writer_output + (i);
  }
  pthread_t * writers = (pthread_t * ) malloc(nwriters * sizeof(pthread_t));

  if (writers == NULL) {
    perror("Malloc readers");
    return -1;
  }

  //--------
  double * remove_output = (double * ) malloc(nwriters * sizeof(double));
  if (remove_output == NULL) {
    perror("Malloc writer output");
    return -1;
  }
  thread_args * remover_args = (thread_args * ) malloc(nwriters * sizeof(thread_args));
  if (remover_args == NULL) {
    perror("Malloc writer args");
    return -1;
  }

  for (uint i = 0; i < nwriters; i++) {
    remover_args[i].bPlusTreeInstance = & obj;
    remover_args[i].nitems = nitems;
    remover_args[i].niters = w_niters;
    remover_args[i].thread_id = i;
    remover_args[i].duration = remove_output + (i);
  }
  pthread_t * removers = (pthread_t * ) malloc(nwriters * sizeof(pthread_t));

  if (removers == NULL) {
    perror("Malloc readers");
    return -1;
  }
  for (uint i = 0; i < nreaders; i++) {

    if (pthread_create( & readers[i], NULL, searchThread, (void * ) & reader_args[i])) {

      perror("pthread_create");
      return -1;

    }

  }

  for (uint i = 0; i < nwriters; i++) {
    if (pthread_create( & writers[i], NULL, insertThread, (void * ) & writer_args[i])) {

      perror("pthread_create");
      return -1;

    }

  }

  for (uint i = 0; i < nreaders; i++) {

    if (pthread_create( & range_readers[i], NULL, rangeThread, (void * ) & range_args[i])) {

      perror("pthread_create");
      return -1;

    }

  }

  for (uint i = 0; i < nwriters; i++) {
    if (pthread_create( & removers[i], NULL, deleteThread, (void * ) & remover_args[i])) {
      perror("pthread_create");
      return -1;

    }

  }

  for (uint i = 0; i < nwriters; i++) {
    pthread_join(writers[i], NULL);
  }

  for (uint i = 0; i < nreaders; i++) {
    pthread_join(range_readers[i], NULL);
  }

  for (uint i = 0; i < nwriters; i++) {
    pthread_join(removers[i], NULL);
  }

  for (uint i = 0; i < nreaders; i++) {
    pthread_join(readers[i], NULL);
  }

  // vector<long unsigned int> searchResult = obj.rangesearch(50, 150);
  // searchResult = obj.rangesearch(10000, 10000 + 99);

  double reader_mean = calculate_mean(reader_output, nreaders * niters);
  double reader_std_dev = calculate_std_dev(reader_output, reader_mean, nreaders * niters);
  double reader_min = calculate_min(reader_output, nreaders * niters);
  double reader_max = calculate_max(reader_output, nreaders * niters);

  double writer_mean = calculate_mean(writer_output, nwriters);
  double writer_std_dev = calculate_std_dev(writer_output, writer_mean, nwriters);
  double writer_min = calculate_min(writer_output, nwriters);
  double writer_max = calculate_max(writer_output, nwriters);

  double remover_mean = calculate_mean(remove_output, nwriters);
  double remover_std_dev = calculate_std_dev(remove_output, remover_mean, nwriters);
  double remover_min = calculate_min(remove_output, nwriters);
  double remover_max = calculate_max(remove_output, nwriters);

  double range_mean = calculate_mean(range_output, nreaders);
  double range_std_dev = calculate_std_dev(range_output, range_mean, nreaders);
  double range_min = calculate_min(range_output, nreaders);
  double range_max = calculate_max(range_output, nreaders);

  printf("Readers (Point): min %f ms, max %f ms, mean %f ms, std_dev %f\n", reader_min, reader_max, reader_mean, reader_std_dev);
  printf("Readers (Range): min %f ms, max %f ms, mean %f ms, std_dev %f\n", range_min, range_max, range_mean, range_std_dev);
  printf("Writers (Insert): min %f ms, max %f ms, mean %f ms, std_dev %f\n", writer_min, writer_max, writer_mean, writer_std_dev);
  printf("Writers (Remove): min %f ms, max %f ms, mean %f ms, std_dev %f\n", remover_min, remover_max, remover_mean, remover_std_dev);
}
