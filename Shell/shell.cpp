#include <iostream>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <cstring>
#include <fstream>

using namespace std; 

map <string,pair<int,int>> new_Comm; 
string now; 
ofstream file;
int c = 0;			

void clear();
string read();
vector<const char*> parsing(string);
void sig_handler(int);
bool hasPipes(vector<const char*>);
void execPipes(vector<const char*>);
void exec(vector<const char*>);
void create_new(char **, int);
void sig_handler2(int);

void clear(){
	system("@cls||clear");
}

string read(){
	string s = "";
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	while(s.size()==0){
		cout << cwd << " $ "; 
		getline(cin, s);
		if(s=="clear"){
			clear();
			s="";
		} 
	}
	if(s=="exit" || s=="EXIT") exit(0);
	return s; 
}


vector<const char*> parsing(string s){
	vector<const char*> args; 
	vector<string> commands;
	string aux = ""; 
	for(auto i : s){
		if(i != ' ' ) aux+=i;
		else{
			commands.push_back(aux);
			aux = "";
		}
	}
	commands.push_back(aux);
	

	for(int i = 0; i < commands.size(); i++){
		args.push_back(commands[i].c_str());
	}

	return args;
}


void sig_handler(int sig){
    char c=1;
    while(c!='Y' || c!='y' || c!='n' || c!='N'){
     	cout << "\nRealmente quieres salir? [y/n] ";
     	cin >> c;
     	if (c == 'y' || c == 'Y') exit(0);
     	else {
     		return;
     	}
     	
     }
 }


bool hasPipes(string arg){
	for (auto i : arg){
		if(i == '|') return true;
	}
	return false;
}

bool hasCmdmonset(string arg){
	string ans ="cmdmonset"; 
	if(strstr(arg.c_str(),ans.c_str() )) return true;
	return false;
}

void execPipes(string s){
	int count = 0;
	for(auto i : s){
		if(i == '|') count++; 
	}

	int pipes[2*count];
	for (int i = 0; i < count; ++i){
		if(pipe(pipes + i*2) < 0){
			perror("pipe error");
			exit(EXIT_FAILURE);
		}	
	}

	vector<string> commands;
	string aux="";
	for(int i = 0; i < s.size(); i++){
		if(s[i]!='|')aux+=s[i];
		else{
			aux.pop_back();
			commands.push_back(aux);
			aux = "";
			i++;
		}
	}
	commands.push_back(aux);

	int j = 0;
	int pid;
	int index = 0;
	for(auto i : commands){
		vector<string> cmd;
		aux="";
		for(int j = 0; j < i.size(); j++){
			if(i[j]!=' ')aux+=i[j];
			else{
				cmd.push_back(aux);
				aux = "";
			}
		}
		cmd.push_back(aux);
		char *cmdPtr[cmd.size()+1];
		cmdPtr[cmd.size()] = NULL;
		for(int j = 0; j < cmd.size(); j++){
			cmdPtr[j] = strdup(cmd[j].c_str());
		}

		pid = fork();
		if(pid < 0){
			cout << "error" << endl;
		}
		else if(pid == 0){
			if(index < commands.size()-1){
				if(dup2(pipes[j+1],1) < 0){
					perror("dup2");
					exit(EXIT_FAILURE);
				}
			}
			if(j!=0){
				if(dup2(pipes[j-2],0)<0){
					perror(" dup2");
					exit(EXIT_FAILURE);
				}
			}
			for(int k = 0; k < 2*count; k++){
				close(pipes[k]);
			}
			if(execvp(cmdPtr[0], cmdPtr) < 0){
				perror("execvp error");
				exit(EXIT_FAILURE);
			}
		}
		j+=2;
		index++;
	}
	for(int i = 0; i < 2 * count; i++){
		close(pipes[i]);
	}
	for(int i = 0; i < count+1; i++){
		wait(NULL);
	}

}

void exec(string com){
	now = "";
	vector<const char*> arg = parsing(com);
	char *args[arg.size()+1];
	for(int i = 0; i < arg.size(); i++){
		args[i] = (char*)arg[i];
	}
	args[arg.size()]=NULL;
	
	if(hasPipes(com)) execPipes(com);

	else if (hasCmdmonset(com)){
		if(arg.size()== 4){
			new_Comm.insert({arg[1], pair <int,int> (atoi(arg[2]),atoi(arg[3]))});
		}
		else {
			cout << "wrong format" << endl;
		}
	}
	else if(new_Comm.count(args[0])>0){
		now = com; 
		c = 0;
		file.open("log.txt");
		clear();
		signal(SIGALRM, sig_handler2);
		alarm(1);
	}
	else{
		int pid = fork();
		if(pid<0) cout << "error" << endl;
		else if (pid == 0) {
			execvp(args[0], args);
			perror("comando no existente");
			exit(EXIT_FAILURE);	
		}
		else{
			wait(NULL);
		}
	}
	return;
}

void sig_handler2(int sig) {
	vector<const char*> arg = parsing(now);
	char *args[arg.size()+1];
	for(int i = 0; i < arg.size(); i++){
		args[i] = (char*)arg[i];

	}
	args[arg.size()]=NULL;

	//args[0];

    
    cerr << endl << "writing in log.txt..." << endl;
    int p[2];
    pipe(p);
    int pid = fork();
	if(pid<0) cout << "error" << endl;
	else if (pid == 0) {
		close(p[0]);
		dup2(p[1],1);
		dup2(p[1],2);
		close(p[1]);

		char *command[2];
		command[0] = strdup("vmstat");
		command[1] = NULL;

		execvp(command[0], command);
		perror("comando no existente");
		exit(EXIT_FAILURE);	
	}
	else{
		char buffer[1024] = {0};
		close(p[1]);

		while(read(p[0], buffer, sizeof(buffer)) != 0){
			cerr << buffer << endl;
			file << buffer << '\n';
		}

		wait(NULL);
	}

    if (++c < new_Comm[args[0]].second / new_Comm[args[0]].first ) alarm(new_Comm[args[0]].first); // vuelve a setear alarma en 1 seg
    else {
		printf("Press any key for continue\n");
		file.close();
		
    }
}




int main(int argc, char const *argv[]){
	clear();
	while(1){
		signal(SIGINT, sig_handler);
		//clear();
		exec(read());
		
	}
	return 0;
}