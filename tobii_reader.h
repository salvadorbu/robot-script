#ifndef TOBII_READER_H
#define TOBII_READER_H

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_DLL
#define TOBII_READER __declspec(dllexport)
#else
#define TOBII_READER __declspec(dllimport)
#endif

	int TOBII_READER GetX();
	int TOBII_READER GetY();

#ifdef __cplusplus
}
#endif

#endif