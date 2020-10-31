#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <iostream>
using namespace std;

using List = vector<pair<size_t, size_t> >;

class InvertedIndex {
public:
  void Add(string &&document);
  const List &Lookup(const string& word) const;

  size_t Size() const {
    return size;
  }

  map<string, List> GetIndex() {
    return index;
  }

private:
  map<string, List> index;
  List index_empty;
  size_t size = 0;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);
  //void PrintIndex();

private:
  InvertedIndex index;
};
