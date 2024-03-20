#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>

#define ANSI_RESET_ALL     "\x1b[0m"
#define STYLE_BOLD         "\x1b[1m"
#define COLOR_RED          "\x1b[31m"
#define COLOR_GREEN        "\x1b[32m"
#define COLOR_YELLOW       "\x1b[33m"
#define COLOR_MAGENTA      "\x1b[35m"

#define MAX_ROUTES 20
#define MAX_ROUTE_LEN 77
#define ROUTE_PROPERTIES 7
#define END_INPUT "###"

#define TAG_NUM 0
#define TAG_PLANE 1
#define TAG_CITY 2
#define TAG_DAYS_OF_WEEK 3
#define TAG_TIME_FROM 4
#define TAG_TIME_TO 5
#define TAG_PRICE 6

/*
Разработка программного обеспечения для учета данных об авиарейсах № 3 Массив строк «Аэропорт»
1
3 boing777 Moscow 1256 10:30 10:50 16450
###

12 airbus230 Moscow 1256 14:10 14:20 6710
71 boing777 Moscow 125 6:45 6:55 1300
8 boing777 Novosibirsk 234 12:30 13:45 800
24 airbus230 Moscow 1256 21:35 21:50 7600
23 airbus230 Novosibirsk 1467 6:30 7:40 1940
52 boing777 Novosibirsk 2567 17:05 18:10 2500
###
*/


typedef struct Time { int hour, minute; };
typedef struct Route {
    int num;
    float price;
    char plane[20], city[20], days[10];
    Time timeFrom, timeTo;
};

int validateTime(Time time)
{
    return (time.hour >= 0 && time.hour < 24 && time.minute >= 0 && time.minute < 60);
}

int validatePrice(int price)
{
    return price > 0;
}

int validateDays(char days[10])
{
    int len = strlen(days),
        intdays = atoi(days);

    if (intdays > 10000000 - 1 || intdays < 1) return 0;
    // дней больше чем есть в недели

    for (int i = 0; i < len; i++)
    {
        if (days[i] < '1' || days[i] > '7') return 0;
        // дня недели не существует

        for (int j = 0; j < i; j++)
            if (days[i] == days[j]) return 0;
        // дни повторяются
    }
}

int serialiseRoute(char* str, Route* route)
{
    int validData = 1;
    char* word = strtok(str, " ,:");

    for (int property = 0; property < ROUTE_PROPERTIES; property++)
    {
        if (word == NULL)
        {
            validData = 0;
            //printf(COLOR_RED "Error: bad input!\n" ANSI_RESET_ALL);
            break;
        }
        else
            switch (property) {
            case TAG_NUM: {
                (*route).num = atoi(word);
                break;
            }
            case TAG_PLANE: {
                strcpy((*route).plane, word);
                break;
            }
            case TAG_CITY: {
                strcpy((*route).city, word);
                break;
            }
            case TAG_DAYS_OF_WEEK: {
                if (validateDays(word))
                    strcpy((*route).days, word);
                else
                {
                    validData = 0;
                    //printf(COLOR_RED "Error: bad input days!" ANSI_RESET_ALL "\n");
                }
                break;
            }
            case TAG_TIME_FROM: {
                Time time;
                time.hour = atoi(word);
                word = strtok(NULL, " :");
                if (word == NULL)
                {
                    validData = 0;
                    //printf(COLOR_RED "Error: bad input time!\n" ANSI_RESET_ALL);
                    break;
                }
                time.minute = atoi(word);

                if (validateTime(time))
                    (*route).timeFrom = time;
                else
                {
                    //printf(COLOR_RED "Error: bad input departure time!" ANSI_RESET_ALL "\n");
                    validData = 0;
                }
                break;
            }
            case TAG_TIME_TO: {
                Time time;
                time.hour = atoi(word);
                word = strtok(NULL, " :");
                if (word == NULL)
                {
                    validData = 0;
                    //printf(COLOR_RED "Error: bad input time!\n" ANSI_RESET_ALL);
                    break;
                }
                time.minute = atoi(word);

                if (validateTime(time))
                    (*route).timeTo = time;
                else
                {
                    //printf(COLOR_RED "Error: bad input arrival time!" ANSI_RESET_ALL "\n");
                    validData = 0;
                }
                break;
            }
            case TAG_PRICE: {
                float price = atof(word);
                if (price && validatePrice(price))
                    (*route).price = price;
                else
                    //printf(COLOR_RED "Error: bad input price!" ANSI_RESET_ALL "\n");
                    break;
            }
            default:break;
            }
        word = strtok(NULL, " ,:");
    }
    return validData;
}
// ввод рейсов
void addRoutes(char* fileName)
{
    FILE* routeDB = fopen(fileName, "r+");
    if (routeDB == NULL) return;

    printf("Enter the data of routes as follows:\n");
    printf(COLOR_YELLOW "number" COLOR_MAGENTA " plane" COLOR_YELLOW " city" COLOR_MAGENTA " days of week" COLOR_YELLOW " departure time" COLOR_MAGENTA " arrival time" COLOR_YELLOW " price" ANSI_RESET_ALL"\n");
    printf("Enter \"###\" to end\n");

    char* str = (char*)calloc(MAX_ROUTE_LEN, sizeof(char));
    // пока не будет обнаружен символ ### и массив не закончился
    while (1)
    {
        Route route;
        gets_s(str, MAX_ROUTE_LEN);
        if (str != NULL && strcmp(str, END_INPUT) == 0) { break; fclose(routeDB); }


        if (serialiseRoute(str, &route))
        {
            if (fseek(routeDB, 0, SEEK_END) == 0)
                fprintf(routeDB, "%.10d %10.10s %10.10s %10.10s %.4d %.5d %.4d %.5d %10.2f",
                    route.num, route.plane, route.city, route.days,
                    route.timeFrom.hour, route.timeFrom.minute,
                    route.timeTo.hour, route.timeTo.minute, route.price);
            else
                printf(COLOR_RED "Error: File - data not Saved!" ANSI_RESET_ALL "\n");
        }
    }

    fclose(routeDB);
}

