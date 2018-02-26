#ifndef FPT_INPUT_H
#define FPT_INPUT_H

int Gi_wait_click(int p[2]);
// G_get_wait_click
int G_wait_click(double p[2]);
// G_wait_key
int G_wait_key();

int GetInt();
void PrintInt(int v);
double GetDouble();
void PrintDouble(double v);
void GetString(const char* v);
void PrintString(const char* v);

// void GetTimeInt(int hms[3]);
// void GetTimeDouble(double hms[3]);

#endif /* ifndef FPT_INPUT_H */
