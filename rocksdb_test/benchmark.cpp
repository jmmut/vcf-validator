// Copyright (c) 2013, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.
#include <cstdio>
#include <string>
#include <iostream>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

using namespace rocksdb;


int main(int argc, char ** argv) {
  DB* db;
  Options options;
  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;
  std::string kDBPath = argc == 2 ? std::string(argv[1]) : "/tmp/rocksdb_simple_example";


  // open DB
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  int lines = 10000000;
  for (int i = 0; i < lines; i++) {
    // Put key-value
    s = db->Put(WriteOptions(), std::to_string(i), "Generic error message");
    assert(s.ok());
  }

  std::string value;
  // get value

  for (int i = 0; i < lines; i++) {
    s = db->Get(ReadOptions(), std::to_string(i), &value);
    if (i % 100000 == 0) {
      std::cout << "line " << i << ", " << value << std::endl;
    }
    assert(s.ok());

//    assert(value == "value");
    }
/*
  // atomically apply a set of updates
  {
    WriteBatch batch;
    batch.Delete("key1");
    batch.Put("key2", value);
    s = db->Write(WriteOptions(), &batch);
  }

  s = db->Get(ReadOptions(), "key1", &value);
  assert(s.IsNotFound());

  db->Get(ReadOptions(), "key2", &value);
  assert(value == "value");
*/
  delete db;

  return 0;
}
