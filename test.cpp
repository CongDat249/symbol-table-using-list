#include<iostream>
#include<regex>

using namespace std;

bool isPrinted(string s, string name)
{
    regex res("([\\w])+(//)");
    int pos = s.find(name);
    if (pos == s.npos)
    {
        return false;
    }
    else
    {
        if (regex_match(s.substr(pos, name.length() + 2), res))
        {
            return true;
        }
        return false;
    }
}


// INSERT a b
int main()
{

}