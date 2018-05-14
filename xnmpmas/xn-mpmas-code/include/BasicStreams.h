//------------------------------------------------------------------------------
//
//  Thomas Berger (main author), Pepijn Schreinemachers, and Thorsten Arnold
//
//
//  Hohenheim University (490d)
//  Professorship for Land Use Economics in the Tropics and Subtropics
//
//  May not be distributed without permission by main author
//
//  File:       BasicStreams.h (formerly fileFuns.h)
//
//  Contents:   Auxiliary class for stream handling, e.g. if file exists
//
//
//
//------------------------------------------------------------------------------



#ifndef _BasicStreams_h
#define _BasicStreams_h


#include <string.h>
#include <string>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;  // std::stream  --> stream (simpler)


#include "BasicEnums.h"

//char* copyRestOfString(char* inchar, int startVal);
void  copyRestOfString(char* inchar, char* outchar, int startVal);
string   stringCopyRestOfString(string inchar, int startVal);
string itos(int i);
void makeCharFrom3Int(char* irrigIDstr, int crA, int irrTech, int clusterNum);

//--------------------------------------------------------------------
// output_file_open() and input_file_open() are safer versions of
// using open().  When you open a file for output, if that file
// already exists you will overwrite it and loose its previous
// contents.  Not good. So output_file_open firsts check to see if
// the file 'filename' exists.  If it does, then output_file_open
// returns false, which means failure. If it returns true, then
// the file was successfully opened, and the fout stream should
// be ready for writing.
//--------------------------------------------------------------------
bool output_file_open (string filename, ofstream & fout);
bool input_file_open  (string filename, ifstream & fin);


//--------------------------------------------------------------------
// file_exists() uses a little trick to see if a file by the name of
// "filename" currently exists.  It does so by opening the file for
// reading.  If that fails, then we know the file doesn't exist, and
// return false.  If the file does exist, then we simply close it and
// return true.
//--------------------------------------------------------------------
bool file_exists (string filename);
bool file_exists (string filename, bool do_output);

int check_dirExists(string dname);
int check_dirExists(char* dname);

void createDIR(char* makeDIR);
void createDIR(string str_makeDir);


int getListOfAllDirectoriesStartingWith(string localDirectory, string startStringOfDirectories, string*& arrayOfStrings);
int getListOfAllFilesStartingWith(string localDirectory, string startStringOfDirectories, string*& arrayOfStrings);
// "Real" function for above two
int getListOfAllFilesByType_StartingWith(int type, string localDirectory, string startStringOfDirectories, string*& arrayOfStrings);


int getNumberOfFilesWithStart(string fnDir, string startFilename);
int getArrayOfFilesnamesWithStart(
        string fnIn,
        string startFilename,
        string*& allFilenames);

void deleteFile(string filename, bool doOutput);

#endif
