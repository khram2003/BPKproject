# BPKproject

## Установка
Чтобы скачать проект себе используйте `git clone <url>`

## Запуск сервера
Сервер запускается удаленно, если вы хотите использовать свой сервер, склонируйте проект туда и соберите [server](server):
```
cmake .
make server
./server/server
```

## База данных
В проекте используется PostgreSQL, установите эту СУБД на вашем сервере и измените подключение к БД в [server.cpp](server/server.cpp)

## Сборка под Windows
Если у вас Windows поменяйте в [CMakeLists.txt](CMakeLists.txt) пути до `QT_MINGW` и `MINGW`

## Авторы
Contributors: `@alexbuyan`, `@parseny`, `@khram`

Mentor: `@vasalf`
