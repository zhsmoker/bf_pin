// bf_pin.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <regex>
#include <chrono>
#include <thread>
#include <cstdio>
//#include <openssl/md5.h>
#include <openssl/evp.h>

using namespace std;

#define NUMPIN 8

struct ParamThread
{
    size_t current = 0;
    size_t max = 0;
    short status = 0;/// 0 - work, 1 - not find, 2 - find 
    string result = "Work";
};

string calculate_md5(const string& content) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    string output;
    EVP_DigestInit_ex2(context, md, NULL);
    EVP_DigestUpdate(context, content.c_str(), content.length());
    EVP_DigestFinal_ex(context, md_value, &md_len);
    EVP_MD_CTX_free(context);
    output.resize(md_len*2);
    for (size_t i = 0; i < md_len; ++i) sprintf_s(&output[i*2], md_len, "%02x", md_value[i]);
    return output;
}

void bft(int nt, vector<ParamThread>& pt, string md5)
{
    string temp_pin;
    string md5digest;

    bool f_find = false;
    while (pt[nt].current < pt[nt].max && !f_find)
    {
        temp_pin = to_string(pt[nt].current);
        while (temp_pin.length() < NUMPIN)
            temp_pin = "0" + temp_pin;

        if (calculate_md5(temp_pin) == md5)
        {
            pt[nt].status = 2;
            pt[nt].result = "Pin: " + temp_pin + "   :)";
        }
           
        for (size_t i = 0; i < pt.size(); i++)
            if (pt[i].status == 2) f_find = true;

        pt[nt].current++;
    }

    if (!f_find || (f_find && pt[nt].status != 2))
    {
        pt[nt].status = 1;
        pt[nt].result = "Pin: not found   :( ";
    }
}

int main(int argc, char* argv[])
{
    const regex r("^[a-f0-9]{32}$");
        
    string md5;
    string temp_str;
    string temp_pin = "00000000";

    if (argc > 1) {
        temp_str = argv[1];
    }
    else {
        cout << "Enter the md5 hash: (example - 65522f4e85d0be0399d821ddcda26d01)" << "\n";
        cin >> temp_str;
    }
     
    for (size_t i = 0; i < strlen(temp_str.data()); i++)
        md5+=tolower(temp_str[i]);

    //cout << md5 << "\n";

    if (regex_match(md5.data(), r)) {
        cout << "Check md5 !";
    }
    else {
        cout << "\nThe entered value is not an md5 hash !" << "\n\n";
        system("pause");
        return 0;
    }

    cout << "\nWorking..." << "\n\n";

    int count_thread = thread::hardware_concurrency();
    vector<ParamThread> pt(count_thread);
    vector<thread> th;
    int part = (int)pow(10, NUMPIN) / count_thread;
    
    clock_t begin_time = clock();
    for (size_t i = 0; i < pt.size(); i++)
    {
        pt[i].current = part * i;

        if (i == pt.size() - 1) pt[i].max = (int)pow(10, NUMPIN)-1;
        else pt[i].max = part * (i + 1);

        //bft(i, pt, md5);
        th.push_back(thread(bft, i, ref(pt), md5));
    };

    bool f_find = true;
    while (f_find)
    {
        system("cls");
        cout << "Working..." << "\n\n";
        for (size_t i = 0; i < pt.size(); i++)
        {
            int percent = 100 - ((pt[i].max - pt[i].current) / (part / 100));
            string tpin = to_string(pt[i].current);
            while (tpin.length() < NUMPIN) tpin = "0" + tpin;
            
            cout << "Thread[" << i << "] ";
            cout << tpin << " - " << pt[i].max << /*"\t" << pt[i].status <<*/ "\t" << pt[i].result;
            //cout << " [";
            //for (size_t i = 0; i < percent / 5; i++) cout << "=";
            //for (size_t i = 0; i < 100-percent / 5; i++) cout << " ";
            //cout << "]";
            cout << " " << percent << " %\n";
            
            if (pt[i].status == 2) f_find = false;
        }
        cout << "\n";
        this_thread::sleep_for(chrono::seconds(5));
    }

    for (size_t i = 0; i < th.size(); i++)
        th[i].join();

    cout << "Work time:  " << (clock() - begin_time)/CLOCKS_PER_SEC << " sec.\n";
    
    //6e43eb106cc2fef12235527e05abc129 - 00150000
    //7e7de0bed2f111ca36f9756864a86b5c - 00500000  6 sec. 1 t
    //c981605d1a34f91b9ecc8a23ffd14f84 - 01500000 
    //c981605d1a34f91b9ecc8a23ffd14f84 - 26500000 1
    //3dc3bd78a53c4949d08f5c73a49c371e - 30500000
    //d1ca3aaf52b41acd68ebb3bf69079bd1 - 10000000  130 sec. 1 t
    //5eceadafba9f7df05d245049d9d2de4e - 20000000  6 sec. 1 t
    //ef775988943825d2871e1cfa75473ec0 - 99999999
    system("pause");
    return 0;
}

