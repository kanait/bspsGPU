////////////////////////////////////////////////////////////////////
//
// $Id$
//
// STL string utility
//
// Copyright (c) 2005 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _STRUTIL_H
#define _STRUTIL_H 1

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

class StrUtil {

public:

  // �ŏ��̕�����������
  void first_word( std::string& str, std::string& fw ) {
    std::istringstream in(str); in >> fw;
  };

  // �ŏ��̕�����������
  void nth_word( std::string& str, int n, std::string& fw ) {
    std::istringstream in(str);
    int i = 0;
    std::string t;
    while ( i < n ) { in >> t; ++i; }
    fw = t;
  };

  // ������J�E���g
  int word_count( std::string& str ) {
    std::istringstream in(str);

    int count = 0; 
    std::string sstr;
    while ( in >> sstr ) ++count;

    return count;
  };

  // �����^�𕶎���^�ɕϊ�
  std::string itos( int n ) {
    std::stringstream str_stream;
    str_stream << n;
    return str_stream.str();
  };

  // �����^�𕶎���^�ɕϊ�
  std::string ftos( float n ) {
    std::stringstream str_stream;
    str_stream << n;
    return str_stream.str();
  };

  std::string GetExtension(const string &path) {
    std::string ext;
    size_t pos1 = path.rfind('.');
    if(pos1 != std::string::npos){
        ext = path.substr(pos1+1, path.size()-pos1);
        std::string::iterator itr = ext.begin();
        while(itr != ext.end()){
            *itr = tolower(*itr);
            itr++;
        }
        itr = ext.end()-1;
        while(itr != ext.begin()){    // �p�X�̍Ō��\0��X�y�[�X���������Ƃ��̑΍�
            if(*itr == 0 || *itr == 32){
                ext.erase(itr--);
            }
            else{
                itr--;
            }
        }
    }

    return ext;
  };
};

#endif // _STRUTIL_H

  