// печать рейсов
void printRoutes(char* fileName)
{
    FILE* routeDB = fopen(fileName, "r+");
    if (routeDB == NULL) return;
    rewind(routeDB);

    char* str = (char*)calloc(MAX_ROUTE_LEN, sizeof(char));
    Route route;
    int isFirstLine = 1;


    putchar(201);
    for (int i = 0; i < 5; i++) putchar(205);
    putchar(203);
    for (int i = 0; i < 11; i++) putchar(205);
    putchar(203);
    for (int i = 0; i < 14; i++) putchar(205);
    putchar(203);
    for (int i = 0; i < 9; i++) putchar(205);
    putchar(203);
    for (int i = 0; i < 8; i++) putchar(205);
    putchar(203);
    for (int i = 0; i < 8; i++) putchar(205);
    putchar(203);
    for (int i = 0; i < 12; i++) putchar(205);
    putchar(187);
    printf("\n");

    putchar(186);
    printf(STYLE_BOLD " %-3.3s " ANSI_RESET_ALL, "Num"); putchar(186);
    printf(STYLE_BOLD " %-9.9s " ANSI_RESET_ALL, "plane"); putchar(186);
    printf(STYLE_BOLD " %-12.12s " ANSI_RESET_ALL, "city"); putchar(186);
    printf(STYLE_BOLD " %-7.7s " ANSI_RESET_ALL, "days"); putchar(186);
    printf(STYLE_BOLD " %-6.6s " ANSI_RESET_ALL, "departure"); putchar(186);
    printf(STYLE_BOLD " %-6.6s " ANSI_RESET_ALL, "landing"); putchar(186);
    printf(STYLE_BOLD " %-10.10s " ANSI_RESET_ALL, "price"); putchar(186);
    putchar('\n');

    putchar(204);
    for (int i = 0; i < 5; i++) putchar(205);
    putchar(206);
    for (int i = 0; i < 11; i++) putchar(205);
    putchar(206);
    for (int i = 0; i < 14; i++) putchar(205);
    putchar(206);
    for (int i = 0; i < 9; i++) putchar(205);
    putchar(206);
    for (int i = 0; i < 8; i++) putchar(205);
    putchar(206);
    for (int i = 0; i < 8; i++) putchar(205);
    putchar(206);
    for (int i = 0; i < 12; i++) putchar(205);
    putchar(185);
    printf("\n");

    // Здесь работаем точно также как с strtok
    fgets(str, MAX_ROUTE_LEN, routeDB);
    while (serialiseRoute(str, &route))
    {
        if (isFirstLine) isFirstLine = 0;
        else {
            putchar(204);
            for (int i = 0; i < 5; i++) putchar(205);
            putchar(206);
            for (int i = 0; i < 11; i++) putchar(205);
            putchar(206);
            for (int i = 0; i < 14; i++) putchar(205);
            putchar(206);
            for (int i = 0; i < 9; i++) putchar(205);
            putchar(206);
            for (int i = 0; i < 8; i++) putchar(205);
            putchar(206);
            for (int i = 0; i < 8; i++) putchar(205);
            putchar(206);
            for (int i = 0; i < 12; i++) putchar(205);
            putchar(185);
            printf("\n");
        }

        putchar(186);
        printf(" %-3.d ", route.num); putchar(186);
        printf(" %-9.9s ", route.plane); putchar(186);
        printf(" %-12.12s ", route.city); putchar(186);
        printf(" %-7.7s ", route.days); putchar(186);
        printf(" %2.d:%-2.d  ", route.timeFrom.hour, route.timeFrom.minute); putchar(186);
        printf(" %2.d:%-2.d  ", route.timeTo.hour, route.timeTo.minute); putchar(186);
        printf(" %-10.2f ", route.price); putchar(186);
        putchar('\n');
        fgets(str, MAX_ROUTE_LEN, routeDB);
    }
    fclose(routeDB);

    putchar(200);
    for (int i = 0; i < 5; i++) putchar(205);
    putchar(202);
    for (int i = 0; i < 11; i++) putchar(205);
    putchar(202);
    for (int i = 0; i < 14; i++) putchar(205);
    putchar(202);
    for (int i = 0; i < 9; i++) putchar(205);
    putchar(202);
    for (int i = 0; i < 8; i++) putchar(205);
    putchar(202);
    for (int i = 0; i < 8; i++) putchar(205);
    putchar(202);
    for (int i = 0; i < 12; i++) putchar(205);
    putchar(188);

    if (isFirstLine)
    {
        system("CLS");
        printf(COLOR_RED "No data of routes!" ANSI_RESET_ALL);
    }
    printf("\n\n");


}

