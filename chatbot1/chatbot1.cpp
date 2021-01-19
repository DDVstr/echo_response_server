// chatbot1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <uwebsockets/App.h>
 

using namespace std;

const string SET_NAME = "SET_NAME::";
const string DIRECT = "DIRECT::"; 


// вернет true, если команда установления имени 
bool SET_NAME_COMMAND(string_view message)
{
   return  message.find(SET_NAME)==0;
}
//убираем часть имени (возвращает только имя )
string parseName(string_view message) 
    { 

       return   string(message.substr(SET_NAME.length())); //message -это лямбда, которая обращается соответсвенно к функциям 
    }

bool DIRECT_COMMAND(string_view message)
{
    message.find(DIRECT) == 0; 

}

int   parseRecieverID(string_view message)
{
    string_view rest = message.substr(DIRECT.length());

    int pos = rest.find("::"); 
    string_view text = rest.substr(pos+2); 
    return string(text);

}

 


int main() {
    //структура  данных, которая хранит информацию о пользователе в чате
    //хранится в памяти
    struct PerSocketData {
        
        int user_id;
        std::string name; 

    };

    int last_user_id = 10; 
     

   //создается приложение SSL -протокол шифрования 
    //здесь создается новый сервер 

    uWS::App()
         .ws<PerSocketData>("/*", {//будем хранить информацию о пользователях в сокетдата
            /* Settings */

        //настройки сетевой части 

            .compression = uWS::SHARED_COMPRESSOR,
            .maxPayloadLength = 16 * 1024,
            .idleTimeout = 10,
            .maxBackpressure = 1 * 1024 * 1024,
            /* Handlers */

            //обработчики, open при открытии соединения 
            //message вызывается при получении сообщения от пользователя, сообщение в тех смысле(кому, куда данные передаются )

            .upgrade = nullptr,
            .open = [&last_user_id](auto*connection ) {   
        //назначить пользователю уникальный номер 

       
            std::cout << "New connection created"<<std::endl;
            PerSocketData* userdata =  (PerSocketData*) connection->getUserData();// приведение типов 
            userdata->user_id = last_user_id++;
            userdata->name = "Unnamed";


            connection->subscribe("broadcast");
            connection->subscribe("user#" + to_string(userdata->user_id));
    },

    .message = [](auto* connection, std::string_view message, uWS::OpCode opCode) {
       
        std::cout << "new message recieved"<<std::endl<<message;
        PerSocketData* userdata = (PerSocketData*)connection->getUserData();
        if (SET_NAME_COMMAND(message))
        {
            //нужно имя записать куда следует
            cout << "User set his name"; 
            parseName(message); 

        }

        if (DIRECT_COMMAND(message))
        {
            cout << "user sent direct message"; 

            //записать имя куда нужно 

        }
    },
  
   //метод, которыевключатеся, когда пользователь выходит с сервера 

    .close = [](auto*/*ws*/, int /*code*/, std::string_view /*message*/) {
        /* You may access ws->getUserData() here */
        std::cout << "connection closed" << std::endl;
    }
            }).listen(9001, [](auto* listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port " << 9001 << std::endl;
        }
    }).run();
}
//план серевера 


// 1. уникальный номер пользователя
// 2. номер пользователя( Имя пользователя )

//SET_NAME::MIKE 

//кому-то написать 
 //MESSAGE - это командв 
//DIRECT::user_id::message_text






//сокет сетевая абстракция, в который можно отправить информацию или ее получить 