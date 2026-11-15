#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// Safe input helpers: every value is read as a whole line and checked, so
// typing letters in a number field can never break the program.
// true once the input has ended (for example the end of a redirected file)
inline bool &inputEnded()
{
    static bool ended = false;
    return ended;
}

inline bool readLine(string &out)
{
    if (!getline(cin, out))
    {
        inputEnded() = true;
        return false;
    }
    if (out.length() > 0 && out[out.length() - 1] == '\r')
    {
        out.erase(out.length() - 1);
    }
    int start = 0;
    int end = (int)out.length();
    while (start < end && (out[start] == ' ' || out[start] == '\t'))
    {
        start++;
    }
    while (end > start && (out[end - 1] == ' ' || out[end - 1] == '\t'))
    {
        end--;
    }
    out = out.substr(start, end - start);
    return true;
}

// Asks until the person types a whole number from low to high.
// Returns false only when the input has ended.
inline bool readInt(string prompt, int low, int high, int &value)
{
    while (true)
    {
        cout << prompt;
        string line;
        if (!readLine(line))
        {
            return false;
        }
        bool ok = line.length() > 0 && line.length() <= 9;
        int i = 0;
        if (ok && line[0] == '-')
        {
            i = 1;
            ok = line.length() > 1;
        }
        for (; ok && i < (int)line.length(); i++)
        {
            if (line[i] < '0' || line[i] > '9')
            {
                ok = false;
            }
        }
        if (ok)
        {
            int number = atoi(line.c_str());
            if (number >= low && number <= high)
            {
                value = number;
                return true;
            }
        }
        cout << "Invalid input. Please enter a whole number from " << low << " to " << high << ".\n";
    }
}

// Asks until the person types some text (no commas, because the data files use commas).
inline bool readText(string prompt, string &value)
{
    while (true)
    {
        cout << prompt;
        string line;
        if (!readLine(line))
        {
            return false;
        }
        if (line.length() == 0)
        {
            cout << "This cannot be empty.\n";
        }
        else if (line.find(',') != string::npos)
        {
            cout << "Please do not use commas.\n";
        }
        else if (line.length() > 40)
        {
            cout << "Please use at most 40 characters.\n";
        }
        else
        {
            value = line;
            return true;
        }
    }
}

#endif