void changeRoute(char* fileName)
{
    Route route;

    int num, isFound;

    while (1)
    {
        system("CLS");
        printRoutes(fileName);
        printf("Enter \"###\" to end\n");
        printf("Properties:\n%d num\n%d plane\n%d city\n%d days\n%d departure\n%d landing\n%d price\n",
            TAG_NUM + 1, TAG_PLANE + 1, TAG_CITY + 1, TAG_DAYS_OF_WEEK + 1, TAG_TIME_FROM + 1, TAG_TIME_TO + 1, TAG_PRICE + 1);
        printf("Enter <num of route> <num of property> <new value> to change route: ");



        char* routeStr = (char*)calloc(100, sizeof(char));
        char* inputStr = (char*)calloc(100, sizeof(char));
        gets_s(inputStr, 100);

        inputStr = strtok(inputStr, " ,:");
        if (strcmp(inputStr, END_INPUT) == 0) break;
        if (inputStr == NULL || atoi(inputStr) == 0) continue;
        num = atoi(inputStr);

        inputStr = strtok(NULL, " ,:");
        if (inputStr == NULL || atoi(inputStr) == 0) continue;
        int property = atoi(inputStr) - 1;

        inputStr = strtok(NULL, "");

        isFound = 0;

        FILE* routeDB = fopen(fileName, "r+");
        if (routeDB == NULL) return;
        rewind(routeDB);

        fgets(routeStr, MAX_ROUTE_LEN, routeDB);
        while (serialiseRoute(routeStr, &route))
        {
            if (route.num == num)
            {   // нашли маршрут который нужно изменить
                int flag = 0;
                // валидация данных
                if (property == TAG_NUM)
                {
                    if (atoi(inputStr) != 0) flag = 1;
                }
                else if (property == TAG_PRICE)
                {
                    float price = atof(inputStr);
                    if (price && validatePrice(price)) flag = 1;
                }
                else if (property == TAG_DAYS_OF_WEEK)
                {
                    if (validateDays(inputStr)) flag = 1;
                }
                else if (property == TAG_TIME_FROM || property == TAG_TIME_TO)
                {
                    Time time;
                    char* t, timeStr[20];
                    strcpy(timeStr, inputStr);

                    t = strtok(timeStr, " .:");
                    time.hour = atoi(t);
                    t = strtok(NULL, " .:");
                    time.minute = atoi(t);

                    if (validateTime(time)) flag = 1;
                }
                else if (property == TAG_CITY || property == TAG_PLANE)  flag = 1;

                int fseekRes = fseek(routeDB, -MAX_ROUTE_LEN + 11 * property + 1, SEEK_CUR);
                if (flag == 1 && inputStr != NULL && fseekRes == 0)
                    fprintf(routeDB, "%10.10s", inputStr);
                break;
            }
            fgets(routeStr, MAX_ROUTE_LEN, routeDB);
        }
        fclose(routeDB);
    }


}

