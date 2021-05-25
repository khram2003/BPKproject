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
Лучше устанавливать QT там же, где вы ставили компилятор. Например, из-за того что мы использовали MSYS2 MINGW и QT с официального сайта, наш проект не собирался. Для решения проблемы поставьте QT через MSYS2 командой `pacman -S mingw-w64-x86_64-qt6-base` и в `Панель Управления -> Система -> Дополнительные параметры системы -> Переменные среды` установите `QT_QPA_PLATFORM_PLUGIN=C:/msys64/mingw64/share/plugins/platforms` (Возможно, на вашей системе он будет немного отличаться)

## Используемые библиотеки и фреймворки
[QT](https://www.qt.io/ "QT")

[websocketpp](https://github.com/zaphoyd/websocketpp/ "websocketpp GitHub repository")

[json.hpp](https://github.com/nlohmann/json "nlohmann/json GitHub repository")

## Авторы
Contributors: [@alexbuyan](https://github.com/alexbuyan "Alexander Buyantuev"), [@parseny](https://github.com/parseny "Arsenii Pimenov"), [@khram](https://github.com/khram "Nikita Khramov")

Mentor: [@vasalf](https://github.com/vasalf "Vasily Alferov")
