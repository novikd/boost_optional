#include <iostream>
#include "optional.h"

using namespace std;

int main() {
    optional<int> a;
    a = 1;
    *a = 1;
    cout << *a << "\n";
    a.emplace(3);
    if (a){
        cout << "Work " << *a;
    } else {
        cout << "None";
    }
    optional<pair<string, int>> b = make_optional<pair<string, int>>("Wow this shit is working ", 228);
    if (b)
        cout << (*b).first << " " << (*b).second << "\n";

    a = none;
    optional<int> cc;
    cout << (a == cc) << "\n";

    a = 5;
    cc = 6;
    cout << *a << ' ' << *cc << "\n";

    a.swap(cc);

    cout << *a << ' ' << *cc;

    return 0;
}
