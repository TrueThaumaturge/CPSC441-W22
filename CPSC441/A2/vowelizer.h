//File: vowelizer.h
//Name: Quentin Jennings
//UCID: 30089570
//Section: L01 (Carey)
#ifndef VOWELIZER
#define VOWELIZER

#include <stdbool.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool isVowel(char ch);
void split_simple(const char* str, char* str_vowels, char* str_cons);
void merge_simple(char* str, const char* str_vowels, const char* str_cons);
void split_advanced(const char* str, char* str_vowels, char* str_cons);
void merge_advanced(char* str, const char* str_vowels, const char* str_cons);

#endif