void deleteRoute(char* fileName)
{
    const char deleteName[] = "routeDB_old.txt";
    char* str = (char*)calloc(MAX_ROUTE_LEN, sizeof(char));
    Route route;

    int num, isFind;

    while (1)
    {
        system("CLS");
        printRoutes(fileName);
        printf("Enter \"###\" to end\n");
        printf("Enter num of route to delete: ");


        while (!(scanf("%s", str) && (strcmp(str, END_INPUT) == 0 || atoi(str) != 0)));
        while (getchar() != '\n');

        if (strcmp(str, END_INPUT) == 0) break;
        else num = atoi(str);


        isFind = 0;



        int renameResult = rename(fileName, deleteName);

        FILE* oldRouteDB = fopen(deleteName, "r");
        FILE* newRouteDB = fopen(fileName, "w");
        if (oldRouteDB == NULL || newRouteDB == NULL || renameResult != 0) return;
        rewind(oldRouteDB);
        rewind(newRouteDB);

        fgets(str, MAX_ROUTE_LEN, oldRouteDB);
        while (serialiseRoute(str, &route))
        {
            if (route.num != num) // копируем все, кроме которого хотим удалить
            {
                fprintf(newRouteDB, "%.10d %10.10s %10.10s %10.10s %.4d %.5d %.4d %.5d %10.2f",
                    route.num, route.plane, route.city, route.days,
                    route.timeFrom.hour, route.timeFrom.minute,
                    route.timeTo.hour, route.timeTo.minute, route.price);
            }
            fgets(str, MAX_ROUTE_LEN, oldRouteDB);
        }

        fclose(newRouteDB);
        fclose(oldRouteDB);
        remove(deleteName);
    }
}

int compareFloat(char* cmp, float p1, float p2)
{
    if (strcmp(cmp, ">") == 0) return p1 > p2;
    else if (strcmp(cmp, "<") == 0) return p1 < p2;
    else if (strcmp(cmp, ">=") == 0) return p1 >= p2;
    else if (strcmp(cmp, "<=") == 0) return p1 <= p2;
    else if (strcmp(cmp, "=") == 0) return p1 == p2;
    else return 0;
}
int compareTime(char* cmp, Time t1, Time t2)
{

    if (strcmp(cmp, ">") == 0) return t1.hour > t2.hour || t1.hour == t2.hour && t1.minute > t2.minute;
    else if (strcmp(cmp, "<") == 0) return t1.hour < t2.hour || t1.hour == t2.hour && t1.minute < t2.minute;
    else if (strcmp(cmp, ">=") == 0) return t1.hour > t2.hour || t1.hour == t2.hour && t1.minute >= t2.minute;
    else if (strcmp(cmp, "<=") == 0) return t1.hour < t2.hour || t1.hour == t2.hour && t1.minute <= t2.minute;
    else if (strcmp(cmp, "=") == 0) return t1.hour == t2.hour && t1.minute == t2.minute;
    else return 0;
}

int checkDay(int day, char days[10])
{
    int len = strlen(days);
    for (int i = 0; i < len; i++)
        if (day == int(days[i]) - 48)
            return 1;
    return 0;
}

