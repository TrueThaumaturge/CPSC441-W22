//File: vowelizer.c
//Name: Quentin Jennings
//UCID: 30089570
//Section: L01 (Carey)
#include "vowelizer.h"

//str_vowels and str_cons are expected to be empty strings
void split_simple(const char* str, char* str_vowels, char* str_cons) {
    //iterates through the input string
    while(*str != '\0') {
        if(isVowel(*str)) { //if the current character is a vowel
            *str_vowels = *str; //add it to the vowels
            *str_cons = ' ';
        }
        else { //if it's a consonant
            *str_cons = *str; //add it to the consonants
            *str_vowels = ' ';
        }
        //iterate each string forward
        str++;
        str_vowels++;
        str_cons++;
    }
    //finish the 2 strings we just created
    *str_vowels = '\0';
    *str_cons = '\0';

    return;
}

//str_vowels and str_cons are expected to be empty strings
void merge_simple(char* str, const char* str_vowels, const char* str_cons) {
    //iterates through the output string to place the characters
    if(*str_vowels == '\0') { //if there are no vowels
        strcpy(str, str_cons); //simply copy the consonant string
    }
    else { //otherwise
        while(*str_vowels != '\0') { //until you reach the end
            if(*str_vowels != ' ') { //if the next char is a vowel
                *str = *str_vowels;
            }
            else if(*str_vowels == ' ') { //if the next char is a consonant
                *str = *str_cons;
            }
            str++;
            str_vowels++;
            str_cons++;
        }
        *str = '\0';
    }
}

//str_vowels and str_cons are expected to be empty strings
void split_advanced(const char* str, char* str_vowels, char* str_cons) {
    int n = 0; //number of letters before vowel
    //iterates through the input string
    while(*str != '\0') {
        if(isVowel(*str)) { //if the current character is a vowel
            *str_vowels = n + 48; //adds the number of non-vowels to the string
            n = 0; //resets n
            str_vowels++; //increments vowels pointer

            *str_vowels = *str;
            str_vowels++; //increments vowels pointer again
        }
        else { //if it's a consonant
            *str_cons = *str; //add it to the consonants
            str_cons++; //increments cons pointer
            n++; //increments n
        }
        str++;
    }
    //finishes the strings
    *str_vowels = '\0';
    *str_cons = '\0';
}

//str_vowels and str_cons are expected to be empty strings
void merge_advanced(char* str, const char* str_vowels, const char* str_cons) {
    while(*str_vowels != '\0') { //inserts all the vowels and whatever's in between
        int n_cons = *str_vowels - 48; //grabs the number of characters before the vowel
        str_vowels++;

        for(int i = 0; i < n_cons; i++) { //adds the consonants before the vowel to the string
            *str = *str_cons;
            str++;
            str_cons++;
        }

        *str = *str_vowels; //adds the vowel to the string
        str++;
        str_vowels++;
    }
    //adds the rest of the non-vowels
    while(*str_cons != '\0') {
        *str = *str_cons;
        str++;
        str_cons++;
    }
    *str = '\0';
}

bool isVowel(char ch) {
    ch = tolower(ch);
    return (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u');
}

#if 0 //main for testing purposes
int main(int argc, char *argv[]) {
    
    char inputstr[50] = "Hello there!";
    char a[50], b[50];
    split_simple(inputstr, a, b);
    printf("Testing Simple Split:\nString: [%s]\nVowels: [%s]\nRest: [%s]\n\n", inputstr, a, b);

    char outputstr[50];
    merge_simple(outputstr, a, b);
    printf("Testing Simple Merge:\nResult: [%s]\n\n", outputstr);

    char c[50], d[50];
    split_advanced(inputstr, c, d);
    printf("Testing Advanced Split:\nString: [%s]\nVowels: [%s]\nRest: [%s]\n\n", inputstr, c, d);

    char outputstr2[50];
    merge_advanced(outputstr2, c, d);
    printf("Testing Advanced Merge:\nResult: [%s]\n\n", outputstr2);

    return 0;
}
#endif