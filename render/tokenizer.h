////////////////////////////////////////////////////////////////////
//
// $Id: tokenizer.h,v 1.1.1.1 2005/04/01 17:12:53 kanai Exp $
//
// Copyright (c) 2002 by Keio Research Institute at SFC
// All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include <algorithm>
#include <string>
using namespace std;

typedef std::string String;

class tokenizer {
 private:
  String::size_type cur_;
  String str_;
  String del_;
  bool   ret_;
//   void skip();
  void skip() {
    if ( cur_ == str_.length() )
      cur_ = String::npos;
    if ( !ret_ && cur_ != String::npos ) {
      String::size_type tmp = str_.find_first_not_of(del_, cur_);
      if ( tmp != String::npos )
	cur_ = tmp;
    };
}
 public:
  typedef std::pair<String::size_type,String::size_type> range_type;
  tokenizer(const String& str, const String& del, bool ret=false) 
    : cur_(0), str_(str), del_(del), ret_(ret) {};
    
//   bool empty();
  bool empty() {
    skip();
    return cur_ == String::npos;
  };
//   String next();
  String next() {
    range_type range = next_range();
    return str_.substr(range.first, range.second);
  };
  String str() { return str_; };
//   tokenizer::range_type tokenizer::next_range() {
  range_type next_range() {
    skip();
    String::size_type start = cur_;
    //String::size_type tmp = cur_;
    if ( cur_ != String::npos ) cur_ = str_.find_first_of(del_,cur_);
    if ( cur_ == String::npos ) return range_type(start,str_.length()-start);
    if ( ret_ && start == cur_ && del_.find(str_[cur_]) != String::npos ) ++cur_;
    return range_type(start,cur_-start);
  };

//   void set_str(const String& str);
  void set_str(const String& str) {
    str_ = str;
    cur_ = 0;
  };

//   void set_delimiter(const String& del, bool ret=false);
  void set_delimiter(const String& delim, bool ret) {
    del_ = delim;
    ret_ = ret;
  };

//   size_t count() const;
  size_t count() const {
    size_t count = 0;
    String::size_type currpos = cur_;
    while ( currpos != String::npos ) {
      if ( !ret_ ) {
	currpos = str_.find_first_not_of(del_,currpos);
	if ( currpos == String::npos ) { ++count; break; }
      } else if ( currpos == str_.length() ) {
	break;
      }
      String::size_type start = currpos;
      if ( currpos != String::npos ) currpos = str_.find_first_of(del_,currpos);
      if ( currpos == String::npos ) { ++count; break; }
      if ( ret_ && start == currpos && del_.find(str_[currpos]) != String::npos ) ++currpos;
      ++count;
    }
    return count;
  };

};

// sample
// "apple,banana,cherry" を "," で区切る
//stx::tokenizer<string> tok("apple,banana,cherry", ",");
//while ( !tok.empty() ) {
//  cout << tok.next() << " ";
//}
//cout << endl;
//tokenizer (コンストラクタ)
     
// tokenizer::tokenizer(const String& str, const String& del, bool ret)
//   : cur_(0), str_(str), del_(del), ret_(ret) {}
/*  str:トークン分割対象となる文字列 */
/*  del:トークンを区切る文字の集合 */
/*  ret:区切り文字をトークンとして扱うならtrue */
/*  ret == true のとき、区切り文字をトークンとして扱います。 */
/*  たとえば、  */

// sample
// "apple_banana__cherry_" を "_" で区切る
/*  bool ret = false; */
/*  stx::tokenizer<string> tok("apple_banana__cherry_", "_", ret); */
/*  while ( !tok.empty() ) { */
/*    cout << tok.next() << " "; */
/*  } */
/*  cout << endl; */
/*  では "apple", "banana", "cherry" が切り出されますが、ret = true とすると、 */
/*  "apple", "_", "banana", "_", "_", "cherry", "_" が切り出されます。  */
/*  skip (private) */
     
// void tokenizer::skip() {
//   if ( cur_ == str_.length() )
//     cur_ = String::npos;
//   if ( !ret_ && cur_ != String::npos ) {
//     String::size_type tmp = str_.find_first_not_of(del_, cur_);
//     if ( tmp != String::npos )
//       cur_ = tmp;
//   }
// }

/*  empty */
// bool tokenizer::empty() {
//   skip();
//   return cur_ == String::npos;
// }

/*  切り出すトークンが見つからないとき true を返します。  */

/*  next */
// String tokenizer::next() {
//   range_type range = next_range();
//   return str_.substr(range.first, range.second);
// }
/*  切り出されたトークンを返します。  */


/*  next_range */

// tokenizer::range_type tokenizer::next_range() {
//   skip();
//   String::size_type start = cur_;
//   //String::size_type tmp = cur_;
//   if ( cur_ != String::npos ) cur_ = str_.find_first_of(del_,cur_);
//   if ( cur_ == String::npos ) return range_type(start,str_.length()-start);
//   if ( ret_ && start == cur_ && del_.find(str_[cur_]) != String::npos ) ++cur_;
//   return range_type(start,cur_-start);
// }

/*  トークンを切り出し、その位置と長さを表す */
/*  std::pair<String::size_type,String::size_type> rangeを返します。 */
/*  range.first はトークンの開始位置、range.second はトークンの長さです。  */

/*  set_str */

// void tokenizer::set_str(const String& str) {
//   str_ = str;
//   cur_ = 0;
// }

/*  トークン分割対象となる文字列を設定します。 */
/*  トークン分割の開始位置を0に設定します。  */

/*  set_delimiter */

// void tokenizer::set_delimiter(const String& delim, bool ret) {
//   del_ = delim;
//   ret_ = ret;
// }

/*  区切り文字の集合を設定します。 */
/*  区切り文字をトークンとして扱うときは第2引数を true としてください。  */

/*  count */

// size_t tokenizer::count() const {
//   size_t count = 0;
//   String::size_type currpos = cur_;
//   while ( currpos != String::npos ) {
//     if ( !ret_ ) {
//       currpos = str_.find_first_not_of(del_,currpos);
//       if ( currpos == String::npos ) { ++count; break; }
//     } else if ( currpos == str_.length() ) {
//       break;
//     }
//     String::size_type start = currpos;
//     if ( currpos != String::npos ) currpos = str_.find_first_of(del_,currpos);
//     if ( currpos == String::npos ) { ++count; break; }
//     if ( ret_ && start == currpos && del_.find(str_[currpos]) != String::npos ) ++currpos;
//     ++count;
//   }
//   return count;
// }

/*  現在の位置から切り出されるトークンの数を返します。 */

/*  string_tokenizer / wstring_tokenizer */

/*  使用頻度の多いtokenizer<std::string>およびtokenizer<std::wstring>をそれぞれ */
/*  string_tokenizer,wstring_tokenizerとtypedefしました。  */

#endif // TOKENIZER_H
