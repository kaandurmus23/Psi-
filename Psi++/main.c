#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_ID_SIZE 30
#define MAX_INT_SIZE 10
#define MAX_LINE_SIZE 256


// Anahtar kelime tablosu
char *keywords[] = {
    "break", "case", "char", "const", "continue",
    "do", "else", "enum", "float", "for",
    "goto", "if", "int", "long", "record",
    "return", "static", "while"
};


int is_operator(char *op, char **token_ptr) {
    char *operators[] = {"++", "--", "*", "/", "+", "-", ":="};
    int num_operators = sizeof(operators) / sizeof(operators[0]);
    int op_len = 2;
    for (int i = 0; i < num_operators; i++) {
        if (strncmp(operators[i], op, strlen(operators[i])) == 0) {
            *token_ptr = operators[i];
            return 1;
        }
    }
    return 0;
}

int is_string(char *op, char **token_ptr){
    if(strncmp(op,"\"",1) == 0){
        *op++;
        int string_ended = 0;
        char word[MAX_LINE_SIZE];
        int i = 0;
        while (*op != '\0') {
            if (strncmp(op, "\"", 1) == 0) {
                string_ended = 1;
                op += 1;
                break;
            }
            word[i++] = *op++;
        }
        if (!string_ended) {
            printf("Error: string not ended\n");
            exit(EXIT_FAILURE);
            return 1;
        }else{
            *token_ptr = word;
            return 1;
        }
    }
    return 0;
}


int is_single_char_token(char *op, char **token_ptr) {
    switch (*op) {
        case '(':
            *token_ptr = "LeftPar";
            return 1;
        case ')':
            *token_ptr = "RightPar";
            return 1;
        case '[':
            *token_ptr = "LeftSquareBracket";
            return 1;
        case ']':
            *token_ptr = "RightSquareBracket";
            return 1;
        case '{':
            *token_ptr = "LeftCurlyBracket";
            return 1;
        case '}':
            *token_ptr = "RightCurlyBracket";
            return 1;
        default:
            return 0;
    }
}

// Anahtar kelime tablosundaki bir kelime mi kontrol eder
int is_keyword(char *token) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(keywords[i], token) == 0) {
            return 1;
        }
    }
    return 0;
}



int main(void) {
    FILE *in_file = fopen("code.psi", "r"); // Okunacak dosya
    FILE *out_file = fopen("code.lex", "w"); // Yazılacak dosya


    char line[MAX_LINE_SIZE]; // Okunan satır
    char *token; // Okunan token
    char *line_ptr; // Satırın işaretçisi

    if (!in_file) {
        printf("Dosya açma hatası!");
        return 1;
    }

    if (!out_file) {
        printf("Dosya açma hatası!");
        return 1;
    }

    while (fgets(line, MAX_LINE_SIZE, in_file)) {
        line_ptr = line;
        while (*line_ptr) {


            // Boşlukları atla
            while (isspace(*line_ptr)) {
                ++line_ptr;
            }

            //Yorum satırını sil
            if (strncmp(line_ptr, "/*", 2) == 0) {
                int comment_ended = 0;
                while (*line_ptr != '\0') {
                    if (strncmp(line_ptr, "*/", 2) == 0) {
                        comment_ended = 1;
                        line_ptr += 2;
                        break;
                    }
                    line_ptr++;
                    continue;
                }
                if (!comment_ended) {
                    printf("Error: comment not closed\n");
                    exit(EXIT_FAILURE);
                    return 0;
                }
            }


            // Eğer string ise
            if(is_string(line_ptr, &token)){
                fprintf(out_file, "STRING: %s\n", token);
                line_ptr += strlen(token) + 2;
                continue;
            }

            // Eğer karakter bir operatör ise
            if (is_operator(line_ptr, &token)) {
                fprintf(out_file, "OPERATOR: %s\n", token);
                line_ptr += strlen(token);
                continue;
            }

            // Eğer karakter bracket ise
            if (is_single_char_token(line_ptr, &token)) {
                fprintf(out_file, "%s\n", token);
                line_ptr += 1;
                continue;
            }

            //Eğer satır sonu ise
            if (strncmp(line_ptr, ";", 1) == 0){
                fprintf(out_file, "EndOfLine\n");
                line_ptr += 1;
                continue;
            }

            // Eğer karakter bir sayı ise
            if (isdigit(*line_ptr)) {
                // Tam sayı
                char num[MAX_INT_SIZE];
                int i = 0;
                while (isdigit(*line_ptr)) {
                    num[i++] = *line_ptr++;
                }
                if(i >= 9 ){
                    printf("Error: Max integer size 10 digits\n");
                    exit(EXIT_FAILURE);
                }else{
                    num[i] = '\0';
                    fprintf(out_file, "INTEGER: %s\n", num);
                }
                continue;
            }

            // Eğer karakter bir harf ise
            if (isalpha(*line_ptr)) {
                // Kelime
                char word[MAX_ID_SIZE + 1];
                int i = 0;
                while (isalnum(*line_ptr) || *line_ptr == '_') {
                    if (isalpha(*line_ptr)){
                        word[i] = tolower(*line_ptr);
                    }else{
                        word[i] = *line_ptr;
                    }
                    i++;
                    if((i) >= MAX_ID_SIZE){
                        printf("Error: Maximum identifier size is 30 characters\n");
                        exit(EXIT_FAILURE);
                    }
                    line_ptr++;
                }

                word[i] = '\0';
                if (is_keyword(word)) {
                    fprintf(out_file, "KEYWORD: %s\n", word);
                } else {
                    fprintf(out_file, "IDENTIFIER: %s\n", word);
                }
                continue;

            }



        }

    }

    fclose(in_file);
    fclose(out_file);

    return 0;
}

