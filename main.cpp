#include<bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

const int disk_size = 500000000;
const int block_size = 10240;

vector<bool> block_bitmap;
unordered_map<string, int> open_files;
unordered_map<int, string> open_files_rev;
vector<bool> fd_bitmap;

class block
{
    public:
	char filename[50];
	int start_index;
	int filesize;
	int mode;
}files[49];

FILE *disk;


bool disk_on=false;

string create_disk(string diskname)
{
	if(access(diskname.c_str(), F_OK)!=-1)
	{
		return "Disk already exists";
	}
	disk = fopen(diskname.c_str(), "wb");
	char ch[block_size];
	for(int i =0;i<disk_size/block_size;i++)
	{
		fwrite(ch,1,block_size,disk);
	}
	block_bitmap.push_back(true);
	for(int i=1;i<=48;i++)
	{	
		block_bitmap.push_back(false);
	}
	for(int i=0;i<=49;i++)
	{	
		fd_bitmap.push_back(false);
	}
	for(int i=0;i<49;i++)
	{
		strcpy(files[i].filename, "noname");
		files[i].filesize = 0;
		files[i].mode = -1;
		files[i].start_index = 0;
	}
	fseek(disk,0,SEEK_SET);
	for(int i=0;i<49;i++)
		fwrite(&files[i],sizeof(block),1,disk);
	fclose(disk);
	return "Disk Created Successfully";
}
string mount_disk(string diskname)
{
	disk = fopen(diskname.c_str(), "rb+");
	if(disk==NULL)
	{
		return "Disk not exists";
	}
	fseek(disk,0,SEEK_SET);
	for(int i=0;i<49;i++)
		fread(&files[i],sizeof(block),1,disk);
	disk_on =  true;
	for(int i=0;i<=49;i++)
	{	
		fd_bitmap.push_back(false);
	}
	return "Disk Mounted Successfully";
}
string create_file(string filename)
{
	int i=1;
	for(i=1;i<49;i++)
	{
		if(strcmp(files[i].filename,"noname")==0)
		{
			break;	
		}
		if(strcmp(files[i].filename,filename.c_str())==0)
		{
			return "File Already Exists";
		}
	}
	strcpy(files[i].filename, filename.c_str());
	files[i].filesize = 0;
	files[i].start_index = i*10240*10240;
	files[i].mode = -1;
	return "File Created Successfully!!!!!!!";
}
string open_file(string filename)
{
	int i=1;
	for(i=1;i<49;i++)
	{
		if(strcmp(files[i].filename,filename.c_str())==0)
		{
			break;
		}
	}
	if(i==49)
	{
		return "File not found";
	}
	if(files[i].mode != -1)
	{
		return "File already opened";
	}
	cout<<"0. Read Mode"<<endl;
	cout<<"1. Write Mode"<<endl;
	cout<<"2. Append Mode"<<endl;
	cout<<"Enter mode : ";
	cin>>files[i].mode;
	int fd=1;
	while(fd_bitmap[fd]==true)
	{
		fd++;
	}
	string res = "File " + filename + " has been opened in [ " + to_string(files[i].mode) + " ] mode with file descriptor " + to_string(fd);
	open_files[filename] = fd;
	open_files_rev[fd] = filename;
	fd_bitmap[fd] = true;
	return res;
}
string read_file(int fd)
{
	string fname = open_files_rev[fd];
	if(fname=="")
		return "File not Opened";
	int i;
	for(i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,fname.c_str())==0)
			break;
	}
	if(files[i].mode !=0)
	{
		return "File not open in Read Mode";
	}
	fseek(disk,files[i].start_index, SEEK_SET);
	char str[30000];
	fread(&str,sizeof(char),sizeof(str),disk);
	printf("%s",str);
	cout<<endl<<endl;
	return "File Readed Successfully";
}
string write_file(int fd)
{
	string fname = open_files_rev[fd];
	if(fname=="")
		return "File not Opened";
	int i;
	for(i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,fname.c_str())==0)
			break;
	}
	if(files[i].mode !=1)
	{
		return "File not open in Write Mode";
	}
	char str[30000];
    	cout<<"Enter Content : "<<endl;
    	cin.getline(str,30000,'$');
    	int j =0;
  	while(str[j++]!='*');
  	files[i].filesize = j;
	fseek(disk,files[i].start_index, SEEK_SET);
	fwrite(str,sizeof(char),sizeof(str),disk);
	return "File Written Successfully";
}
string append_file(int fd)
{
	string fname = open_files_rev[fd];
	if(fname=="")
		return "File not Opened";
	int i;
	for(i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,fname.c_str())==0)
			break;
	}
	if(files[i].mode !=2)
	{
		return "File not open in Append Mode";
	}
	char str[30000];
    	cout<<"Enter Content : "<<endl;
    	cin.getline(str,30000,'$');
	fseek(disk,files[i].start_index+files[i].filesize-1, SEEK_SET);
	fwrite(str,sizeof(char),sizeof(str),disk);
	int j =0;
  	while(str[j++]!='*');
  	files[i].filesize += j;
	return "File Appended Successfully";
}
string close_file(int fd)
{
	string fname = open_files_rev[fd];
	if(fname=="")
		return "File not Opened";
	int i;
	for(i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,fname.c_str())==0)
			break;
	}
	if(files[i].mode ==-1)
	{
		return "File Already closed";
	}
	files[i].mode = -1;
	fd_bitmap[fd] = false;
	open_files.erase(fname);
	open_files_rev.erase(fd);
	
	return "File Closed Successfully";
}
string delete_file(string filename)
{
	int i;
	for(i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,filename.c_str())==0)
			break;
	}
	if(i==48)
		return "File not found";
	if(files[i].mode !=-1)
	{
		return "File is open state";
	}
	int j;
	for(int j=i;j<48;j++)
	{
		if(strcmp(files[j].filename,"noname")==0)
			break;
		strcpy(files[j].filename, files[j+1].filename);
		files[j].filesize = files[j+1].filesize;
		files[j].mode = files[j+1].mode;
		files[j].start_index = files[j+1].start_index;
	}	
	strcpy(files[j].filename, "noname");
	files[j].filesize = 0;
	files[j].mode = -1;
	files[j].start_index = 0;
	return "File Deleted Successfully";
}
void list_files()
{
	for(int i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,"noname")==0)
		{
			break;
		}
		cout<<files[i].filename<<endl;
	}
	cout<<endl<<endl;
	return;
}
void list_open_files()
{
	for(int i=1;i<48;i++)
	{
		if(strcmp(files[i].filename,"noname")==0)
		{
			break;
		}
		else if(files[i].mode !=-1)
		{
			cout<<files[i].filename<<" file is opened in [ "<<files[i].mode<<" ] mode with file Descriptor "<<open_files[string(files[i].filename)]<<endl;
		}
	}
	cout<<endl<<endl;
	return;
}
string unmount()
{	
	for(int i=0;i<49;i++)
		files[i].mode = -1;
	fseek(disk, 0, SEEK_SET);
	for(int i=0;i<49;i++)
		fwrite(&files[i],sizeof(block),1,disk);

	block_bitmap.clear();
	open_files.clear();
	open_files_rev.clear();
	fd_bitmap.clear();
	
	fclose(disk);
	disk_on = false;
	return "Unmounted Successfully!!!!!!!";
}
void disk_operation()
{
	while(true)
	{
		cout<<"1. Create File"<<endl;
		cout<<"2. Open File"<<endl;
		cout<<"3. Read File"<<endl;
		cout<<"4. Write File"<<endl;
		cout<<"5. Append File"<<endl;
		cout<<"6. Close File"<<endl;
		cout<<"7. Delete File"<<endl;
		cout<<"8. List of Files"<<endl;
		cout<<"9. List of Open Files"<<endl;
		cout<<"10. Unmount"<<endl;
		cout<<"Enter Choice : ";
		int ch;
		cin>>ch;
		if(ch==1)
		{
			string filename;
			cout<<"Enter filename : ";
			cin>>filename;
			cout<<endl<<endl;
			cout<<create_file(filename)<<endl<<endl;
		}
		if(ch==2)
		{
			string filename;
			cout<<"Enter filename : ";
			cin>>filename;
			cout<<endl<<endl;
			cout<<open_file(filename)<<endl<<endl;
		}
		if(ch==3)
		{
			int fd;
			cout<<"Enter file Descriptor : ";
			cin>>fd;
			cout<<endl<<endl;
			cout<<read_file(fd)<<endl<<endl;
		}
		if(ch==4)
		{
			int fd;
			cout<<"Enter file Descriptor : ";
			cin>>fd;
			cout<<endl<<endl;
			cout<<write_file(fd)<<endl<<endl;
		}
		if(ch==5)
		{
			int fd;
			cout<<"Enter file Descriptor : ";
			cin>>fd;
			cout<<endl<<endl;
			cout<<append_file(fd)<<endl<<endl;
		}
		if(ch==6)
		{
			int fd;
			cout<<"Enter file Descriptor : ";
			cin>>fd;
			cout<<endl<<endl;
			cout<<close_file(fd)<<endl<<endl;
		}
		if(ch==7)
		{
			string filename;
			cout<<"Enter file names : ";
			cin>>filename;
			cout<<endl<<endl;
			cout<<delete_file(filename)<<endl<<endl;
		}
		if(ch==8)
		{
			cout<<endl<<endl;
			list_files();
		}
		if(ch==9)
		{
			cout<<endl<<endl;
			list_open_files();
		}
		if(ch==10)
		{
			cout<<endl<<endl;
			cout<<unmount()<<endl<<endl;
			break;
		}
	}
}
int main()
{	 
	while(true)
	{
		int ch;
		string diskname;
		cout<<"1. Create Disk"<<endl;
		cout<<"2. Mount Disk"<<endl;
		cout<<"3. Exit"<<endl;
		cout<<"Enter choice : ";
		cin>>ch;
		if(ch==1)
		{
			cout<<"Enter Disk Name : ";
			cin>>diskname;
			cout<<endl;
			cout<<create_disk(diskname)<<endl<<endl;
		}
		else if(ch==2)
		{
			cout<<"Enter Disk Name : ";
			cin>>diskname;
			cout<<endl;
			cout<<mount_disk(diskname)<<endl<<endl;
			if(disk_on)
			{
				disk_operation();
			}
		}
		else if(ch==3)
		{
			cout<<"Thank for using the the File System!!!!!"<<endl<<endl;
			break;
		}
		else 
			cout<<"Please enter valid choice!!!!!!!"<<endl<<endl;		
	}
	return 0;
}
