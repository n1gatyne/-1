#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <string>

const int MAX_PEOPLE = 100;
const int MAX_LINE = 1000;
const int MAX_NAME = 50;

struct Person {
    char name[MAX_NAME];
    double paid;
    double spent;
};

int findPerson(Person* people, int n, const char* name) {
    for (int i = 0; i < n; i++) {
        if (strcmp(people[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    FILE* input = fopen("input.txt", "r");
    FILE* output = fopen("output.txt", "w");

    if (!input) {
        printf("Error: cannot open input.txt\n");
        return 1;
    }

    Person people[MAX_PEOPLE];
    int peopleCount = 0;
    char line[MAX_LINE];

    // ---- читаем первую строку ----
    fgets(line, MAX_LINE, input);

    std::stringstream ss_first(line);
    int n;
    ss_first >> n;

    std::string name;
    while (ss_first >> name) {
        strcpy(people[peopleCount].name, name.c_str());
        people[peopleCount].paid = 0;
        people[peopleCount].spent = 0;
        peopleCount++;
    }

    // ---- читаем траты ----
    while (fgets(line, MAX_LINE, input)) {
        if (line[0] == '\n' || line[0] == '\0') continue;

        std::string str(line);
        std::stringstream ss(str);

        std::string payer;
        double amount;

        ss >> payer >> amount;

        int payerIdx = findPerson(people, peopleCount, payer.c_str());
        if (payerIdx != -1) {
            people[payerIdx].paid += amount;
        }

        // все включены по умолчанию
        int included[MAX_PEOPLE];
        for (int i = 0; i < peopleCount; i++) included[i] = 1;

        std::string slash;
        if (ss >> slash) {
            if (slash == "/") {
                std::string rest;
                getline(ss, rest);

                std::stringstream ss2(rest);
                std::string ex;

                while (getline(ss2, ex, ',')) {
                    // убрать пробелы
                    while (!ex.empty() && ex[0] == ' ') ex.erase(0, 1);
                    while (!ex.empty() && ex.back() == ' ') ex.pop_back();

                    int idx = findPerson(people, peopleCount, ex.c_str());
                    if (idx != -1) {
                        included[idx] = 0;
                    }
                }
            }
        }

        int cnt = 0;
        for (int i = 0; i < peopleCount; i++) {
            if (included[i]) cnt++;
        }

        double share = amount / cnt;

        for (int i = 0; i < peopleCount; i++) {
            if (included[i]) {
                people[i].spent += share;
            }
        }
    }

    fclose(input);

    // ---- вывод ----
    for (int i = 0; i < peopleCount; i++) {
        fprintf(output, "%s %.1f %.1f\n",
                people[i].name,
                people[i].paid,
                people[i].spent);
    }

    fprintf(output, "\n");

    // ---- балансы ----
    double balance[MAX_PEOPLE];
    for (int i = 0; i < peopleCount; i++) {
        balance[i] = people[i].paid - people[i].spent;
    }

    int debtors[MAX_PEOPLE], creditors[MAX_PEOPLE];
    double debtAmt[MAX_PEOPLE], credAmt[MAX_PEOPLE];
    int dCount = 0, cCount = 0;

    for (int i = 0; i < peopleCount; i++) {
        if (balance[i] < -0.001) {
            debtors[dCount] = i;
            debtAmt[dCount++] = -balance[i];
        } else if (balance[i] > 0.001) {
            creditors[cCount] = i;
            credAmt[cCount++] = balance[i];
        }
    }

    int d = 0, c = 0;
    while (d < dCount && c < cCount) {
        double t = (debtAmt[d] < credAmt[c]) ? debtAmt[d] : credAmt[c];

        fprintf(output, "%s %.1f %s\n",
                people[debtors[d]].name,
                t,
                people[creditors[c]].name);

        debtAmt[d] -= t;
        credAmt[c] -= t;

        if (debtAmt[d] < 0.001) d++;
        if (credAmt[c] < 0.001) c++;
    }

    fclose(output);

    printf("Done. Check output.txt\n");
    return 0;
}