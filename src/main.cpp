/*
* @file    main.cpp
* @brief   Entry point for application
* @date    Date Created:: 2015-05-26 09-07
* @version $Revision:: 1#$
* @author  Dovgal Kostiantyn
* @copyright (c) The MIT License 2015
*/

#include "software-quality-model.h"
#include <QApplication>
#include <QMessageBox>


#ifdef _WINDOWS
// This function is used as an entry point in case linker flag is set to /SUBSYSTEM:WINDOWS
// Only for Windows platform
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* argv, int nShowCmd)
{
  int argc_mock = 1;
  QApplication a(argc_mock, &argv);

  SQM_client w;
  w.show();

  return a.exec();
}
#endif 

// This function is used as an entry point in folowing cases:
// - linker flag is set to /SUBSYSTEM:CONSOLE on Windows platform
// - on Linux or Mac OS platform
int main(int argc, char *argv[]) 
{
  QApplication a(argc, argv);

  SQM_client w;
  w.show();

  return a.exec();
}
