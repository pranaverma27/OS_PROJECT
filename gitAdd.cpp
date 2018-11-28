#include<stdio.h>
#include<string>
#include <openssl/sha.h>
// #include "generateSHA.cpp"
#include "indexCreate.cpp"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include<fstream>
#include<iostream>
#include<stdlib.h>
#include <sstream>

using namespace std;

#define deb(x) cout<<"Checkpoint "<<x<<endl

class TestA
{
    private:
        char type[10];
        char header[50];
        char content[1000000];
        int length;
        

    public:
        // TestA(){type = "";header="";content=""; length=0; }
        string gitAdd(string fileName);

        void Print_Type()
        {
            cout<<type<<endl;
        }

        void Print_Content()
        {
            int len = sizeof(content)/sizeof(char);
            // cout<<"len: "<<len<<endl;
            cout<<content;
        }

        // void setContent(char *con){
        //     content=con;
        // }

        char * getContent(){
            return content;
        }
};

struct blobObj{
    string type;
    string hash;
};

string TestA::gitAdd(string fileName)
{
    FILE *fptr;
  
    // char buffer[512];
    ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);

    ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, ios::beg);

    vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);
    // cout<<"filesize: "<<fileSize<<endl; 
    // char* t = new char[(long long)(fileSize)+1];
    // content = t;
    string temp=string(bytes.data(), fileSize);
    strcpy(content, temp.c_str());
    // setContent(content);
    strcpy(type,"blob");
    string temptype=type;
    strcpy(header,(temptype+" "+to_string(fileSize)).c_str());
    string tempheader=header;
    string store = tempheader+ " " + content;
    string sha1=generateSHAstring(store);

    return sha1;
}

void serialize(string fileName){
    //cout<<"hello";
    TestA Test;
    string hash_filename=Test.gitAdd(fileName);

    struct blobObj b;
    b.hash=hash_filename;
    b.type="blob";
    string writeStr=b.hash+" "+b.type;

    std::ofstream ofs;
    ofs.open(".mygit/info/objectsFile.txt", std::ofstream::out | std::ofstream::app);
    ofs << writeStr;
    ofs.close();

    string directory="";
    string blobname="";
    for(int i=0;i<2;i++)
        directory+=hash_filename[i];
    mkdir((".mygit/objects/"+directory).c_str(),0777);
     cout<<directory<<endl;

    for(int i=2;i<40;i++)
        blobname+=hash_filename[i];

     cout<<blobname<<endl;
    // cout<<hash_filename;

    //adding entry into index
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    string pa(cwd);
    string path = pa + "/" + fileName;
    // cout<<path<<endl;
    char *cstr = new char[path.length() + 1];
    strcpy(cstr, path.c_str());
    char *cstr1 = new char[hash_filename.length() + 1];
    strcpy(cstr1, hash_filename.c_str());
    indexFill("100644",cstr1,0,cstr,0);
    

    // FILE *File = fopen((".mygit/objects/"+directory+"/"+blobname).c_str(),"wb");
    // // Test.Print_Content();
    // // cout<<Test.getContent();
    // fwrite((char *)&Test,sizeof(Test),1,File); //Treats the object as if it is a char array
    // fclose(File);

    ofs.open((".mygit/objects/"+directory+"/"+blobname).c_str(),ios::out|ios::binary);
    ofs.write((char *)&Test,sizeof(Test));
    ofs.close();

    
}

 TestA deserialize(string file){
     TestA Test2;
     // FILE *File;

     string directory="";
     string blobname="";

     deb(sizeof(Test2.getContent()));

     for(int i=0;i<2;i++)
        directory+=file[i];

    for(int i=2;i<40;i++)
        blobname+=file[i];
     string path=".mygit/objects/"+directory+"/"+blobname;
     // File = fopen(path.c_str(),"rb");
     // fread((char *)&Test2,sizeof(Test2),1,File); //Treats the object as if it is a char array
     ifstream ifs (path.c_str(),ios::in|ios::binary);
     ifs.read((char *)&Test2,sizeof(Test2));
     deb(23);
     Test2.Print_Type();
     Test2.Print_Content();
     deb(34);
     // fclose(File);
     return Test2;
 }
int main(){
    // string fileName="abc.txt";
    serialize("7");

    string hash;
    cout<<"enter hash: ";
    cin>>hash;
    deserialize(hash);
    return 0;
}