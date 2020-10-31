#pragma once

#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "profile.h"
#include "duration.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
#include <iomanip>
#include <ctime>

using namespace std;

class DocGenerator {
public:
  DocGenerator(const size_t doc_count_,
      const size_t words_in_doc_,
      const size_t query_count_,
      const size_t words_in_query_,
      const int word_length_,
      const size_t voc_size_ = 0):
        doc_count(doc_count_),
        words_in_doc(words_in_doc_),
        query_count(query_count_),
        words_in_query(words_in_query_),
        word_length(word_length_),
        voc_size(voc_size_ == 0 ? doc_count*words_in_doc : voc_size_) {
    vocabulary.reserve(voc_size);
    srand(time(NULL));
    for (size_t i = 0; i < voc_size; i++) {
      vocabulary.push_back(random_string(word_length));
    }
  }

  void ViewDocs() const {
    ViewData(docs);
  }
  void ViewQueries() const {
    ViewData(queries);
  }
  void ViewInfo() const {
    for (auto& [word, stat]: info) {
      cout << word << ": ";
      for (auto [docid, hits]: stat) {
        cout << "{ doc id: " << docid << " : " << hits << " hits } ";
      }
      cout << endl;
    }
  }
  pair<istringstream, istringstream> GetData() {
    ostringstream d, q;
    Generate();
    for (auto& row: docs) {
      for (auto& word: row) {
        d << word << ' ';
      }
      d << '\n';
    }
    for (auto& row: queries) {
      for (auto& word: row) {
        q << word << ' ';
      }
      q << '\n';
    }
    istringstream docs_input(d.str());
    istringstream queries_input(q.str());
    return make_pair(move(docs_input), move(queries_input));
  }
private:
  const size_t doc_count, words_in_doc, query_count, words_in_query, voc_size;
  const int word_length;
  size_t unique_words_in_docs;
  vector<string> vocabulary;
  string random_string( size_t length )
  {
    auto randchar = []() -> char
    {
      const char charset[] =
          "0123456789"
          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
          "abcdefghijklmnopqrstuvwxyz";
          const size_t max_index = (sizeof(charset) - 1);
          return charset[ rand() % max_index ];
     };
    string str(length,0);
    generate_n( str.begin(), length, randchar );
    return str;
  }
  vector<vector<string_view>> docs;
  vector<vector<string_view>> queries;
  map<string_view, pair<size_t, size_t>> expected;
  map<string_view, map<size_t, size_t>> info;
  void GenerateDocsAndInfo() {
    docs.reserve(doc_count);
    set<string_view> uniques;
    for (size_t i = 0; i < doc_count; i++) {
      vector<string_view> row;
      row.reserve(words_in_doc);
      for (size_t j = 0; j < words_in_doc; j++) {
        size_t ind = rand() % voc_size;
        row.push_back({vocabulary[ind]});
        info[{vocabulary[ind]}][i]++;
        uniques.insert({vocabulary[ind]});
      }
      docs.push_back(row);
    }
    unique_words_in_docs = uniques.size();
  };
  void GenerateQueries() {
    queries.reserve(query_count);
    for (size_t i = 0; i < query_count; i++) {
      set<string_view> srow;
      while (srow.size() < min(words_in_doc, min(words_in_query, unique_words_in_docs))) {
        size_t doc_i = rand() % doc_count;
        size_t doc_j = rand() % words_in_doc;
        string_view word = docs[doc_i][doc_j];
        srow.insert(word);
      }
      vector<string_view> row;
      move(srow.begin(), srow.end(), back_inserter(row));
      queries.push_back(move(row));
    }
  };
  void GenerateExpected() {};
  void ViewData(const vector<vector<string_view>>& data) const {
    set<string_view> s;
    for (auto& row: data) {
      for (auto& word: row) {
        cout << word << ' ';
        s.insert(word);
       }
       cout << endl;
     }
     cout << "Number of unique words: " << s.size() << endl;
  }
  void Generate() {
      GenerateDocsAndInfo();
      GenerateQueries();
  }
};

void TestSpeed() {
  {
    cerr << "Testing All words" << endl;
    DocGenerator d(50000, 50, 50000, 10, 100, 10000);
    auto [docs, queries] = d.GetData();
    ostringstream queries_output;
    cerr << "Data generated, starting processing" << endl;
    SearchServer srv;
    {
      LOG_DURATION("Update");
      srv.UpdateDocumentBase(docs);
    }
    {
      LOG_DURATION("Totat queries processing");
      srv.AddQueriesStream(queries, queries_output);
    }
  }
}

