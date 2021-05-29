////////////////////////////////////////////////////////////////////
//
// C++11 timer class based on std::chrono
//
// Copyright (c) 2016 by Takashi Kanai. All rights reserved. 
//
////////////////////////////////////////////////////////////////////

#ifndef _C11TIMER_HXX
#define _C11TIMER_HXX 1

#include <chrono>

class C11Timer {

public:

  static void ResetCount() {
    Single().reset_ = std::chrono::system_clock::now();
  };

  // リセット後の経過時間のミリ秒で取得(小数点以下)
  static double GetNowCount() {
    auto diff = std::chrono::system_clock::now() - Single().reset_;
    return (double)std::chrono::duration_cast<std::chrono::microseconds>(diff).count() / 1000;
  };

  // 日付を取得
  static void GetDate(tm* currentTime) {
    time_t timer;

    time(&timer);
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(currentTime, &timer);
#else
    currentTime = localtime(&timer);
#endif
  };

  // 現在の日付、時刻を文字列にして返す
  static std::string GetDateString() {
    tm time;
    GetDate( &time );
    string str = std::to_string(time.tm_year+1900) + "_"
      + std::to_string(time.tm_mon) + "_"
      + std::to_string(time.tm_mday) + "_"
      + std::to_string(time.tm_hour) + "_"
      + std::to_string(time.tm_min) + "_"
      + std::to_string(time.tm_sec);
    return str;
  };

  // FPS計測の更新と取得
  static double CheckGetFPS() {
    CheckFPS();
    return GetFPS();
  };

  // FPSを取得
  static double GetFPS() {
    return Single().fps_;
  };

  // FPSの計測開始
  static void ResetFPS() {
    Single().fpsCounter_ = std::chrono::system_clock::now();
  };

  // FPS計測を更新
  static void CheckFPS() {
    auto diff = std::chrono::system_clock::now() - Single().fpsCounter_;
    Single().fps_ = 1000000.0 / (double)std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
    // automatically reset fpsCounter_
    ResetFPS();
  };

  // 処理時間の更新と取得
  static double CheckGetWatch( bool isReset ) {
    CheckWatch( isReset );
    return GetWatch();
  };

  // 処理時間取得
  static double GetWatch() {
    return Single().watch_;
  };

  // 処理時間計測開始
  static void ResetWatch() {
    Single().watchCounter_ = std::chrono::system_clock::now();
  };

  // 処理時間計測更新
  // StartWatch からの経過時間をミリ秒単位で描画
  // 続けてDrawWatchする事も可能
  static void CheckWatch( bool isReset ) {
    auto diff = std::chrono::system_clock::now() - Single().watchCounter_;
    Single().watch_ = (double)std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() / 1000.0;
    if ( isReset ) ResetWatch();
  };

private:

  double fps_;
  double watch_;

  std::chrono::system_clock::time_point reset_;
  std::chrono::system_clock::time_point fpsCounter_;
  std::chrono::system_clock::time_point watchCounter_;

  static C11Timer& Single()
  {
    static C11Timer single_;
    return single_;
  };

};

#endif // _C11TIMER_HXX
