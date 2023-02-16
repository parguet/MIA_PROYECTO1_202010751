#include <iostream>
#include <regex>
#include <queue>
using namespace std;
void addToken(string cadena,string tipo);
void analizarTipo(string comando);
void readTokens(string comando);
int main() {
    cout<<" ---- Bienvenido al Sistema de Archivos de  - 202010751 ---- "<<endl;
    string comando;
    cout << "Ingresa un comando: "<< endl;
    getline(cin,comando);

}

struct token {
    string value;
    string tipo;
};

queue<token> colaTokens;

void analizarTipo(string comando){
    regex mkdisk("[m|M][k|K][d|D][i|I][s|S][k|K]");
    if(regex_search(comando,mkdisk)==1){
        comando = regex_replace(comando, mkdisk, "");
        cout<<" ---- Se dectecto mkdisk ---- "<<endl;
        cout<<comando<<endl;
        readTokens(comando);

    }
}


void readTokens(string comando){
    string buffer = "";
    comando += " ";
    queue<token> instance;
    colaTokens = instance;
    int estate = 0;
    string texto = "";
    for(int i=0; i<comando.length(); i++){
        texto+= comando[i];
        char c = comando[i];
        if (estate == 0){
            if (isalpha(c) or c == '*' or c == '?'){
                estate = 1;
                buffer+= tolower(c);
            }else if (isdigit(c)){
                estate = 2;
                buffer+= c;
            }else if (c == '"'){

                estate = 4;
                //buffer+= c;

            }else if (c == '/' or  c == '.'){
                estate = 8;
                buffer+= c;
            }else if (isspace(c)){
                estate = 0;
                buffer = "";
            }else if (c == '-') {
                estate = 9;
                buffer+= c;
            }else if (c == '=') {
                buffer+= c;
                addToken(buffer, "IGUAL");
                buffer = "";
            }else{
                cout<<"Erro lexico"<<endl;
            }

        }else if (estate == 1){
            if (isalpha(c) || isdigit(c) || c == '_' || c == '.' || c == '*' ){
                buffer+= tolower(c);
            }else{
                addToken(buffer, "ID");
                buffer = "";
                estate = 0;
                i--;
            }
        }
        else if (estate == 2){
            if (isdigit(c)  ){
                buffer+=c;
            }else if(c == '.'){
                estate = 6;
                buffer+=c;
            }else{
                if(isalpha(c)){
                    estate = 1;
                    buffer+=c;
                }else{
                    addToken(buffer, "NUMERO");
                    buffer = "";
                    estate = 0;
                    i--;
                }

            }
        }
        else if (estate == 4){
            if(c != '"'){
                buffer+=c;
            }else{
                //buffer+= c;
                addToken(buffer, "CADENA");
                buffer = "";
                estate = 0;
            }
        }
        else if (estate == 6){
            if (isdigit(c)){
                buffer+=c;
            }else{
                addToken(buffer, "NUMERO");
                buffer = "";
                estate = 0;
                i--;
            }
        }
        else if(estate == 8){
            if (!isspace(c)){
                buffer+=c;
            }else{
                addToken(buffer, "RUTA");
                estate = 0;
                buffer = "";
            }
        }else if(estate == 9){
            if (isdigit(c)){
                estate = 2;
                buffer+=c;
            }else if(isalpha(c)){
                addToken(buffer, "GION");
                estate =0;
                i--;
                buffer = "";
            }
        }
    }
}

void addToken(string cadena,string tipo){
    token tk ;
    tk.value = cadena;
    tk.tipo = tipo;
    colaTokens.push(tk);
}