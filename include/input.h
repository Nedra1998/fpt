#ifndef FPT_INPUT_H
#define FPT_INPUT_H

int IGetClick(int p[2]);
int DGetClick(double p[2]);
int GetKey();

int GetInt();
void PrintInt(int v);
double GetDouble();
void PrintDouble(double v);
void GetString(const char* v);
void PrintString(const char* v);

// void GetTimeInt(int hms[3]);
// void GetTimeDouble(double hms[3]);

#endif /* ifndef FPT_INPUT_H */
