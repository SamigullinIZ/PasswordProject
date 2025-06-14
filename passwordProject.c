#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>

jmp_buf env; //буфер для сохранения состояния программы 

void disable_echo(int fd) {
    struct termios tattr;
    /* Получаем текущие настройки терминала */
    tcgetattr(fd, &tattr);
    /* Убираем echo и canonical mode */
    tattr.c_lflag &= ~ECHO;
    tattr.c_lflag &= ~ICANON;
    /* Применяем новые настройки сразу же */
    tcsetattr(fd, TCSANOW, &tattr);
}

void enable_echo(int fd) {
    struct termios tattr;
    /* Получаем текущие настройки терминала */
    tcgetattr(fd, &tattr);
    /* Включаем echo и canonical mode */
    tattr.c_lflag |= ECHO;
    tattr.c_lflag |= ICANON;
    /* Применяем новые настройки сразу же */
    tcsetattr(fd, TCSANOW, &tattr);
}

void signal_handler(int sig){
    enable_echo(STDIN_FILENO);
    longjmp(env, sig);
}


int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);

    // Try block
    int error_code = setjmp(env);
    if(error_code == 0){

        char password[17];
    int c, pos = 0;

    printf("Введите пароль: ");
    fflush(stdout);

    /* Сначала выключаем отображение символов */
    disable_echo(STDIN_FILENO);

    while (((c = getchar()) != EOF) && (pos < 16)) {
        if (c == '\n') break;       // Если введена новая строка, заканчиваем ввод
        
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z')) {
            password[pos++] = c;    // Записываем валидный символ
            putchar('*');           // Печать маскирующего символа
            fflush(stdout);
        } else if (c == '\b' || c == 127) {  // Удаление символа
            if (pos > 0) {
                pos--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
                fflush(stdout);
            }
        }
    }

    password[pos] = '\0';           // Завершение строки пароля
    printf("\n Пароль введен.\n");
    }else{
        //catch
        printf("\n Программа прервана сигналом %d\n", error_code);
    }

    

    /* Возвращаем терминал в нормальное состояние */
    enable_echo(STDIN_FILENO);

    return error_code;
}