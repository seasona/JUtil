#include <cstdio>

#define INT 100
#define STRING "XXX"

void print1(int x){
    printf("%d\n", x);
}

void print2(const char* s){
    printf("%s\n", s);
}

int main(){
    print1(INT);
    print2(STRING);
    return 0;
}