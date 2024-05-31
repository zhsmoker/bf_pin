// bf_pin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <regex>
#include <chrono>
#include <thread>
#include <openssl/md5.h>

using namespace std;

#define NUMPIN 8

void bft(int& result, int current, int end, const unsigned char* md5)
{
    unsigned char t_pin[NUMPIN];
    unsigned char md5digest[MD5_DIGEST_LENGTH];
    
    int num = current;
    int i;

    for (i = NUMPIN; i > 0; i--, num /= 10)
        t_pin[i - 1] = (num % 10) + '0';

    bool f_find = false;

    while (current <= end && result>99999999)
    {
        
#pragma warning(suppress : 4996)
        MD5(t_pin, NUMPIN, md5digest);

        f_find = true;
        for (i = 0; i < MD5_DIGEST_LENGTH && f_find; i++) {
            if (md5digest[i] != md5[i])
                f_find = false;
        }

        if (f_find)
        {
            result = current;
            return;
        }

        for (i = NUMPIN; i > 0; i--) {
            if (t_pin[i - 1] == '9')
                t_pin[i - 1] = '0';
            else {
                t_pin[i - 1]++;
                break;
            }
        }

        current++;
    }

    return;
}

int main(int argc, char* argv[])
{
    const regex r("^[a-f0-9]{32}$");
        
    string md5;
    string temp_str;
    unsigned char tempmd5[MD5_DIGEST_LENGTH];

    if (argc > 1) {
        temp_str = argv[1];
    }
    else {
        cout << "Example: md5 hash = pin" << "\n";
        cout << "d1ca3aaf52b41acd68ebb3bf69079bd1 = pin 10000000" << "\n";
        cout << "ef775988943825d2871e1cfa75473ec0 = pin 99999999" << "\n";
        cout << "Enter the md5 hash: ";
        cin >> temp_str;
    }
     
    for (size_t i = 0; i < strlen(temp_str.data()); i++)
        md5+=tolower(temp_str[i]);

    if (regex_match(md5.data(), r)) {
        cout << "Check md5 !" << "\n";
    }
    else {
        cout << "\nThe entered value is not an md5 hash !" << "\n\n";
        system("pause");
        return 0;
    }
    
    clock_t begin_time = clock();

    int i1, i2, b1, b2;
    for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        i1 = int(md5[i*2]);
        i2 = int(md5[i*2 + 1]);
        b1 = (i1 >= 0x30 && i1 <= 0x39) ? i1 - 0x30 : i1 - 0x57;
        b2 = (i2 >= 0x30 && i2 <= 0x39) ? i2 - 0x30 : i2 - 0x57;
        tempmd5[i] = (b1 << 4) | b2;
    }

    int result = (int)pow(10, NUMPIN) + 77;
    int count_thread = thread::hardware_concurrency();
    
    cout << "Working (thread count - " << count_thread << " ) ..." << "\n";
    
    int part = (int)pow(10, NUMPIN) / count_thread;

    vector<thread> th;
    int current, max;
    for (int i = 0; i < count_thread; i++)
    {
        current = part * i;

        if (i == count_thread - 1) max = (int)pow(10, NUMPIN)-1;
        else max = part * (i + 1);

        th.push_back(thread(bft, ref(result), current, max, tempmd5));
    };

    for (int i = 0; i < th.size(); i++)
        th[i].join();

    string p = to_string(result);

    if (result < 100000000)
    {
        while (p.length() < NUMPIN)
            p = "0" + p;
        cout << "Find pin: " << p << endl;
    }
    else 
        cout << "Pin not found" << endl;


    //this_thread::sleep_for(chrono::seconds(5));
    clock_t end_time = clock();

    double time_spent = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
    printf("Work time is %f seconds.\n", time_spent);

    
    //dd4b21e9ef71e1291183a46b913ae6f2 - 00000000
    //6e43eb106cc2fef12235527e05abc129 - 00150000
    //7e7de0bed2f111ca36f9756864a86b5c - 00500000
    //c981605d1a34f91b9ecc8a23ffd14f84 - 01500000 
    //c981605d1a34f91b9ecc8a23ffd14f84 - 26500000
    //3dc3bd78a53c4949d08f5c73a49c371e - 30500000
    //d1ca3aaf52b41acd68ebb3bf69079bd1 - 10000000 ver 1 - 50 sec. ver 2 - 39 sec.
    //5eceadafba9f7df05d245049d9d2de4e - 20000000
    //ef775988943825d2871e1cfa75473ec0 - 99999999
    
    system("pause");
    return 0;
}