void request(char* fileName)
{
    FILE* routeDB = fopen(fileName, "r+");
    if (routeDB == NULL) return;

    while (1)
    {
        int numFlag = 0,
            planeFlag = 0,
            cityFlag = 0,
            priceFlag = 0,
            timeToFlag = 0,
            timeFromFlag = 0,
            daysFlag = 0;
        Time timeFrom, timeTo;

        timeTo.hour = -1; timeTo.minute = -1;

        int inputFlag,
            num = 0,
            dayOfWeek;
        float price = 0;

        char
            comparePrice[10],
            compareTimeFrom[10],
            compareTimeTo[10];

        char plane[20],
            city[20];
        for (int property = 0; property < ROUTE_PROPERTIES; property++)
        {
            switch (property) {
            case TAG_NUM: {
                char str[20];
                do {
                    printf("Enter <num of route> or - to scip: ");
                    gets_s(str, 20);
                    num = atoi(str);
                } while (!(str != NULL && num != NULL || str[0] == '-'));

                if (num != NULL) numFlag = 1;
                break;
            }
            case TAG_PLANE: {
                char str[20];
                do {
                    printf("Enter <plane> or - to scip: ");
                    gets_s(str, 20);
                } while (!(str != NULL || str[0] == '-'));

                if (str[0] != '-')
                {
                    strcpy(plane, str);
                    planeFlag = 1;
                }
                break;
            }
            case TAG_CITY: {
                char str[20];
                do {
                    printf("Enter <city> or - to scip: ");
                    gets_s(str, 20);
                } while (!(str != NULL || str[0] == '-'));

                if (str[0] != '-')
                {
                    strcpy(city, str);
                    cityFlag = 1;
                }
                break;
            }
            case TAG_DAYS_OF_WEEK: {
                char str[8];
                do {
                    printf("Enter <day of week> or - to scip: ");
                    gets_s(str, 8);
                    dayOfWeek = atoi(str);
                } while (!(str != NULL && dayOfWeek > 0 && dayOfWeek <= 7 || str[0] == '-'));

                if (str[0] != '-') daysFlag = 1;
                break;
            }
            case TAG_TIME_FROM: {
                char str[20];
                do {
                    timeFrom.hour = -1; timeFrom.minute = -1;
                    printf("Enter <, >, =, <=, >=  and <hour> <minute> of TimeFrom or - to scip: ");
                    gets_s(str, 20);
                    if (str[0] == '-') break;

                    strcpy(compareTimeFrom, strtok(str, " ,:"));
                    if (compareTimeFrom != NULL)
                    {
                        char* word = strtok(NULL, " ,:");
                        if (word != NULL)
                        {
                            timeFrom.hour = atoi(word);
                            word = strtok(NULL, " ,:");
                            if (word != NULL)
                                timeFrom.minute = atoi(word);
                        }
                    }
                } while (!(validateTime(timeFrom) &&
                    (strcmp(compareTimeFrom, ">") == 0 ||
                        strcmp(compareTimeFrom, "<") == 0 ||
                        strcmp(compareTimeFrom, "=") == 0 ||
                        strcmp(compareTimeFrom, "<=") == 0 ||
                        strcmp(compareTimeFrom, ">=") == 0)
                    ));
                if (str[0] != '-') timeFromFlag = 1;
                break;
            }
            case TAG_TIME_TO: {
                char str[20];
                do {
                    timeTo.hour = -1; timeTo.minute = -1;
                    printf("Enter <, >, =, <=, >=  and <hour> <minute> of TimeTo or - to scip: ");
                    gets_s(str, 20);
                    if (str[0] == '-') break;

                    strcpy(compareTimeTo, strtok(str, " ,:"));
                    if (compareTimeTo != NULL)
                    {
                        char* word = strtok(NULL, " ,:");
                        if (word != NULL)
                        {
                            timeTo.hour = atoi(word);
                            word = strtok(NULL, " ,:");
                            if (word != NULL)
                                timeTo.minute = atoi(word);
                        }
                    }
                } while (!(validateTime(timeTo) &&
                    (strcmp(compareTimeTo, ">") == 0 ||
                        strcmp(compareTimeTo, "<") == 0 ||
                        strcmp(compareTimeTo, "=") == 0 ||
                        strcmp(compareTimeTo, "<=") == 0 ||
                        strcmp(compareTimeTo, ">=") == 0)
                    ));

                if (str[0] != '-') timeToFlag = 1;
                break;
            }
            case TAG_PRICE: {
                char str[20];
                do {
                    printf("Enter <, >, =, <=, >=  and <price> or - to scip: ");
                    gets_s(str, 20);
                    if (str[0] == '-') break;

                    strcpy(comparePrice, strtok(str, " ,:"));
                    char* word = strtok(NULL, " ,:");
                    if (word != NULL)
                        price = atof(word);

                } while (!(price != NULL && price > 0 &&
                    (strcmp(comparePrice, ">") == 0 ||
                        strcmp(comparePrice, "<") == 0 ||
                        strcmp(comparePrice, "=") == 0 ||
                        strcmp(comparePrice, "<=") == 0 ||
                        strcmp(comparePrice, ">=") == 0)
                    ));

                if (str[0] != '-') priceFlag = 1;
                break;
            }
            default:break;
            }
        }

        char requestFileName[] = "request.txt";
        FILE* requestRouteDB = fopen(requestFileName, "w");
        if (requestRouteDB == NULL) break;
        rewind(requestRouteDB);

        char* str = (char*)calloc(MAX_ROUTE_LEN, sizeof(char));
        Route route;

        rewind(routeDB);
        fgets(str, MAX_ROUTE_LEN, routeDB);
        while (serialiseRoute(str, &route))
        {
            if (
                (!numFlag || route.num == num) &&
                (!planeFlag || strcmp(route.plane, plane) == 0) &&
                (!cityFlag || strcmp(route.city, city) == 0) &&
                (!daysFlag || checkDay(dayOfWeek, route.days)) &&
                (!priceFlag || compareFloat(comparePrice, route.price, price)) &&
                (!timeFromFlag || compareTime(compareTimeFrom, route.timeFrom, timeFrom)) &&
                (!timeToFlag || compareTime(compareTimeTo, route.timeTo, timeTo))
                )
            { // сохраняем в новый файл соответсвующие запросу рейсы
                fprintf(requestRouteDB, "%.10d %10.10s %10.10s %10.10s %.4d %.5d %.4d %.5d %10.2f",
                    route.num, route.plane, route.city, route.days,
                    route.timeFrom.hour, route.timeFrom.minute,
                    route.timeTo.hour, route.timeTo.minute, route.price);
            }
            fgets(str, MAX_ROUTE_LEN, routeDB);
        }


        fclose(requestRouteDB);
        printRoutes(requestFileName);
        remove(requestFileName);

        printf("Enter \"###\" to end or more symbole to repeat: \n");
        char* action = (char*)calloc(10, sizeof(char));
        gets_s(action, 10);
        if (strcmp(action, END_INPUT) == 0) { break; fclose(routeDB); }
        system("CLS");
        printRoutes(fileName);
    }

    fclose(routeDB);
}

void menu()
{
    char fileName[] = "routeDB.txt";
    Route routes[MAX_ROUTES];
    int action, len = 0, isInputSuc;

    while (1)
    {
        system("CLS");
        printRoutes(fileName);

        printf(STYLE_BOLD "Choose num action" ANSI_RESET_ALL "\n"
            COLOR_GREEN "1) Add Route" ANSI_RESET_ALL "\n"
            COLOR_YELLOW "2) Change Routes " ANSI_RESET_ALL "\n"
            COLOR_RED "3) Delete Route" ANSI_RESET_ALL "\n"
            COLOR_YELLOW "4) Make request" ANSI_RESET_ALL "\n"
            "5) Close program\n"
        );

        do {
            isInputSuc = scanf("%d", &action);
            while (getchar() != '\n');
        } while (isInputSuc == 0 or 1 > action or action > 5);

        switch (action)
        {
        case 1:
        {
            addRoutes(fileName);
            break;
        }
        case 2:
        {
            changeRoute(fileName);
            break;
        }
        case 3:
        {
            deleteRoute(fileName);
            break;
        }
        case 4:
        {
            request(fileName);
            break;
        }
        case 5:
        {
            exit(0);
            break;
        }
        default: break;
        }

    }
}

int main()
{
    menu();
}