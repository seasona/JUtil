#include <cstdio>

int main() {
    char output1[32];
    char output2[32];
    const char *surname = "HankGrandLike";
    const char *name = "DogCatMouseKig";
    int age = 16;
    // snprintf中间的约束格式字符串是可以多个的，中间用空格隔开，它会自动合并
    // "%016" "x"等同于"%016x"
    snprintf(output1, 0x20,
             "%016"
             "x",
             age);
    snprintf(output2, 0x20,
             "%s"
             "-"
             "%s",
             surname, name);
    printf("The result output1 is %s\n", output1);
    printf("The result output2 is %s\n", output2);
    return 0;
}